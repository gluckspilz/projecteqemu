/*  EQEMu:  Everquest Server Emulator
Copyright (C) 2001-2004  EQEMu Development Team (http://eqemu.org)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	  You should have received a copy of the GNU General Public License
	  along with this program; if not, write to the Free Software
	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "../common/debug.h"
#include "spdat.h"
#include "masterentity.h"
#include "../common/packet_dump.h"
#include "../common/moremath.h"
#include "../common/Item.h"
#include "worldserver.h"
#include "../common/skills.h"
#include "../common/bodytypes.h"
#include "../common/classes.h"
#include "../common/rulesys.h"
#include <math.h>
#include <assert.h>
#ifndef WIN32
#include <stdlib.h>
#include "../common/unix.h"
#endif

#include "StringIDs.h"

#ifdef EMBPERL
#include "embparser.h"
#endif

extern Zone* zone;
extern volatile bool ZoneLoaded;
#if !defined(NEW_LoadSPDat) && !defined(DB_LoadSPDat)
	extern SPDat_Spell_Struct spells[SPDAT_RECORDS];
#endif
extern bool spells_loaded;
extern WorldServer worldserver;
//uchar blah[]={0x0D,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
//uchar blah2[]={0x12,0x00,0x00,0x00,0x16,0x01,0x00,0x00};


// the spell can still fail here, if the buff can't stack
// in this case false will be returned, true otherwise
bool Mob::SpellEffect(Mob* caster, int16 spell_id, float partial)
{
	_ZP(Mob_SpellEffect);

	int caster_level, buffslot, effect, effect_value, i;
	ItemInst *SummonedItem=NULL;
#ifdef SPELL_EFFECT_SPAM
#define _EDLEN	200
	char effect_desc[_EDLEN];
#endif

	if(!IsValidSpell(spell_id))
		return false;

	const SPDat_Spell_Struct &spell = spells[spell_id];

	bool c_override = false;
	if(caster && caster->IsClient() && GetCastedSpellInvSlot() > 0)
	{
		const ItemInst* inst = caster->CastToClient()->GetInv().GetItem(GetCastedSpellInvSlot());
		if(inst)
		{
			if(inst->GetItem()->Click.Level > 0)
			{
				caster_level = inst->GetItem()->Click.Level;
				c_override = true;
			}
			else
			{
				caster_level = caster ? caster->GetCasterLevel(spell_id) : GetCasterLevel(spell_id);
			}
		}
		else
			caster_level = caster ? caster->GetCasterLevel(spell_id) : GetCasterLevel(spell_id);
	}
	else
		caster_level = caster ? caster->GetCasterLevel(spell_id) : GetCasterLevel(spell_id);

	if(c_override)
	{
		int durat = CalcBuffDuration(caster, this, spell_id, caster_level);
		if((durat-1) > 0)
		{
			buffslot = AddBuff(caster, spell_id, durat, caster_level);
			if(buffslot == -1)	// stacking failure
				return false;
		}
		else
		{
			buffslot = -2;
		}
	}
	else
	{
		if((CalcBuffDuration(caster,this,spell_id)-1) > 0){
			if(IsEffectInSpell(spell_id, SE_BindSight))
			{
				if(caster)
				{
					buffslot = caster->AddBuff(caster, spell_id);
				}
				else
					buffslot = -1;
			}
			else
			{
				buffslot = AddBuff(caster, spell_id);
			}
			if(buffslot == -1)	// stacking failure
				return false;
		} else {
			buffslot = -2;	//represents not a buff I guess
		}
	}

#ifdef SPELL_EFFECT_SPAM
		Message(0, "You are affected by spell '%s' (id %d)", spell.name, spell_id);
		if(buffslot >= 0)
		{
			Message(0, "Buff slot:  %d  Duration:  %d tics", buffslot, buffs[buffslot].ticsremaining);
		}
#endif

	if(buffslot >= 0) 
	{
		buffs[buffslot].melee_rune = 0;
		buffs[buffslot].magic_rune = 0;
		buffs[buffslot].numhits = 0;
		
		if(IsClient() && CastToClient()->GetClientVersionBit() & BIT_UnderfootAndLater)
		{
			EQApplicationPacket *outapp = MakeBuffsPacket(false);
			CastToClient()->FastQueuePacket(&outapp);
		}
	}

	if(IsNPC())
	{
		if(((PerlembParser*)parse)->SpellHasQuestSub(spell_id, "EVENT_SPELL_EFFECT_NPC"))
		{
			parse->Event(EVENT_SPELL_EFFECT_NPC, 0, itoa(spell_id), CastToNPC(), this, caster ? caster->GetID() : 0);
			CalcBonuses();
			return true;
		}
	}
	else if(IsClient())
	{
		if(((PerlembParser*)parse)->SpellHasQuestSub(spell_id, "EVENT_SPELL_EFFECT_CLIENT"))
		{
			parse->Event(EVENT_SPELL_EFFECT_CLIENT, 0, itoa(spell_id), NULL, this, caster ? caster->GetID() : 0);
			CalcBonuses();
			return true;
		}
	}
	
	if(spells[spell_id].viral_targets > 0) {
		if(!ViralTimer.Enabled()) 
			ViralTimer.Start(1000);
			
		has_virus = true;
		for(int i = 0; i < MAX_SPELL_TRIGGER*2; i+=2)
		{
			if(!viral_spells[i])
			{
				viral_spells[i] = spell_id;
				viral_spells[i+1] = caster->GetID();
				break;
			}
		}
	}

	if(spells[spell_id].numhits > 0)
		buffs[buffslot].numhits = spells[spell_id].numhits;
	
	// iterate through the effects in the spell
	for (i = 0; i < EFFECT_COUNT; i++)
	{
		if(IsBlankSpellEffect(spell_id, i))
			continue;

		effect = spell.effectid[i];
		effect_value = CalcSpellEffectValue(spell_id, i, caster_level, caster ? caster : this);

		if(spell_id == SPELL_LAY_ON_HANDS && caster && caster->GetAA(aaImprovedLayOnHands))
			effect_value = GetMaxHP();

#ifdef SPELL_EFFECT_SPAM
		effect_desc[0] = 0;
#endif

		switch(effect)
		{
			case SE_CurrentHP:	// nukes, heals; also regen/dot if a buff
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Hitpoints: %+i", effect_value);
#endif
				// SE_CurrentHP is calculated at first tick if its a dot/buff
				if (buffslot >= 0)
					break;

				// for offensive spells check if we have a spell rune on
				sint32 dmg = effect_value;
				if(dmg < 0)
				{
					// take partial damage into account
					dmg = (sint32) (dmg * partial / 100);

					//handles AAs and what not...
					if(caster)
					{
						dmg = GetVulnerability(dmg, caster, spell_id, 0);
						dmg = caster->GetActSpellDamage(spell_id, dmg);	
					}
					dmg = -dmg;
					Damage(caster, dmg, spell_id, spell.skill, false, buffslot, false);
				}
				else if(dmg > 0) {
					//healing spell...
					if(caster)
						dmg = caster->GetActSpellHealing(spell_id, dmg);

					HealDamage(dmg, caster);
				}

#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Hitpoints: %+i  actual: %+i", effect_value, dmg);
#endif
				break;
			}

			case SE_CurrentHPOnce:	// used in buffs usually, see Courage
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Hitpoints Once: %+i", effect_value);
#endif

				sint32 dmg = effect_value;
				if (spell_id == 2751 && caster) //Manaburn
				{
					dmg = caster->GetMana()*-3;
					caster->SetMana(0);
				} else if (spell_id == 2755 && caster) //Lifeburn
				{
					dmg = caster->GetHP()*-15/10;
					caster->SetHP(1);
					if(caster->IsClient()){
						caster->CastToClient()->SetFeigned(true);
						caster->SendAppearancePacket(AT_Anim, 115);
					}
				} 

				//do any AAs apply to these spells?
				if(dmg < 0) {
					dmg = -dmg;
					Damage(caster, dmg, spell_id, spell.skill, false, buffslot, false);
				} else {
					HealDamage(dmg, caster);
				}
				break;
			}

			case SE_PercentalHeal:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Percental Heal: %+i (%d%% max)", spell.max[i], effect_value);
#endif
				//im not 100% sure about this implementation.
				//the spell value forumula dosent work for these... at least spell 3232 anyways
				sint32 val = spell.max[i];

				if(caster)
					val = caster->GetActSpellHealing(spell_id, val);

				sint32 mhp = GetMaxHP();
				sint32 cap = mhp * spell.base[i] / 100;

				if(cap < val)
					val = cap;

				if(val > 0)
					HealDamage(val, caster);

				break;
			}

			case SE_CompleteHeal:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Complete Heal");
#endif
				//make sure they are not allready affected by this...
				//I think that is the point of making this a buff.
				//this is in the wrong spot, it should be in the immune
				//section so the buff timer does not get refreshed!

				int i;
				bool inuse = false;
				uint32 buff_count = GetMaxTotalSlots();
				for(i = 0; i < buff_count; i++) {
					if(buffs[i].spellid == spell_id && i != buffslot) {
						Message(0, "You must wait before you can be affected by this spell again.");
						inuse = true;
						break;
					}
				}
				if(inuse)
					break;

				Heal();
				break;
			}

			case SE_CurrentMana:
			{
				if(IsManaTapSpell(spell_id)) {
					if(GetCasterClass() != 'N') {
#ifdef SPELL_EFFECT_SPAM
						snprintf(effect_desc, _EDLEN, "Current Mana: %+i", effect_value);
#endif
						SetMana(GetMana() + effect_value);
						caster->SetMana(caster->GetMana() + abs(effect_value));
#ifdef SPELL_EFFECT_SPAM
						caster->Message(0, "You have gained %+i mana!", effect_value);
#endif
					}
				}
				else {
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Mana: %+i", effect_value);
#endif
				if (buffslot >= 0)
					break;

				SetMana(GetMana() + effect_value);
				}

				break;
			}
			
			case SE_CurrentManaOnce:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Mana Once: %+i", effect_value);
#endif
				SetMana(GetMana() + effect_value);
				break;
			}

			case SE_Translocate:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Translocate: %s %d %d %d heading %d",
					spell.teleport_zone, spell.base[1], spell.base[0],
					spell.base[2], spell.base[3]
				);
#endif
				if(IsClient())
				{

					if(caster) 
						CastToClient()->SendOPTranslocateConfirm(caster, spell_id);
					
				}
				break;
			}

			case SE_Succor:
			{
				float x, y, z, heading;
				const char *target_zone;

				x = spell.base[1];
				y = spell.base[0];
				z = spell.base[2];
				heading = spell.base[3];

				if(!strcmp(spell.teleport_zone, "same"))
				{
					target_zone = 0;
				}
				else
				{
					target_zone = spell.teleport_zone;
					if(IsNPC() && target_zone != zone->GetShortName()){
						if(!GetOwner()){
							CastToNPC()->Depop();
							break;
						}else{
							if(!GetOwner()->IsClient())
								CastToNPC()->Depop();
								break;
						}
					}
				}

				if(IsClient())
				{
					// Below are the spellid's for known evac/succor spells that send player
					// to the current zone's safe points.

					// Succor = 1567
					// Lesser Succor = 2183
					// Evacuate = 1628
					// Lesser Evacuate = 2184
					// Decession = 2558
					// Greater Decession = 3244
					// Egress = 1566

					if(!target_zone) {
#ifdef SPELL_EFFECT_SPAM
						LogFile->write(EQEMuLog::Debug, "Succor/Evacuation Spell In Same Zone.");
#endif
						if(IsClient())
							CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), x, y, z, heading, 0, EvacToSafeCoords);
						else
							GMMove(x, y, z, heading);
					}
					else {
#ifdef SPELL_EFFECT_SPAM
						LogFile->write(EQEMuLog::Debug, "Succor/Evacuation Spell To Another Zone.");
#endif
						if(IsClient())
							CastToClient()->MovePC(target_zone, x, y, z, heading);
					}
				}

				break;
			}
			case SE_YetAnotherGate: //Shin: Used on Teleport Bind.
			case SE_Teleport:	// gates, rings, circles, etc
			case SE_Teleport2:
			{
				float x, y, z, heading;
				const char *target_zone;

				x = spell.base[1];
				y = spell.base[0];
				z = spell.base[2];
				heading = spell.base[3];

				if(!strcmp(spell.teleport_zone, "same"))
				{
					target_zone = 0;
				}
				else
				{
					target_zone = spell.teleport_zone;

					if(IsNPC() && target_zone != zone->GetShortName()){
						if(!GetOwner()){
							CastToNPC()->Depop();
							break;
						}else{
							if(!GetOwner()->IsClient())
								CastToNPC()->Depop();
								break;
						}
					}
				}

				if (effect == SE_YetAnotherGate && caster->IsClient())
				{ //Shin: Teleport Bind uses caster's bind point
					x = caster->CastToClient()->GetBindX();
					y = caster->CastToClient()->GetBindY();
					z = caster->CastToClient()->GetBindZ();
					heading = caster->CastToClient()->GetBindHeading();
					//target_zone = caster->CastToClient()->GetBindZoneId(); target_zone doesn't work due to const char
					CastToClient()->MovePC(caster->CastToClient()->GetBindZoneID(), 0, x, y, z, heading);
					break;
				}

#ifdef SPELL_EFFECT_SPAM
				const char *efstr = "Teleport";
				if(effect == SE_Teleport)
					efstr = "Teleport v1";
				else if(effect == SE_Teleport2)
					efstr = "Teleport v2";
				else if(effect == SE_Succor)
					efstr = "Succor";

				snprintf(effect_desc, _EDLEN,
					"%s: %0.2f, %0.2f, %0.2f heading %0.2f in %s",
					efstr, x, y, z, heading, target_zone ? target_zone : "same zone"
				);
#endif
				if(IsClient())
				{
					if(!target_zone)
						CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), x, y, z, heading);
					else
						CastToClient()->MovePC(target_zone, x, y, z, heading);
				}
				else{
					if(!target_zone)
						GMMove(x, y, z, heading);
				}
				break;
			}

			case SE_Invisibility2:
			case SE_Invisibility:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Invisibility");
#endif
				// solar: TODO already invis message and spell kill from SpellOnTarget
				SetInvisible(true);
				break;
			}

			case SE_InvisVsAnimals:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Invisibility to Animals");
#endif
				invisible_animals = true;		// Mongrel: We're now invis to undead
				break;
			}

			case SE_InvisVsUndead2:
			case SE_InvisVsUndead:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Invisibility to Undead");
#endif
				invisible_undead = true;		// Mongrel: We're now invis to undead
				break;
			}

			case SE_FleshToBone:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Flesh To Bone");
#endif
				if(IsClient()){
					ItemInst* transI = CastToClient()->GetInv().GetItem(SLOT_CURSOR);
					if(transI && transI->IsType(ItemClassCommon) && transI->IsStackable()){
						uint32 fcharges = transI->GetCharges();
							//Does it sound like meat... maybe should check if it looks like meat too...
							if(strstr(transI->GetItem()->Name, "meat") ||
								strstr(transI->GetItem()->Name, "Meat") ||
								strstr(transI->GetItem()->Name, "flesh") ||
								strstr(transI->GetItem()->Name, "Flesh") ||
								strstr(transI->GetItem()->Name, "parts") ||
								strstr(transI->GetItem()->Name, "Parts")){
								CastToClient()->DeleteItemInInventory(SLOT_CURSOR, fcharges, true);
								CastToClient()->SummonItem(13073, fcharges);
							}
							else{
								Message(13, "You can only transmute flesh to bone.");
							}
						}
					else{
						Message(13, "You can only transmute flesh to bone.");
					}
				}
				break;
			}

			case SE_GroupFearImmunity:{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Group Fear Immunity");
#endif
				if(IsClient()){
					Group *g = entity_list.GetGroupByClient(CastToClient());
					uint32 time = spell.base[i]*10;
					if(g){
						for(int gi=0; gi < 6; gi++){
							if(g->members[gi] && g->members[gi]->IsClient())
							{
								g->members[gi]->CastToClient()->EnableAAEffect(aaEffectWarcry , time);
							}
						}
					}
					else{
						CastToClient()->EnableAAEffect(aaEffectWarcry , time);
					}
				}
				break;
			}

			case SE_AddFaction:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Faction Mod: %+i", effect_value);
#endif
				// EverHood
				if(caster && GetPrimaryFaction()>0) {
					caster->AddFactionBonus(GetPrimaryFaction(),spell.base[0]);
				}
				break;
			}

			case SE_Stun:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Stun: %d msec", effect_value);
#endif
				//Typically we check for immunities else where but since stun immunities are different and only
				//Block the stun part and not the whole spell, we do it here, also do the message here so we wont get the message on a resist
				if(SpecAttacks[UNSTUNABLE])
				{
					caster->Message_StringID(MT_Shout, IMMUNE_STUN);
				}
				else
				{
					int stun_resist = itembonuses.StunResist+spellbonuses.StunResist; 
					if(IsClient()) 
						stun_resist += aabonuses.StunResist;

					if(stun_resist <= 0 || MakeRandomInt(0,99) >= stun_resist) 
					{ 
						mlog(COMBAT__HITS, "Stunned. We had %d percent resist chance.", stun_resist);
						Stun(effect_value); 
					}
					else { 
						if(IsClient()) 
							Message_StringID(MT_Stun, SHAKE_OFF_STUN);
							
						mlog(COMBAT__HITS, "Stun Resisted. We had %d percent resist chance.", stun_resist);
					}
				}
				break;
			}

			case SE_Charm:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Charm: %+i (up to lvl %d)", effect_value, spell.max[i]);
#endif

				if (!caster)	// can't be someone's pet unless we know who that someone is
					break;

				if(IsNPC())
				{
					CastToNPC()->SaveGuardSpotCharm();
				}
				InterruptSpell();
				entity_list.RemoveDebuffs(this);
				entity_list.RemoveFromTargets(this);
				WipeHateList();

				if (IsClient() && caster->IsClient()) {
					caster->Message(0, "Unable to cast charm on a fellow player.");
					BuffFadeByEffect(SE_Charm);
					break;
				} else if(IsCorpse()) {
					caster->Message(0, "Unable to cast charm on a corpse.");
					BuffFadeByEffect(SE_Charm);
					break;
				} else if(caster->GetPet() != NULL && caster->IsClient()) {
					caster->Message(0, "You cannot charm something when you already have a pet.");
					BuffFadeByEffect(SE_Charm);
					break;
				} else if(GetOwner()) {
					caster->Message(0, "You cannot charm someone else's pet!");
					BuffFadeByEffect(SE_Charm);
					break;
				}

				Mob *my_pet = GetPet();
				if(my_pet)
				{
					my_pet->Kill();
				}

				caster->SetPet(this);
				SetOwnerID(caster->GetID());
				SetPetOrder(SPO_Follow);

				if(caster->IsClient()){
					EQApplicationPacket *app = new EQApplicationPacket(OP_Charm, sizeof(Charm_Struct));
					Charm_Struct *ps = (Charm_Struct*)app->pBuffer;
					ps->owner_id = caster->GetID();
					ps->pet_id = this->GetID();
					ps->command = 1;
					entity_list.QueueClients(this, app);
					safe_delete(app);
					SendPetBuffsToClient();
				}

				if (IsClient()) 
				{
					AI_Start();
					SendAppearancePacket(14, 100, true, true);
				} else if(IsNPC()) {
					CastToNPC()->SetPetSpellID(0);	//not a pet spell.
				}

				bool bBreak = false;

				// define spells with fixed duration
				// charm spells with -1 in field 209 are all of fixed duration, so lets use that instead of spell_ids
				if(spells[spell_id].field209 == -1)
					bBreak = true;

				if (!bBreak)
				{
					int resistMod = partial + (GetCHA()/25);
					resistMod = resistMod > 100 ? 100 : resistMod;
					buffs[buffslot].ticsremaining = resistMod * buffs[buffslot].ticsremaining / 100;
				}

				if(IsClient())
				{
					if(buffs[buffslot].ticsremaining > RuleI(Character, MaxCharmDurationForPlayerCharacter))
						buffs[buffslot].ticsremaining = RuleI(Character, MaxCharmDurationForPlayerCharacter);
				}

				break;
			}


			case SE_SenseDead:
			case SE_SenseSummoned:
			case SE_SenseAnimals:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Sense Target: %+i", effect_value);
#endif
				if(IsClient())
				{
					CastToClient()->SetSenseExemption(true);
				
					if(CastToClient()->GetClientVersionBit() & BIT_SoDAndLater)
					{
						bodyType bt = BT_Undead;

						int MessageID = SENSE_UNDEAD;

						if(effect == SE_SenseSummoned)
						{
							bt = BT_Summoned;
							MessageID = SENSE_SUMMONED;
						}
						else if(effect == SE_SenseAnimals)
						{
							bt = BT_Animal;
							MessageID = SENSE_ANIMAL;
						}

						Mob *ClosestMob = entity_list.GetClosestMobByBodyType(this, bt);

						if(ClosestMob)
						{
							Message_StringID(MT_Spells, MessageID);
							SetHeading(CalculateHeadingToTarget(ClosestMob->GetX(), ClosestMob->GetY()));
							SetTarget(ClosestMob);
							CastToClient()->SendTargetCommand(ClosestMob->GetID());
							SendPosUpdate(2);
						}
						else
							Message_StringID(clientMessageError, SENSE_NOTHING);
					}
				}
				break;
			}

			case SE_Fear:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Fear: %+i", effect_value);
#endif
				//use resistance value for duration...
				buffs[buffslot].ticsremaining = ((buffs[buffslot].ticsremaining * partial) / 100);

				if(IsClient())
				{
					if(buffs[buffslot].ticsremaining > RuleI(Character, MaxFearDurationForPlayerCharacter))
						buffs[buffslot].ticsremaining = RuleI(Character, MaxFearDurationForPlayerCharacter);
				}

				if(RuleB(Combat, EnableFearPathing)){
					if(IsClient())
					{
						AI_Start();
						animation = GetRunspeed() * 21; //set our animation to match our speed about
					}

					CalculateNewFearpoint();
					if(curfp) 
					{
						break;
					}
				} 
				else 
				{
					Stun(buffs[buffslot].ticsremaining * 6000 - (6000 - tic_timer.GetRemainingTime()));
				}
				break;
			}

			case SE_BindAffinity:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Bind Affinity");
#endif
				if (IsClient())
				{
					if(CastToClient()->GetGM() || RuleB(Character, BindAnywhere))
					{
						EQApplicationPacket *action_packet = new EQApplicationPacket(OP_Action, sizeof(Action_Struct));
						Action_Struct* action = (Action_Struct*) action_packet->pBuffer;
						EQApplicationPacket *message_packet = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
						CombatDamage_Struct *cd = (CombatDamage_Struct *)message_packet->pBuffer;
	
						action->target = GetID();
						action->source = caster ? caster->GetID() : GetID();
						action->level = 65;
						action->instrument_mod = 10;
						action->sequence = (GetHeading() * 12345 / 2);
						action->type = 231;
						action->spell = spell_id;
						action->buff_unknown = 4;

						cd->target = action->target;
						cd->source = action->source;
						cd->type = action->type;
						cd->spellid = action->spell;
						cd->sequence = action->sequence;

						CastToClient()->QueuePacket(action_packet);
						if(caster->IsClient() && caster != this)
							caster->CastToClient()->QueuePacket(action_packet);

						CastToClient()->QueuePacket(message_packet);
						if(caster->IsClient() && caster != this)
							caster->CastToClient()->QueuePacket(message_packet);

						CastToClient()->SetBindPoint();
						Save();
						safe_delete(action_packet);
						safe_delete(message_packet);
					}
					else
					{
						if(!zone->CanBind())
						{
							Message_StringID(13, CANNOT_BIND);
							break;
						}
						if(!zone->IsCity())
						{
							if(caster != this)
							{
								Message_StringID(13, CANNOT_BIND);
								break;
							}
							else
							{
								EQApplicationPacket *action_packet = new EQApplicationPacket(OP_Action, sizeof(Action_Struct));
								Action_Struct* action = (Action_Struct*) action_packet->pBuffer;
								EQApplicationPacket *message_packet = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
								CombatDamage_Struct *cd = (CombatDamage_Struct *)message_packet->pBuffer;
			
								action->target = GetID();
								action->source = caster ? caster->GetID() : GetID();
								action->level = 65;
								action->instrument_mod = 10;
								action->sequence = (GetHeading() * 12345 / 2);
								action->type = 231;
								action->spell = spell_id;
								action->buff_unknown = 4;

								cd->target = action->target;
								cd->source = action->source;
								cd->type = action->type;
								cd->spellid = action->spell;
								cd->sequence = action->sequence;

								CastToClient()->QueuePacket(action_packet);
								if(caster->IsClient() && caster != this)
									caster->CastToClient()->QueuePacket(action_packet);

								CastToClient()->QueuePacket(message_packet);
								if(caster->IsClient() && caster != this)
									caster->CastToClient()->QueuePacket(message_packet);

								CastToClient()->SetBindPoint();
								Save();
								safe_delete(action_packet);
								safe_delete(message_packet);
							}
						}
						else
						{
							EQApplicationPacket *action_packet = new EQApplicationPacket(OP_Action, sizeof(Action_Struct));
							Action_Struct* action = (Action_Struct*) action_packet->pBuffer;
							EQApplicationPacket *message_packet = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
							CombatDamage_Struct *cd = (CombatDamage_Struct *)message_packet->pBuffer;
		
							action->target = GetID();
							action->source = caster ? caster->GetID() : GetID();
							action->level = 65;
							action->instrument_mod = 10;
							action->sequence = (GetHeading() * 12345 / 2);
							action->type = 231;
							action->spell = spell_id;
							action->buff_unknown = 4;

							cd->target = action->target;
							cd->source = action->source;
							cd->type = action->type;
							cd->spellid = action->spell;
							cd->sequence = action->sequence;

							CastToClient()->QueuePacket(action_packet);
							if(caster->IsClient() && caster != this)
								caster->CastToClient()->QueuePacket(action_packet);

							CastToClient()->QueuePacket(message_packet);
							if(caster->IsClient() && caster != this)
								caster->CastToClient()->QueuePacket(message_packet);

							CastToClient()->SetBindPoint();
							Save();
							safe_delete(action_packet);
							safe_delete(message_packet);
						}
					}
				}
				break;
			}

			case SE_Gate:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Gate");
#endif			
				if(!spellbonuses.AntiGate)
					Gate();
				break;
			}

			case SE_CancelMagic:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Cancel Magic: %d", effect_value);
#endif
				uint32 buff_count = GetMaxTotalSlots();
				for(int slot = 0; slot < buff_count; slot++) {
					if(	buffs[slot].spellid != SPELL_UNKNOWN &&
						buffs[slot].durationformula != DF_Permanent &&
						spells[buffs[slot].spellid].dispel_flag < 1 &&
						!IsDiscipline(buffs[slot].spellid))
				    {
						BuffFadeBySlot(slot);
						slot = buff_count;
					}
				}
				break;
			}

			case SE_DispelDetrimental:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Dispel Detrimental: %d", effect_value);
#endif
				uint32 buff_count = GetMaxTotalSlots();
				for(int slot = 0; slot < buff_count; slot++) {
					if (buffs[slot].spellid != SPELL_UNKNOWN &&
						buffs[slot].durationformula != DF_Permanent &&
				    	IsDetrimentalSpell(buffs[slot].spellid) &&
						spells[buffs[slot].spellid].dispel_flag < 1)
				    {
						BuffFadeBySlot(slot);
						slot = buff_count;
					}
				}
				break;
			}
			
			case SE_DispelBeneficial:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Dispel Beneficial: %d", effect_value);
#endif
				uint32 buff_count = GetMaxTotalSlots();
				for(int slot = 0; slot < buff_count; slot++) {
					if (buffs[slot].spellid != SPELL_UNKNOWN &&
						buffs[slot].durationformula != DF_Permanent &&
				    	IsBeneficialSpell(buffs[slot].spellid) &&
						spells[buffs[slot].spellid].dispel_flag < 1)
				    {
						BuffFadeBySlot(slot);
						slot = buff_count;
					}
				}
				break;
			}

			case SE_Mez:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Mesmerize");
#endif
				Mesmerize();
				break;
			}

			case SE_SummonItem:
			{
				const Item_Struct *item = database.GetItem(spell.base[i]);
#ifdef SPELL_EFFECT_SPAM
				const char *itemname = item ? item->Name : "*Unknown Item*";
				snprintf(effect_desc, _EDLEN, "Summon Item: %s (id %d)", itemname, spell.base[i]);
#endif
				if(IsClient()) 
				{
					Client *c=CastToClient();
					if (c->CheckLoreConflict(item))  {
						Message_StringID(0,PICK_LORE);
					} else {
						int charges;
						if (spell.formula[i] < 100)
						{
							charges = spell.formula[i];
						}
						else	// variable charges
						{
							charges = CalcSpellEffectValue_formula(spell.formula[i], 0, 20, caster_level, spell_id);
						}
						charges = (spell.formula[i] < 100) ? charges : (charges > 20) ? 20 : (spell.max[i] < 1) ? item->MaxCharges : spell.max[i];
						if (SummonedItem) {
							c->PushItemOnCursor(*SummonedItem);
							c->SendItemPacket(SLOT_CURSOR, SummonedItem, ItemPacketSummonItem);
							safe_delete(SummonedItem);
						}
						SummonedItem=database.CreateItem(spell.base[i],charges);
					}
				}

				break;
			}
			case SE_SummonItemIntoBag:
			{
				const Item_Struct *item = database.GetItem(spell.base[i]);
#ifdef SPELL_EFFECT_SPAM
				const char *itemname = item ? item->Name : "*Unknown Item*";
				snprintf(effect_desc, _EDLEN, "Summon Item In Bag: %s (id %d)", itemname, spell.base[i]);
#endif
				uint8 slot;

				if (!SummonedItem || !SummonedItem->IsType(ItemClassContainer)) {
					if(caster) caster->Message(13,"SE_SummonItemIntoBag but no bag has been summoned!");
				} else if ((slot=SummonedItem->FirstOpenSlot())==0xff) {
					if(caster) caster->Message(13,"SE_SummonItemIntoBag but no room in summoned bag!");
				} else if (IsClient()) {
					if (CastToClient()->CheckLoreConflict(item))  {
						Message_StringID(0,PICK_LORE);
					} else {
						int charges;
						if (spell.formula[i] < 100)
						{
							charges = spell.formula[i];
						}
						else	// variable charges
						{
							charges = CalcSpellEffectValue_formula(spell.formula[i], 0, 20, caster_level, spell_id);
						}
						charges = charges < 1 ? 1 : (charges > 20 ? 20 : charges);
						ItemInst *SubItem=database.CreateItem(spell.base[i],charges);
						if (SubItem!=NULL) {
							SummonedItem->PutItem(slot,*SubItem);
							safe_delete(SubItem);
						}
					}
				}

				break;
			}

			case SE_SummonBSTPet:
			case SE_NecPet:
			case SE_SummonPet:
			case SE_Familiar:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Summon %s: %s", (effect==SE_Familiar)?"Familiar":"Pet", spell.teleport_zone);
#endif
				if(GetPet())
				{
					Message_StringID(MT_Shout, ONLY_ONE_PET);
				}
				else
				{
					MakePet(spell_id, spell.teleport_zone);
				}
				break;
			}

			case SE_DivineAura:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Invulnerability");
#endif
				if(spell_id==4789) // Touch of the Divine - Divine Save
					buffs[buffslot].ticsremaining = spells[spell_id].buffduration; // Prevent focus/aa buff extension

				SetInvul(true);
				break;
			}

			case SE_ShadowStep:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Shadow Step: %d", effect_value);
#endif
				if(IsNPC())	// see Song of Highsun - sends mob home
				{
					Gate();
				}
				// solar: shadow step is handled by client already, nothing required
				break;
			}

			case SE_Blind:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Blind: %+i", effect_value);
#endif
				if (spells[spell_id].base[i] == 1)
					BuffFadeByEffect(SE_Blind);
				// solar: handled by client
				// TODO: blind flag?
				break;
			}

			case SE_Rune:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Melee Absorb Rune: %+i", effect_value);
#endif
				buffs[buffslot].melee_rune = effect_value;	
					SetHasRune(true);
				break;
			}

			case SE_AbsorbMagicAtt:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Spell Absorb Rune: %+i", effect_value);
#endif
				if(effect_value > 0) {
				buffs[buffslot].magic_rune = effect_value;	
					SetHasSpellRune(true);
				}
				break;
			}

			case SE_MitigateMeleeDamage:
			{
				buffs[buffslot].melee_rune = GetPartialMeleeRuneAmount(spell_id);
				SetHasPartialMeleeRune(true);
				break;
			}

			case SE_MitigateSpellDamage:
			{
				buffs[buffslot].magic_rune = GetPartialMagicRuneAmount(spell_id);
				SetHasPartialSpellRune(true);
				break;
			}

			case SE_Levitate:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Levitate");
#endif
				//this sends the levitate packet to everybody else
				//who does not otherwise receive the buff packet.
				SendAppearancePacket(AT_Levitate, 2, true, true);
				break;
			}

			case SE_Illusion:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Illusion: race %d", effect_value);
#endif
				// Gender Illusions
				if(spell.base[i] == -1) {
					// Specific Gender Illusions
					if(spell_id == 1732 || spell_id == 1731) {
						int specific_gender = -1;
						// Male
						if(spell_id == 1732) 
							specific_gender = 0;
						// Female
						else if (spell_id == 1731) 
							specific_gender = 1;
						if(specific_gender > -1) {
							if(caster && caster->GetTarget()) {
								SendIllusionPacket
								(
									caster->GetTarget()->GetBaseRace(),
									specific_gender,
									caster->GetTarget()->GetTexture()
								);
							}
						}
					}
					// Change Gender Illusions
					else {
						if(caster && caster->GetTarget()) {
							int opposite_gender = 0;
							if(caster->GetTarget()->GetGender() == 0) 
								opposite_gender = 1;
							
							SendIllusionPacket
							(
								caster->GetTarget()->GetRace(),
								opposite_gender,
								caster->GetTarget()->GetTexture()
							);
						}
					}
				}
				// Racial Illusions
				else {
					SendIllusionPacket
					(
						spell.base[i],
						Mob::GetDefaultGender(spell.base[i], GetGender()),
						spell.base2[i]
					);
					if(spell.base[i] == OGRE){
						SendAppearancePacket(AT_Size, 9);
					}
					else if(spell.base[i] == TROLL){
						SendAppearancePacket(AT_Size, 8);
					}
					else if(spell.base[i] == VAHSHIR || spell.base[i] == BARBARIAN){
						SendAppearancePacket(AT_Size, 7);
					}
					else if(spell.base[i] == HALF_ELF || spell.base[i] == WOOD_ELF || spell.base[i] == DARK_ELF || spell.base[i] == FROGLOK){
						SendAppearancePacket(AT_Size, 5);
					}
					else if(spell.base[i] == DWARF){
						SendAppearancePacket(AT_Size, 4);
					}
					else if(spell.base[i] == HALFLING || spell.base[i] == GNOME){
						SendAppearancePacket(AT_Size, 3);
					}
					else if(spell.base[i] == WOLF) {
						SendAppearancePacket(AT_Size, 2);
					}
					else{
						SendAppearancePacket(AT_Size, 6);
					}
				}
				for(int x = 0; x < 7; x++){
					SendWearChange(x);
				}
				if(caster && caster->GetAA(aaPermanentIllusion))
					buffs[buffslot].persistant_buff = 1;
				else
					buffs[buffslot].persistant_buff = 0;
				break;
			}

			case SE_IllusionCopy:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Illusion Copy");
#endif
				if(caster && caster->GetTarget()){
						SendIllusionPacket
						(
							caster->GetTarget()->GetRace(),
							caster->GetTarget()->GetGender(),
							caster->GetTarget()->GetTexture()
						);
						caster->SendAppearancePacket(AT_Size, caster->GetTarget()->GetSize());
						for(int x = 0; x < 7; x++){
							caster->SendWearChange(x);
						}
				}
			}

			case SE_WipeHateList:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Memory Blur: %d", effect_value);
#endif
				int wipechance = spells[spell_id].base[i];
				if(MakeRandomInt(0, 100) < wipechance)
				{
					if(IsAIControlled())
					{
						WipeHateList();
					}
					Message(13, "Your mind fogs. Who are my friends? Who are my enemies?... it was all so clear a moment ago...");
				}
				break;
			}

			case SE_SpinTarget:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Spin: %d", effect_value);
#endif
				// solar: the spinning is handled by the client
				int max_level = spells[spell_id].max[i];
				if(max_level == 0)
					max_level = 55; // Default max is 55 level limit
					
				if(SpecAttacks[UNSTUNABLE] || (GetLevel() > max_level)) 
				{
					caster->Message_StringID(MT_Shout, IMMUNE_STUN);
				}
				else
				{
					// solar: the spinning is handled by the client
					// Stun duration is based on the effect_value, not the buff duration(alot don't have buffs)
					Stun(effect_value);
					if(!IsClient()) {
						Spin();
						spun_timer.Start(100); // spins alittle every 100 ms
					}
				}
				break;
			}

			case SE_EyeOfZomm:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Eye of Zomm");
#endif
				if(caster && caster->IsClient()) {
					char eye_name[64];
					snprintf(eye_name, sizeof(eye_name), "Eye_of_%s", caster->GetCleanName());
					int duration = CalcBuffDuration(caster, this, spell_id) * 6;
					caster->TemporaryPets(spell_id, NULL, eye_name, duration);
				}
				break;
			}

			case SE_ReclaimPet:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Reclaim Pet");
#endif
				if
				(
					IsNPC() &&
					GetOwnerID() &&		// I'm a pet
					caster &&					// there's a caster
					caster->GetID() == GetOwnerID()	&& // and it's my master
					GetPetType() != petCharmed
				)
				{
					int lvlmod = 4;
					if(caster->IsClient() && caster->CastToClient()->GetAA(aaImprovedReclaimEnergy))
						lvlmod = 8;	//this is an unconfirmed number, I made it up
					if(caster->IsClient() && caster->CastToClient()->GetAA(aaImprovedReclaimEnergy2))
						lvlmod = 8;	//this is an unconfirmed number, I made it up
					caster->SetMana(caster->GetMana()+(GetLevel()*lvlmod));

					if(caster->IsClient())
						caster->CastToClient()->SetPet(0);
					SetOwnerID(0);	// this will kill the pet
				}
				break;
			}

			case SE_BindSight:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Bind Sight");
#endif
				if(caster && caster->IsClient())
				{
					caster->CastToClient()->SetBindSightTarget(this);
				}
				break;
			}

			case SE_FeignDeath:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Feign Death");
#endif
				//todo, look up spell ID in DB
				if(spell_id == 2488)   //Dook- Lifeburn fix
					break;

				if(IsClient())
					CastToClient()->SetFeigned(true);
				break;
			}

			case SE_VoiceGraft:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Voice Graft");
#endif
				//should set a flag to make this easy to check.
				//dont know how to make it expire when the spell fades right now...
				//const char *msg = "Voice Graft is not implemented.";
				//if(caster) caster->Message(13, msg);
				break;
			}

			case SE_Sentinel:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Sentinel");
#endif
				if(caster)
				{
					if(caster == this)
					{
						Message_StringID(MT_Spells,
							SENTINEL_TRIG_YOU);
					}
					else
					{
						caster->Message_StringID(MT_Spells,
							SENTINEL_TRIG_OTHER, GetCleanName());
					}
				}
				break;
			}

			case SE_LocateCorpse:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Locate Corpse");
#endif
				// This is handled by the client prior to SoD.
				//
				if(IsClient() && (CastToClient()->GetClientVersionBit() & BIT_SoDAndLater))
					CastToClient()->LocateCorpse();

				break;
			}

			case SE_Revive:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Revive");	// heh the corpse won't see this
#endif
				if (IsCorpse() && CastToCorpse()->IsPlayerCorpse()) {

					if(caster)
						mlog(SPELLS__REZ, " corpse being rezzed using spell %i by %s",
						     spell_id, caster->GetName());

					CastToCorpse()->CastRezz(spell_id, caster);
				}
				break;
			}

			case SE_ModelSize:
			case SE_ChangeHeight:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Model Size: %d%%", effect_value);
#endif
				ChangeSize(GetSize() * (effect_value / 100.0));
				break;
			}

			case SE_Root:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Root: %+i", effect_value);
#endif
				rooted = true;
				break;
			}

			case SE_SummonHorse:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Summon Mount: %s", spell.teleport_zone);
#endif
				if(IsClient())	// NPCs can't ride
				{
					CastToClient()->SummonHorse(spell_id);
				}
				break;
			}

			case SE_SummonCorpse:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Summon Corpse: %d", effect_value);
#endif
				// can only summon corpses of clients
				if(IsClient()) {
					Client* TargetClient = 0;
					if(this->GetTarget())
						TargetClient = this->GetTarget()->CastToClient();
					else
						TargetClient = this->CastToClient();

					// We now have a valid target for this spell. Either the caster himself or a targetted player. Lets see if the target is in the group.
					Group* group = entity_list.GetGroupByClient(TargetClient);
					if(group) {
						if(!group->IsGroupMember(TargetClient)) {
							Message(13, "Your target must be a group member for this spell.");
							break;
						}
					}
					else {
						if(TargetClient != this->CastToClient()) {
							Message(13, "Your target must be a group member for this spell.");
							break;
						}
					}

					// Now we should either be casting this on self or its being cast on a valid group member
					if(TargetClient) {
						Corpse *corpse = entity_list.GetCorpseByOwner(TargetClient);
						if(corpse) {
							if(TargetClient == this->CastToClient())
								Message_StringID(4, SUMMONING_CORPSE, TargetClient->CastToMob()->GetCleanName());
							else
								Message_StringID(4, SUMMONING_CORPSE_OTHER, TargetClient->CastToMob()->GetCleanName());
							
							corpse->Summon(CastToClient(), true, true);
						}
						else {
							// No corpse found in the zone
							Message_StringID(4, CORPSE_CANT_SENSE);
						}
					}
					else {
						Message_StringID(4, TARGET_NOT_FOUND);
						LogFile->write(EQEMuLog::Error, "%s attempted to cast spell id %u with spell effect SE_SummonCorpse, but could not cast target into a Client object.", GetCleanName(), spell_id);
					}
				}

				break;
			}
			case SE_AddMeleeProc:
			case SE_WeaponProc:
			{
				uint16 procid = GetProcID(spell_id, i);
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Weapon Proc: %s (id %d)", spells[effect_value].name, procid);
#endif

				if(spells[spell_id].base2[i] == 0)
					AddProcToWeapon(procid, false, 100);
				else
					AddProcToWeapon(procid, false, spells[spell_id].base2[i]+100);
				break;
			}

			case SE_NegateAttacks:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Melee Negate Attack Rune: %+i", effect_value);
#endif
				buffs[buffslot].numhits = effect_value;
				break;				  
			}
			case SE_AppraiseLDonChest:
			{
				if(IsNPC())
				{
					int check = spell.max[0];
					int target = spell.targettype;
					if(target == ST_LDoNChest_Cursed)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNSenseTraps(CastToNPC(), check, LDoNTypeCursed);
						}
					}
					else if(target == ST_Target)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNSenseTraps(CastToNPC(), check, LDoNTypeMagical);
						}
					}
				}			
				break;
			}

			case  SE_DisarmLDoNTrap:
			{
				if(IsNPC())
				{
					int check = spell.max[0];
					int target = spell.targettype;
					if(target == ST_LDoNChest_Cursed)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNDisarm(CastToNPC(), check, LDoNTypeCursed);
						}
					}
					else if(target == ST_Target)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNDisarm(CastToNPC(), check, LDoNTypeMagical);
						}
					}
				}	
				break;
			}

			case SE_UnlockLDoNChest:
			{
				if(IsNPC())
				{
					int check = spell.max[0];
					int target = spell.targettype;
					if(target == ST_LDoNChest_Cursed)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNPickLock(CastToNPC(), check, LDoNTypeCursed);
						}
					}
					else if(target == ST_Target)
					{
						if(caster && caster->IsClient())
						{
							caster->CastToClient()->HandleLDoNPickLock(CastToNPC(), check, LDoNTypeMagical);
						}
					}
				}	
				break;
			}
	
			case SE_Lull:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Lull");
#endif
				// TODO: check vs. CHA when harmony effect failed, if caster is to be added to hatelist
				break;
			}
	
			case SE_PoisonCounter:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Poison Counter: %+i", effect_value);
#endif
				if (effect_value > 0)
					buffs[buffslot].poisoncounters = effect_value;
				else
				{
					effect_value = 0 - effect_value;
					uint32 buff_count = GetMaxTotalSlots();
					for (int j=0; j < buff_count; j++) {
						if (buffs[j].spellid >= (int16)SPDAT_RECORDS)
							continue;
						if (buffs[j].poisoncounters == 0)
							continue;
						if (effect_value >= buffs[j].poisoncounters) {
							if (caster)
								caster->Message(MT_Spells,"You have cured your target from %s!",spells[buffs[j].spellid].name);
							effect_value -= buffs[j].poisoncounters;
							buffs[j].poisoncounters = 0;
							BuffFadeBySlot(j);
						} else {
							buffs[j].poisoncounters -= effect_value;
							effect_value = 0;
							break;
						}
					}
				}
				break;
			}

			case SE_DiseaseCounter:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Disease Counter: %+i", effect_value);
#endif
				if (effect_value > 0)
					buffs[buffslot].diseasecounters = effect_value;
				else
				{
					effect_value = 0 - effect_value;
					uint32 buff_count = GetMaxTotalSlots();
					for (int j=0; j < buff_count; j++) {
						if (buffs[j].spellid >= (int16)SPDAT_RECORDS)
							continue;
						if (buffs[j].diseasecounters == 0)
							continue;
						if (effect_value >= buffs[j].diseasecounters)
						{
							if (caster)
								caster->Message(MT_Spells,"You have cured your target from %s!",spells[buffs[j].spellid].name);
							effect_value -= buffs[j].diseasecounters;
							buffs[j].diseasecounters = 0;
							BuffFadeBySlot(j);
						}
						else
						{
							buffs[j].diseasecounters -= effect_value;
							effect_value = 0;
							break;
						}
					}
				}
				break;
			}

			case SE_CurseCounter:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Curse Counter: %+i", effect_value);
#endif
				if (effect_value > 0)
					buffs[buffslot].cursecounters = effect_value;
				else
				{
					effect_value = 0 - effect_value;
					uint32 buff_count = GetMaxTotalSlots();
					for (int j=0; j < buff_count; j++) {
						if (buffs[j].spellid >= (int16)SPDAT_RECORDS)
							continue;
						if (buffs[j].cursecounters == 0)
							continue;
						if (effect_value >= buffs[j].cursecounters)
						{
							if (caster)
								caster->Message(MT_Spells,"You have cured your target from %s!",spells[buffs[j].spellid].name);
							effect_value -= buffs[j].cursecounters;
							buffs[j].cursecounters = 0;
							BuffFadeBySlot(j);
						}
						else
						{
							buffs[j].cursecounters -= effect_value;
							effect_value = 0;
							break;
						}
					}
				}
				break;
			}
			
			case SE_CorruptionCounter:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Corruption Counter: %+i", effect_value);
#endif
				if (effect_value > 0)
					buffs[buffslot].corruptioncounters = effect_value;
				else
				{
					effect_value = -effect_value;
					uint32 buff_count = GetMaxTotalSlots();
					for (int j=0; j < buff_count; j++) {
						if (buffs[j].spellid >= (int16)SPDAT_RECORDS)
							continue;
						if (buffs[j].corruptioncounters == 0)
							continue;
						if (effect_value >= buffs[j].corruptioncounters) {
							if (caster)
								caster->Message(MT_Spells,"You have cured your target from %s!",spells[buffs[j].spellid].name);
							effect_value -= buffs[j].corruptioncounters;
							buffs[j].corruptioncounters = 0;
							BuffFadeBySlot(j);
						} else {
							buffs[j].corruptioncounters -= effect_value;
							effect_value = 0;
							break;
						}
					}
				}
				break;
			}

			case SE_Destroy:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Destroy");
#endif
				if(IsNPC()) {
					if(GetLevel() <= 52)
						CastToNPC()->Depop();
					else
						Message(13, "Your target is too high level to be affected by this spell.");
				}
				break;
			}

			case SE_TossUp:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Toss Up: %d", effect_value);
#endif
				double toss_amt = (double)spells[spell_id].base[i];
				if(toss_amt < 0)
					toss_amt = -toss_amt;

				if(IsNPC())
				{
					Stun(toss_amt);
				}
				toss_amt = sqrt(toss_amt)-2.0;

				if(toss_amt < 0.0)
					toss_amt = 0.0;

				if(toss_amt > 20.0)
					toss_amt = 20.0;

				if(IsClient())
				{
					CastToClient()->SetKnockBackExemption(true);
				}

				double look_heading = GetHeading();
				look_heading /= 256;
				look_heading *= 360;
				look_heading += 180;
				if(look_heading > 360)
					look_heading -= 360;

				//x and y are crossed mkay
				double new_x = spells[spell_id].pushback * sin(double(look_heading * 3.141592 / 180.0));
				double new_y = spells[spell_id].pushback * cos(double(look_heading * 3.141592 / 180.0));

				EQApplicationPacket* outapp_push = new EQApplicationPacket(OP_ClientUpdate, sizeof(PlayerPositionUpdateServer_Struct));
				PlayerPositionUpdateServer_Struct* spu = (PlayerPositionUpdateServer_Struct*)outapp_push->pBuffer;

				spu->spawn_id	= GetID();
				spu->x_pos		= FloatToEQ19(GetX());
				spu->y_pos		= FloatToEQ19(GetY());
				spu->z_pos		= FloatToEQ19(GetZ());
				spu->delta_x	= NewFloatToEQ13(new_x);
				spu->delta_y	= NewFloatToEQ13(new_y);
				spu->delta_z	= NewFloatToEQ13(toss_amt);
				spu->heading	= FloatToEQ19(GetHeading());
				spu->padding0002	=0;
				spu->padding0006	=7;
				spu->padding0014	=0x7f;
				spu->padding0018	=0x5df27;
				spu->animation = 0;
				spu->delta_heading = NewFloatToEQ13(0);
				outapp_push->priority = 5;
				entity_list.QueueClients(this, outapp_push, true);
				if(IsClient())
					CastToClient()->FastQueuePacket(&outapp_push);

				break;
			}

			case SE_StopRain:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Stop Rain");
#endif
				zone->zone_weather = 0;
				zone->weatherSend();
				break;
			}

			case SE_Sacrifice:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Sacrifice");
#endif
				if(!IsClient() || !caster->IsClient()){
					break;
				}
				CastToClient()->SacrificeConfirm(caster->CastToClient());
				break;
			}

			case SE_SummonPC:
			{
			if(IsClient()){
					CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), caster->GetX(), caster->GetY(), caster->GetZ(), caster->GetHeading(), 2, SummonPC);
					Message(15, "You have been summoned!");
					entity_list.ClearAggro(this);
				}
				else
					caster->Message(13, "This spell can only be cast on players.");

				break;
			}

			case SE_Silence:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Silence");
#endif
				Silence(true);
				break;
			}
			
			case SE_Amnesia:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Amnesia");
#endif
				Amnesia(true);
				break;
			}

			case SE_CallPet:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Call Pet");
#endif
				// solar: this is cast on self, not on the pet
				if(GetPet() && GetPet()->IsNPC())
				{
					GetPet()->CastToNPC()->GMMove(GetX(), GetY(), GetZ(), GetHeading());
				}
				break;
			}
			
			case SE_StackingCommand_Block:
			case SE_StackingCommand_Overwrite:
			{
				// solar: these are special effects used by the buff stuff
				break;
			}


			case SE_TemporaryPets:         //Dook- swarms and wards:
			{
				// EverHood - this makes necro epic 1.5/2.0 proc work properly
				if((spell_id != 6882) && (spell_id != 6884)) // Chaotic Jester/Steadfast Servant
				{
					char pet_name[64];
					snprintf(pet_name, sizeof(pet_name), "%s`s pet", caster->GetCleanName());
					caster->TemporaryPets(spell_id, this, pet_name);
				}
				else
					caster->TemporaryPets(spell_id, this, NULL);
				break;
			}

			case SE_FadingMemories:		//Dook- escape etc
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Fading Memories");
#endif
				entity_list.RemoveFromTargets(caster);
				SetInvisible(true);
				break;
			}

			case SE_RangedProc:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Ranged Proc: %+i", effect_value);
#endif
				uint16 procid = GetProcID(spell_id, i);

				AddRangedProc(procid, spells[spell_id].base2[i]);
				
				break;		
			}
			
			case SE_Rampage:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Rampage");
#endif
				if(caster) 
					entity_list.AEAttack(caster, 30); // on live wars dont get a duration ramp, its a one shot deal

				break;
			}
			
			case SE_AEMelee:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Duration Rampage");
#endif
				if (caster && caster->IsClient()) { // will tidy this up later so that NPCs can duration ramp from spells too  
					CastToClient()->DurationRampage(effect_value*12);
				}
				break;
			}
			
			case SE_AETaunt://Dook- slapped it in the spell effect so client does the animations
			{			// and incase there are similar spells we havent found yet
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "AE Taunt");
#endif
				if(caster && caster->IsClient())
					entity_list.AETaunt(caster->CastToClient());
				break;
			}

			case SE_SkillAttack:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Skill Attack");
#endif
				const ItemInst* itm = NULL;
				sint32 tDmg = 0; //total damage done
				int mDmg = 1; //minimume damage done
				
				/* for this, spells[spell_id].base2[i] is the damage for the attack "weapon damage" */
				
				//if we are a client, do stuff involving clients attacks
				if(caster->IsClient())
				{
					//which skill are we using, get the damage based on that skill
					switch(spells[spell_id].skill)
					{
						case THROWING:
							caster->CastToClient()->GetThrownDamage(spells[spell_id].base[i], tDmg, mDmg);
							break;
						case ARCHERY:
							itm = caster->CastToClient()->GetInv().GetItem(SLOT_RANGE);
							if(itm)
								tDmg = effect_value + itm->GetItem()->Damage * 2 + (itm->GetItem()->Damage * (GetSkill(spells[spell_id].skill) + spells[spell_id].base[i] + GetDEX()) / 225);
							break;
						case BASH:
							itm = caster->CastToClient()->GetInv().GetItem(SLOT_SECONDARY);
							if(itm)
								tDmg = effect_value + ((itm->GetItem()->AC/4)+1) * 2 + (((itm->GetItem()->AC/4)+1) * (GetSkill(spells[spell_id].skill) + GetSTR()) / 225);
							break;
						case KICK:
						case FLYING_KICK:
						case ROUND_KICK:
							itm = caster->CastToClient()->GetInv().GetItem(SLOT_FEET);
							if(itm)
								tDmg = effect_value + ((itm->GetItem()->AC / 2) + 1) * 2 + (((itm->GetItem()->AC / 2) + 1) * (GetSkill(spells[spell_id].skill) + GetSTR()) / 225);
							break;
						case HAND_TO_HAND:
						case EAGLE_STRIKE:
						case TIGER_CLAW:
							itm = caster->CastToClient()->GetInv().GetItem(SLOT_HANDS);
							if(itm)
								tDmg = effect_value + ((itm->GetItem()->AC / 2) + 1) + (((itm->GetItem()->AC / 2) + 1) * (GetSkill(spells[spell_id].skill) + GetSTR()) / 225);
							break;
						default:
							itm = caster->CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
							if(itm)
								tDmg = effect_value + itm->GetItem()->Damage * 2 + (itm->GetItem()->Damage * (GetSkill(spells[spell_id].skill) + GetSTR()) / 225);
							break;
					}
					
				}
				
				if(tDmg == 0)
					tDmg = effect_value;
				
				//these are considered magical attacks, so we don't need to test that
				//if they are resistable that's been taken care of, all these discs have a 10000 hit chance so they auto hit, no need to test
				if(RuleB(Combat, UseIntervalAC))
					caster->DoSpecialAttackDamage(this, spells[spell_id].skill, tDmg, mDmg, (mDmg / 20));
				else
					caster->DoSpecialAttackDamage(this, spells[spell_id].skill, MakeRandomInt(1, tDmg), mDmg, (mDmg / 20));
				break;
			}

			case SE_WakeTheDead:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Wake The Dead");
#endif
				//meh dupe issue with npc casting this
				if(caster->IsClient()){
					//this spell doesn't appear to actually contain the information on duration inside of it oddly
					int dur = 60;
					if(spell_id == 3269)
						dur += 15;
					else if(spell_id == 3270)
						dur += 30;

					caster->WakeTheDead(spell_id, caster->GetTarget(), dur);
				}
				break;
			}

			case SE_Doppelganger:         
			{
				if(caster && caster->IsClient()) {
					char pet_name[64];
					snprintf(pet_name, sizeof(pet_name), "%s`s doppelganger", caster->GetCleanName());
					int pet_count = spells[spell_id].base[i];
					int pet_duration = spells[spell_id].max[i];
					caster->CastToClient()->Doppelganger(spell_id, this, pet_name, pet_count, pet_duration);
				}
				break;
			}

			case SE_DefensiveProc:
			{
				uint16 procid = GetProcID(spell_id, i);
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Defensive Proc: %s (id %d)", spells[effect_value].name, procid);
#endif
				AddDefensiveProc(procid, spells[spell_id].base2[i]);
				break;
			}

			case SE_BardAEDot:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Bard AE Dot: %+i", effect_value);
#endif
				// SE_CurrentHP is calculated at first tick if its a dot/buff
				if (buffslot >= 0)
					break;

				// for offensive spells check if we have a spell rune on
				sint32 dmg = effect_value;
				if(dmg < 0)
				{
					// take partial damage into account
					dmg = (sint32) (dmg * partial / 100);

					//handles AAs and what not...
					//need a bard version of this prolly...
					//if(caster)
					//	dmg = caster->GetActSpellDamage(spell_id, dmg);

					dmg = -dmg;
					Damage(caster, dmg, spell_id, spell.skill, false, buffslot, false);
				} else if(dmg > 0) {
					//healing spell...
					if(caster)
						dmg = caster->GetActSpellHealing(spell_id, dmg);
					HealDamage(dmg, caster);
				}
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Hitpoints: %+i  actual: %+i", effect_value, dmg);
#endif
				break;
			}

			case SE_CurrentEndurance: {
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Endurance: %+i", effect_value);
#endif
				if(IsClient()) {
					CastToClient()->SetEndurance(CastToClient()->GetEndurance() + effect_value);
				}
				break;
			}

			case SE_CurrentEnduranceOnce:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Current Endurance Once: %+i", effect_value);
#endif

				if(IsClient()) {
					CastToClient()->SetEndurance(CastToClient()->GetEndurance() + effect_value);
				}
				break;
			}

			case SE_BalanceHP: {
				if(!caster)
					break;

				if(!caster->IsClient())
					break;

				Raid *r = entity_list.GetRaidByClient(caster->CastToClient());
				if(r)
				{
					int32 gid = 0xFFFFFFFF;
					gid = r->GetGroup(caster->GetName());
					if(gid < 11)
					{
						r->BalanceHP(spell.base[i], gid);
						break;
					}
				}

				Group *g = entity_list.GetGroupByClient(caster->CastToClient());

				if(!g)
					break;

				g->BalanceHP(spell.base[i]);
				break;
			}

			case SE_DeathSave: {
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Death Save: %+i", effect_value);
#endif
				if(caster) {
					int8 SuccessChance = 0;
					float BaseChance = 0.00f;
					float BonusChance = 0.00f;

					switch(effect_value) {
						case 1: {
							BaseChance = 0.10f;
							break;
						}
						case 2: {
							BaseChance = 0.30f;
							break;
						}
						default: {
							LogFile->write(EQEMuLog::Error, "Unknown SE_DeathSave effect value in spell: %s (%i).", spells[spell_id].name, spell_id);
						}
					}

					switch(caster->GetAA(aaUnfailingDivinity)) {
						case 1: {
							BonusChance = 0.10f;
							break;
						}
						case 2: {
							BonusChance = 0.20f;
							break;
						}
						case 3: {
							BonusChance = 0.30f;
							break;
						}
					}

					SuccessChance = (((float)GetCHA() * 0.0005f) + BaseChance + BonusChance) * 100;
						
#ifdef SPELL_EFFECT_SPAM
					snprintf(effect_desc, _EDLEN, "Death Save Chance: %+i", SuccessChance);
#endif
					buffs[buffslot].deathSaveSuccessChance = SuccessChance;
					buffs[buffslot].casterAARank = caster->GetAA(aaUnfailingDivinity);
					SetDeathSaveChance(true);
				}

				break;
			}

			case SE_SummonAndResAllCorpses:
			{
				if(IsClient())
					CastToClient()->SummonAndRezzAllCorpses();

				break;
			}

			case SE_GateToHomeCity:
			{
				if(IsClient())
					CastToClient()->GoToBind(4);
				break;
			}

			case SE_SuspendMinion:
			case SE_SuspendPet:
			{
				if(IsClient())
					CastToClient()->SuspendMinion();

				break;
			}
			
			case SE_Forceful_Rejuv:
			{
				if(IsClient()) {
					for(unsigned int i =0 ; i < MAX_PP_MEMSPELL; ++i) {
						if(IsValidSpell(CastToClient()->m_pp.mem_spells[i])) {
							CastToClient()->m_pp.spellSlotRefresh[i] = 1;
							CastToClient()->GetPTimers().Clear(&database, (pTimerSpellStart + CastToClient()->m_pp.mem_spells[i]));
						}
					}
					SetMana(GetMana());
				}
				break;
			}
			
			case SE_HealFromMana:
			{
				int heal_amount = 0;
				if(caster) {
					sint32 mana_to_use = caster->GetMana() - spell.base[i];
					if(mana_to_use > -1) {
						caster->SetMana(caster->GetMana() - spell.base[i]);
						// we get the full amount of the heal, which is broken into 10 mana increments
						heal_amount = spell.base[i] / 10 * spell.base2[i];
					}
					else {
						heal_amount = GetMana() / 10 * spell.base2[i];
						caster->SetMana(0);
					}
				}
				HealDamage(heal_amount, caster);
				break;
			}
			
			case SE_ManaDrainWithDmg:
			{
				int mana_damage = 0;
				sint32 mana_to_use = GetMana() - spell.base[i];
				if(mana_to_use > -1) {
					SetMana(GetMana() - spell.base[i]);
					// we take full dmg(-10 to make the damage the right sign)
					mana_damage = spell.base[i] / -10 * spell.base2[i];
					Damage(caster, mana_damage, spell_id, spell.skill, false, i, true);
				}
				else {
					mana_damage = GetMana() / -10 * spell.base2[i];
					SetMana(0);
					Damage(caster, mana_damage, spell_id, spell.skill, false, i, true);
				}
				break;
			}
			
			case SE_EndDrainWithDmg:
			{
				if(IsClient()) {
					int end_damage = 0;
					sint32 end_to_use = CastToClient()->GetEndurance() - spell.base[i];
					if(end_to_use > -1) {
						CastToClient()->SetEndurance(CastToClient()->GetEndurance() - spell.base[i]);
						// we take full dmg(-10 to make the damage the right sign)
						end_damage = spell.base[i] / -10 * spell.base2[i];
						Damage(caster, end_damage, spell_id, spell.skill, false, i, true);
					}
					else {
						end_damage = CastToClient()->GetEndurance() / -10 * spell.base2[i];
						CastToClient()->SetEndurance(0);
						Damage(caster, end_damage, spell_id, spell.skill, false, i, true);
					}
				}
				break;
			}
			
			case SE_SetBodyType:
			{
				SetBodyType((bodyType)spell.base[i]);
				break;
			}
			
			case SE_Leap:
			{
				// These effects remove lev and only work a certain distance away.
				BuffFadeByEffect(SE_Levitate);
				if (caster && caster->GetTarget()) {
					float my_x = caster->GetX();
					float my_y = caster->GetY();
					float my_z = caster->GetZ();
					float target_x = GetX();
					float target_y = GetY();
					float target_z = GetZ();
					if ((CalculateDistance(my_x, my_y, my_z) > 10) &&
						(CalculateDistance(my_x, my_y, my_z) < 75) &&
						(caster->CheckLosFN(caster->GetTarget()))) 
					{
						float value, x_vector, y_vector, hypot;
						
						value = (float)spell.base[i]; // distance away from target

						x_vector = target_x - my_x;
						y_vector = target_y - my_y;
						hypot = sqrt(x_vector*x_vector + y_vector*y_vector);
					
						x_vector /= hypot;
						y_vector /= hypot;
					
						my_x = target_x - (x_vector * value);
						my_y = target_y - (y_vector * value);
		
						float new_ground = GetGroundZ(my_x, my_y);
		
						if(caster->IsClient()) 
							caster->CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), my_x, my_y, new_ground, GetHeading()*2);
						else
							caster->GMMove(my_x, my_y, new_ground, GetHeading());
					}
				}
				break;
			}
			
			// Handled Elsewhere
			case SE_ImmuneFleeing:
			case SE_BlockSpellEffect:
			case SE_Knockdown: // handled by client
			case SE_ShadowStepDirectional: // handled by client
			case SE_SpellOnDeath:
			case SE_BlockNextSpellFocus:
			case SE_ReduceReuseTimer:
			case SE_SwarmPetDuration:
			case SE_LimitHPPercent:
			case SE_LimitManaPercent:
			case SE_LimitEndPercent:
			case SE_ExtraAttackChance:
			case SE_ProcChance:
			case SE_StunResist:
			case SE_MinDamageModifier:
			case SE_DamageModifier:
			case SE_HitChance:
			case SE_MeleeSkillCheck:
			case SE_HundredHands:
			case SE_ResistFearChance:
			case SE_ResistSpellChance:
			case SE_AllInstrumentMod:
			case SE_MeleeLifetap:
			case SE_DoubleAttackChance:
			case SE_DualWieldChance:
			case SE_ParryChance:
			case SE_DodgeChance:
			case SE_RiposteChance:
			case SE_AvoidMeleeChance:
			case SE_CrippBlowChance:
			case SE_CriticalHitChance:
			case SE_MeleeMitigation:
			case SE_Reflect:
			case SE_Screech:
			case SE_SingingSkill:
			case SE_MagicWeapon:
			case SE_Hunger:
			case SE_MagnifyVision:
			case SE_Lycanthropy:
			case SE_NegateIfCombat:
			case SE_CastingLevel:
			case SE_RaiseStatCap:
			case SE_ResistAll:
			case SE_ResistMagic:
			case SE_ResistDisease:
			case SE_ResistPoison:
			case SE_ResistCold:
			case SE_ResistFire:
			case SE_AllStats:
			case SE_CHA:
			case SE_WIS:
			case SE_INT:
			case SE_STA:
			case SE_AGI:
			case SE_DEX:
			case SE_STR:
			case SE_ATK:
			case SE_ArmorClass:
			case SE_EndurancePool:
			case SE_Stamina:
			case SE_UltraVision:
			case SE_InfraVision:
			case SE_ManaPool:
			case SE_TotalHP:
			case SE_ChangeFrenzyRad:
			case SE_Harmony:
			case SE_ChangeAggro:
			case SE_Hate2:
			case SE_Identify:
			case SE_Calm:
			case SE_SpellDamageShield:
			case SE_ReverseDS:
			case SE_DamageShield:
			case SE_TrueNorth:
			case SE_WaterBreathing:
			case SE_MovementSpeed:
			case SE_HealOverTime:
			case SE_PercentXPIncrease:
			case SE_SeeInvis: // client
			case SE_DivineSave:
			case SE_Accuracy:
			case SE_Flurry:
			case SE_AttackSpeed:
			case SE_AttackSpeed2:
			case SE_AttackSpeed3:
			case SE_Purify:
			case SE_ImprovedDamage:
			case SE_ImprovedHeal:
			case SE_IncreaseSpellHaste:
			case SE_IncreaseSpellDuration:
			case SE_IncreaseRange:
			case SE_SpellHateMod:
			case SE_ReduceReagentCost:
			case SE_ReduceManaCost:
			case SE_LimitMaxLevel:
			case SE_LimitResist:
			case SE_LimitTarget:
			case SE_LimitEffect:
			case SE_LimitSpellType:
			case SE_LimitSpell:
			case SE_LimitMinDur:
			case SE_LimitInstant:
			case SE_LimitMinLevel:
			case SE_LimitCastTime:
			case SE_LimitManaCost:
			case SE_CombatSkills:
			case SE_SpellDurationIncByTic:
			case SE_TriggerOnCast:
			case SE_HealRate:
			case SE_SkillDamageTaken:
			case SE_SpellVulnerability:
			case SE_SpellTrigger:
			case SE_ApplyEffect:
			case SE_Twincast:
			case SE_InterruptCasting:
			case SE_ImprovedSpellEffect:
			case SE_BossSpellTrigger:
			case SE_CastOnWearoff:
			case SE_EffectOnFade:
			case SE_MaxHPChange:
			case SE_SympatheticProc:
			case SE_SpellDamage:
			case SE_CriticalSpellChance:
			case SE_SpellCritChance:
			case SE_SpellCritDmgIncrease:
			case SE_CriticalHealChance:
			case SE_CriticalHealOverTime:
			case SE_CriticalDoTChance:
			case SE_SpellOnKill:
			case SE_CriticalDamageMob:
			case SE_LimitSpellGroup:
			case SE_ResistCorruption:
			case SE_ReduceSkillTimer:
			case SE_HPToMana:
			case SE_ManaAbsorbPercentDamage:
			case SE_SkillDamageAmount:
			case SE_GravityEffect:
			case SE_IncreaseBlockChance:
			case SE_AntiGate:
			case SE_Fearless:
			{
				break;
			}

			default:
			{
#ifdef SPELL_EFFECT_SPAM
				snprintf(effect_desc, _EDLEN, "Unknown Effect ID %d", effect);
#else
				Message(0, "Unknown spell effect %d in spell %s (id %d)", effect, spell.name, spell_id);
#endif
			}
		}
#ifdef SPELL_EFFECT_SPAM
		Message(0, ". . . Effect #%i: %s", i + 1, (effect_desc && effect_desc[0]) ? effect_desc : "Unknown");
#endif
	}

	CalcBonuses();

	if (SummonedItem) {
		Client *c=CastToClient();
		c->PushItemOnCursor(*SummonedItem);
		c->SendItemPacket(SLOT_CURSOR, SummonedItem, ItemPacketSummonItem);
		safe_delete(SummonedItem);
	}

	return true;
}

int Mob::CalcSpellEffectValue(int16 spell_id, int effect_id, int caster_level, Mob *caster, int ticsremaining)
{
	int formula, base, max, effect_value;

	if
	(
		!IsValidSpell(spell_id) ||
		effect_id < 0 ||
		effect_id >= EFFECT_COUNT
	)
		return 0;

	formula = spells[spell_id].formula[effect_id];
	base = spells[spell_id].base[effect_id];
	max = spells[spell_id].max[effect_id];

	if(IsBlankSpellEffect(spell_id, effect_id))
		return 0;

	effect_value = CalcSpellEffectValue_formula(formula, base, max, caster_level, spell_id, ticsremaining);

	if(caster && IsBardSong(spell_id) &&
	(spells[spell_id].effectid[effect_id] != SE_AttackSpeed) &&
	(spells[spell_id].effectid[effect_id] != SE_AttackSpeed2) &&
	(spells[spell_id].effectid[effect_id] != SE_AttackSpeed3)) {
		int oval = effect_value;
		int mod = caster->GetInstrumentMod(spell_id);
		effect_value = effect_value * mod / 10;
		mlog(SPELLS__BARDS, "Effect value %d altered with bard modifier of %d to yeild %d", oval, mod, effect_value);
	}

	return(effect_value);
}

// solar: generic formula calculations
int Mob::CalcSpellEffectValue_formula(int formula, int base, int max, int caster_level, int16 spell_id, int ticsremaining)
{
/*
neotokyo: i need those formulas checked!!!!

0 = base
1 - 99 = base + level * formulaID
100 = base
101 = base + level / 2
102 = base + level
103 = base + level * 2
104 = base + level * 3
105 = base + level * 4
106 ? base + level * 5
107 ? min + level / 2
108 = min + level / 3
109 = min + level / 4
110 = min + level / 5
119 ? min + level / 8
121 ? min + level / 4
122 = splurt
123 ?
203 = stacking issues ? max
205 = stacking issues ? 105


  0x77 = min + level / 8
*/

	int result = 0, updownsign = 1, ubase = base;
	if(ubase < 0)
		ubase = 0 - ubase;

	// solar: this updown thing might look messed up but if you look at the
	// spells it actually looks like some have a positive base and max where
	// the max is actually less than the base, hence they grow downward
/*
This seems to mainly catch spells where both base and max are negative.
Strangely, damage spells  have a negative base and positive max, but
snare has both of them negative, yet their range should work the same:
(meaning they both start at a negative value and the value gets lower)
*/
	if (max < base && max != 0)
	{
		// values are calculated down
		updownsign = -1;
	}
	else
	{
		// values are calculated up
		updownsign = 1;
	}

	mlog(SPELLS__EFFECT_VALUES, "CSEV: spell %d, formula %d, base %d, max %d, lvl %d. Up/Down %d",
		spell_id, formula, base, max, caster_level, updownsign);

	switch(formula)
	{
		case 60:	//used in stun spells..?
		case 70:
			result = ubase/100; break;
		case   0:
		case 100:	// solar: confirmed 2/6/04
			result = ubase; break;
		case 101:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + (caster_level / 2)); break;
		case 102:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + caster_level); break;
		case 103:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + (caster_level * 2)); break;
		case 104:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + (caster_level * 3)); break;
		case 105:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + (caster_level * 4)); break;

		case 107:
			//Used on Reckless Strength, I think it should decay over time
			result = updownsign * (ubase + (caster_level / 2)); break;
		case 108:
			result = updownsign * (ubase + (caster_level / 3)); break;
		case 109:	// solar: confirmed 2/6/04
			result = updownsign * (ubase + (caster_level / 4)); break;

		case 110:	// solar: confirmed 2/6/04
			//is there a reason we dont use updownsign here???
			result = ubase + (caster_level / 5); break;

		case 111:
            result = updownsign * (ubase + 6 * (caster_level - GetMinLevel(spell_id))); break;
		case 112:
            result = updownsign * (ubase + 8 * (caster_level - GetMinLevel(spell_id))); break;
		case 113:
            result = updownsign * (ubase + 10 * (caster_level - GetMinLevel(spell_id))); break;
		case 114:
            result = updownsign * (ubase + 15 * (caster_level - GetMinLevel(spell_id))); break;

        //these formula were updated according to lucy 10/16/04
		case 115:	// solar: this is only in symbol of transal
			result = ubase + 6 * (caster_level - GetMinLevel(spell_id)); break;
		case 116:	// solar: this is only in symbol of ryltan
            result = ubase + 8 * (caster_level - GetMinLevel(spell_id)); break;
		case 117:	// solar: this is only in symbol of pinzarn
            result = ubase + 12 * (caster_level - GetMinLevel(spell_id)); break;
		case 118:	// solar: used in naltron and a few others
            result = ubase + 20 * (caster_level - GetMinLevel(spell_id)); break;

		case 119:	// solar: confirmed 2/6/04
			result = ubase + (caster_level / 8); break;
		case 121:	// solar: corrected 2/6/04
			result = ubase + (caster_level / 3); break;
		case 122: {
			int ticdif = spells[spell_id].buffduration - (ticsremaining-1);
			if(ticdif < 0)
				ticdif = 0;
			result = -(11 + 11*ticdif);
			break;
		}
		case 123:	// solar: added 2/6/04
			result = MakeRandomInt(ubase, abs(max));
			break;

		//these are used in stacking effects... formula unknown
		case 201:
		case 203:
			result = max;
			break;
		default:
			if (formula < 100)
				result = ubase + (caster_level * formula);
			else if((formula >= 2000) && (formula <= 2650))
			{
				// Source: http://crucible.samanna.net/viewtopic.php?f=38&t=6259
				result = ubase * (caster_level * (formula - 2000) + 1);
			}
			else
				LogFile->write(EQEMuLog::Debug, "Unknown spell effect value forumula %d", formula);
	}

	int oresult = result;

	// now check result against the allowed maximum
	if (max != 0)
	{
		if (updownsign == 1)
		{
			if (result > max)
				result = max;
		}
		else
		{
			if (result < max)
				result = max;
		}
	}

	// if base is less than zero, then the result need to be negative too
	if (base < 0 && result > 0)
		result *= -1;

	mlog(SPELLS__EFFECT_VALUES, "Result: %d (orig %d), cap %d %s", result, oresult, max, (base < 0 && result > 0)?"Inverted due to negative base":"");

	return result;
}


void Mob::BuffProcess()
{
	uint32 buff_count = GetMaxTotalSlots();

	for (int buffs_i = 0; buffs_i < buff_count; ++buffs_i)
	{
		if (buffs[buffs_i].spellid != SPELL_UNKNOWN)
		{
			DoBuffTic(buffs[buffs_i].spellid, buffs[buffs_i].ticsremaining, buffs[buffs_i].casterlevel, entity_list.GetMob(buffs[buffs_i].casterid));
			// If the Mob died during DoBuffTic, then the buff we are currently processing will have been removed
			if(buffs[buffs_i].spellid == SPELL_UNKNOWN)
				continue;

			if(buffs[buffs_i].durationformula != DF_Permanent)
			{
				if(!zone->BuffTimersSuspended() || IsDetrimentalSpell(buffs[buffs_i].spellid))
				{
					--buffs[buffs_i].ticsremaining;
					
					if (buffs[buffs_i].ticsremaining == 0) {
						if (!IsShortDurationBuff(buffs[buffs_i].spellid) || 
							IsFearSpell(buffs[buffs_i].spellid) || 
							IsCharmSpell(buffs[buffs_i].spellid) || 
							IsMezSpell(buffs[buffs_i].spellid) ||
							IsBlindSpell(buffs[buffs_i].spellid))
						{
							mlog(SPELLS__BUFFS, "Buff %d in slot %d has expired. Fading.", buffs[buffs_i].spellid, buffs_i);
							BuffFadeBySlot(buffs_i);
						}
					}
					else if (buffs[buffs_i].ticsremaining < 0)
					{
						mlog(SPELLS__BUFFS, "Buff %d in slot %d has expired. Fading.", buffs[buffs_i].spellid, buffs_i);
						BuffFadeBySlot(buffs_i);
					}
					else
					{
						mlog(SPELLS__BUFFS, "Buff %d in slot %d has %d tics remaining.", buffs[buffs_i].spellid, buffs_i, buffs[buffs_i].ticsremaining);
					}
				}
				else if(IsClient() && !(CastToClient()->GetClientVersionBit() & BIT_SoFAndLater))
				{
					buffs[buffs_i].UpdateClient = true;
				}
			}

			if(IsClient())
			{
				if(buffs[buffs_i].UpdateClient == true)
				{
					CastToClient()->SendBuffDurationPacket(buffs[buffs_i].spellid, buffs[buffs_i].ticsremaining, buffs[buffs_i].casterlevel);
					buffs[buffs_i].UpdateClient = false;
				}
			}
		}
	}
}

void Mob::DoBuffTic(int16 spell_id, int32 ticsremaining, int8 caster_level, Mob* caster) {
	_ZP(Mob_DoBuffTic);

	int effect, effect_value;

	if(!IsValidSpell(spell_id))
		return;

	const SPDat_Spell_Struct &spell = spells[spell_id];

	if (spell_id == SPELL_UNKNOWN)
		return;

	if(IsNPC())
	{
		if(((PerlembParser*)parse)->SpellHasQuestSub(spell_id, "EVENT_SPELL_EFFECT_BUFF_TIC_NPC"))
		{
			parse->Event(EVENT_SPELL_EFFECT_BUFF_TIC_NPC, 0, itoa(spell_id), CastToNPC(), this, caster ? caster->GetID() : 0);
			return;
		}
	}
	else
	{
		if(((PerlembParser*)parse)->SpellHasQuestSub(spell_id, "EVENT_SPELL_EFFECT_BUFF_TIC_CLIENT"))
		{
			parse->Event(EVENT_SPELL_EFFECT_BUFF_TIC_CLIENT, 0, itoa(spell_id), NULL, this, caster ? caster->GetID() : 0);
			return;
		}
	}
	
	// Check for non buff spell effects to fade
	// AE melee effects
	if(IsClient())
		CastToClient()->CheckAAEffect(aaEffectRampage);

	for (int i=0; i < EFFECT_COUNT; i++)
	{
		if(IsBlankSpellEffect(spell_id, i))
			continue;

		effect = spell.effectid[i];
		//I copied the calculation into each case which needed it instead of
		//doing it every time up here, since most buff effects dont need it

		switch(effect)
		{
		case SE_CurrentHP:
		{
			effect_value = CalcSpellEffectValue(spell_id, i, caster_level, caster, ticsremaining);

			//TODO: account for AAs and stuff

			//dont know what the signon this should be... - makes sense
			if (caster && caster->IsClient() &&
				IsDetrimentalSpell(spell_id) &&
				effect_value < 0) {
				sint32 modifier = 100;
				modifier += caster->CastToClient()->GetFocusEffect(focusImprovedDamage, spell_id);

				if(caster){
					if(caster->IsClient() && !caster->CastToClient()->GetFeigned()){
						AddToHateList(caster, -effect_value);
					}
					else if(!caster->IsClient())
					{
						if(!IsClient())
							AddToHateList(caster, -effect_value);
					}
					effect_value = GetVulnerability(effect_value, caster, spell_id, ticsremaining);
					caster->TryDotCritical(spell_id, effect_value);
				}
				effect_value = effect_value * modifier / 100;
			}

			if(effect_value < 0) {
				if(caster && !caster->IsClient())
					effect_value = GetVulnerability(effect_value, caster, spell_id, ticsremaining);
				effect_value = -effect_value;
				Damage(caster, effect_value, spell_id, spell.skill, false, i, true);
			} else if(effect_value > 0) {
				// Regen spell...
				// handled with bonuses
			}
			break;
		}
		case SE_HealOverTime:
		{
			effect_value = CalcSpellEffectValue(spell_id, i, caster_level);
			if(caster)
				effect_value = caster->GetActSpellHealing(spell_id, effect_value);
			effect_value += effect_value * (itembonuses.HealRate + spellbonuses.HealRate) / 100;
			HealDamage(effect_value, caster);
			//healing aggro would go here; removed for now
			break;
		}

		case SE_CurrentEndurance: {
			// Handled with bonuses
			break;
		}

		case SE_BardAEDot:
		{
			effect_value = CalcSpellEffectValue(spell_id, i, caster_level);

			if (invulnerable || /*effect_value > 0 ||*/ DivineAura())
				break;

			if(effect_value < 0) {
				effect_value = -effect_value;
				if(caster){
					if(caster->IsClient() && !caster->CastToClient()->GetFeigned()){
						AddToHateList(caster, effect_value);
					}
					else if(!caster->IsClient())
						AddToHateList(caster, effect_value);
				}
				Damage(caster, effect_value, spell_id, spell.skill, false, i, true);
			} else if(effect_value > 0) {
				//healing spell...
				HealDamage(effect_value, caster);
				//healing aggro would go here; removed for now
			}
			break;
		}

		case SE_Hate2:{
			effect_value = CalcSpellEffectValue(spell_id, i, caster_level);
			if(caster){
				if(effect_value > 0){
					if(caster){
						if(caster->IsClient() && !caster->CastToClient()->GetFeigned()){
							AddToHateList(caster, effect_value);
						}
						else if(!caster->IsClient())
							AddToHateList(caster, effect_value);
					}
				}else{
					sint32 newhate = GetHateAmount(caster) + effect_value;
					if (newhate < 1) {
						SetHate(caster,1);
					} else {
						SetHate(caster,newhate);
					}
				}
			}
			break;
		}

		case SE_Charm: {
			if (!caster || !PassCharismaCheck(caster, this, spell_id)) {
				BuffFadeByEffect(SE_Charm);
			}

			break;
		}

		case SE_Root: {
			float SpellEffectiveness = ResistSpell(spells[spell_id].resisttype, spell_id, caster);
			if(SpellEffectiveness < 25) {
				BuffFadeByEffect(SE_Root);
			}

			break;
		}

		case SE_Hunger: {
			// this procedure gets called 7 times for every once that the stamina update occurs so we add 1/7 of the subtraction.  
			// It's far from perfect, but works without any unnecessary buff checks to bog down the server.
			if(IsClient()) {
				CastToClient()->m_pp.hunger_level += 5;
				CastToClient()->m_pp.thirst_level += 5;
			}
			break;
		}
		case SE_Invisibility:
		case SE_InvisVsAnimals:
		case SE_InvisVsUndead:
		{
			if(ticsremaining > 3)
			{
				if(!IsBardSong(spell_id))
				{
					double break_chance = 2.0;
					if(caster)
					{
						break_chance -= (2 * (((double)caster->GetSkill(DIVINATION) + ((double)caster->GetLevel() * 3.0)) / 650.0));
					}
					else
					{
						break_chance -= (2 * (((double)GetSkill(DIVINATION) + ((double)GetLevel() * 3.0)) / 650.0));
					}

					if(MakeRandomFloat(0.0, 100.0) < break_chance)
					{
						BuffModifyDurationBySpellID(spell_id, 3);
					}
				}
			}
		}
		case SE_Invisibility2:
		case SE_InvisVsUndead2:
		{
			if(ticsremaining <= 3 && ticsremaining > 1)
			{
				Message_StringID(MT_Spells, INVIS_BEGIN_BREAK);
			}
			break;
		}
		case SE_InterruptCasting:
		{	
			if(IsCasting())
			{
				if(MakeRandomInt(0, 100) <= spells[spell_id].base[i])
				{
					InterruptSpell();
				}
			}
			break;
		}
		// These effects always trigger when they fade.
		case SE_ImprovedSpellEffect:
		case SE_BossSpellTrigger:
		case SE_CastOnWearoff:
		{
			if (ticsremaining == 1) 
			{
				SpellOnTarget(spells[spell_id].base[i], this);
			}
			break;
		}
		case SE_LocateCorpse:
		{
			// This is handled by the client prior to SoD.

			if(IsClient() && (CastToClient()->GetClientVersionBit() & BIT_SoDAndLater))
				CastToClient()->LocateCorpse();
		}
			
		default: {
			// do we need to do anyting here?
		}
		}
	}
}

// solar: removes the buff in the buff slot 'slot'
void Mob::BuffFadeBySlot(int slot, bool iRecalcBonuses)
{
	if(slot < 0 || slot > GetMaxTotalSlots())
		return;

	if(!IsValidSpell(buffs[slot].spellid))
		return;

	if (IsClient() && !CastToClient()->IsDead())
		CastToClient()->MakeBuffFadePacket(buffs[slot].spellid, slot);

	mlog(SPELLS__BUFFS, "Fading buff %d from slot %d", buffs[slot].spellid, slot);
	
	if(spells[buffs[slot].spellid].viral_targets > 0) {
		bool last_virus = true;
		for(int i = 0; i < MAX_SPELL_TRIGGER*2; i+=2)
		{
			if(viral_spells[i] && viral_spells[i] != buffs[slot].spellid)
			{
				// If we have a virus that doesn't match this one then don't stop the viral timer
				last_virus = false;
			}
		}
		// This is the last virus on us so lets stop timer
		if(last_virus) {
			ViralTimer.Disable();
			has_virus = false;
		}
	}

	for (int i=0; i < EFFECT_COUNT; i++)
	{
		if(IsBlankSpellEffect(buffs[slot].spellid, i))
			continue;

		switch (spells[buffs[slot].spellid].effectid[i])
		{
			case SE_AddMeleeProc:
			case SE_WeaponProc:
			{
				uint16 procid = GetProcID(buffs[slot].spellid, i);
				RemoveProcFromWeapon(procid, false);
				break;
			}

			case SE_DefensiveProc:
			{
				uint16 procid = GetProcID(buffs[slot].spellid, i);
				RemoveDefensiveProc(procid);
				break;
			}

			case SE_RangedProc:
			{
				uint16 procid = GetProcID(buffs[slot].spellid, i);
				RemoveRangedProc(procid);
				break;
			}

			case SE_SummonHorse:
			{
				if(IsClient())
				{
					/*Mob* horse = entity_list.GetMob(this->CastToClient()->GetHorseId());
					if (horse) horse->Depop();
					CastToClient()->SetHasMount(false);*/
					CastToClient()->SetHorseId(0);
				}
				break;
			}

			case SE_IllusionCopy:
			case SE_Illusion:
			{
				SendIllusionPacket(0, GetBaseGender());
				if(GetRace() == OGRE){
					SendAppearancePacket(AT_Size, 9);
				}
				else if(GetRace() == TROLL){
					SendAppearancePacket(AT_Size, 8);
				}
				else if(GetRace() == VAHSHIR || GetRace() == FROGLOK || GetRace() == BARBARIAN){
					SendAppearancePacket(AT_Size, 7);
				}
				else if(GetRace() == HALF_ELF || GetRace() == WOOD_ELF || GetRace() == DARK_ELF){
					SendAppearancePacket(AT_Size, 5);
				}
				else if(GetRace() == DWARF){
					SendAppearancePacket(AT_Size, 4);
				}
				else if(GetRace() == HALFLING || GetRace() == GNOME){
					SendAppearancePacket(AT_Size, 3);
				}
				else{
					SendAppearancePacket(AT_Size, 6);
				}
				for(int x = 0; x < 7; x++){
					SendWearChange(x);
				}
				break;
			}

			case SE_Levitate:
			{
				if (!AffectedExcludingSlot(slot, SE_Levitate))
					SendAppearancePacket(AT_Levitate, 0);
				break;
			}

			case SE_Invisibility2:
			case SE_Invisibility:
			{
				SetInvisible(false);
				break;
			}

			case SE_InvisVsUndead2:
			case SE_InvisVsUndead:
			{
				invisible_undead = false;	// Mongrel: No longer IVU
				break;
			}

			case SE_InvisVsAnimals:
			{
				invisible_animals = false;
				break;
			}

			case SE_Silence:
			{
				Silence(false);
				break;
			}

			case SE_Amnesia:
			{
				Amnesia(false);
				break;
			}

			case SE_DivineAura:
			{
				SetInvul(false);
				break;
			}

			case SE_Rune:
			{
				buffs[slot].melee_rune = 0;
				break;
			}

			case SE_AbsorbMagicAtt:
			{
				buffs[slot].magic_rune = 0;
				break;
			}

			case SE_Familiar:
			{
				Mob *mypet = GetPet();
				if (mypet){
					if(mypet->IsNPC())
						mypet->CastToNPC()->Depop();
					SetPetID(0);
				}
				break;
			}

			case SE_Mez:
			{
				SendAppearancePacket(AT_Anim, ANIM_STAND);	// unfreeze
				this->mezzed = false;
				break;
			}

			case SE_Charm:
			{
				if(IsNPC())
				{
					CastToNPC()->RestoreGuardSpotCharm();
				}

				Mob* tempmob = GetOwner();
				SetOwnerID(0);
				if(tempmob)
				{
					tempmob->SetPet(0);
				}
				if (IsAIControlled())
				{
					// clear the hate list of the mobs
					entity_list.ReplaceWithTarget(this, tempmob);
					WipeHateList();
					if(tempmob)
						AddToHateList(tempmob, 1, 0);
					SendAppearancePacket(AT_Anim, ANIM_STAND);
				}
				if(tempmob && tempmob->IsClient())
				{
					EQApplicationPacket *app = new EQApplicationPacket(OP_Charm, sizeof(Charm_Struct));
					Charm_Struct *ps = (Charm_Struct*)app->pBuffer;
					ps->owner_id = tempmob->GetID();
					ps->pet_id = this->GetID();
					ps->command = 0;
					entity_list.QueueClients(this, app);
					safe_delete(app);
				}
				if(IsClient())
				{
					InterruptSpell();
					if (this->CastToClient()->IsLD())
						AI_Start(CLIENT_LD_TIMEOUT);
					else
					{
						bool feared = FindType(SE_Fear);
						if(!feared)
							AI_Stop();
					}
				}
				break;
			}

			case SE_Root:
			{
				rooted = false;
				break;
			}

			case SE_Fear:
			{
				if(RuleB(Combat, EnableFearPathing)){
					if(IsClient())
					{
						bool charmed = FindType(SE_Charm);
						if(!charmed)
							AI_Stop();
					}

					if(curfp) {
						curfp = false;
						break;
					}
				}
				else
				{
					UnStun();
				}
				break;
			}

			case SE_BindSight:
			{
				if(IsClient())
				{
					CastToClient()->SetBindSightTarget(NULL);
				}
				break;			
			}
			
			case SE_SetBodyType:
			{
				SetBodyType(GetOrigBodyType());
				break;
			}

			case SE_MovementSpeed:
			{
				if(IsClient())
				{
					Client *my_c = CastToClient();
					int32 cur_time = Timer::GetCurrentTime();
					if((cur_time - my_c->m_TimeSinceLastPositionCheck) > 1000)
					{
						float speed = (my_c->m_DistanceSinceLastPositionCheck * 100) / (float)(cur_time - my_c->m_TimeSinceLastPositionCheck);
						float runs = my_c->GetRunspeed();
						if(speed > (runs * RuleR(Zone, MQWarpDetectionDistanceFactor)))
						{
							if(!my_c->GetGMSpeed() && (runs >= my_c->GetBaseRunspeed() || (speed > (my_c->GetBaseRunspeed() * RuleR(Zone, MQWarpDetectionDistanceFactor)))))
							{
								printf("%s %i moving too fast! moved: %.2f in %ims, speed %.2f\n", __FILE__, __LINE__,
									my_c->m_DistanceSinceLastPositionCheck, (cur_time - my_c->m_TimeSinceLastPositionCheck), speed);
								if(my_c->IsShadowStepExempted())
								{
									if(my_c->m_DistanceSinceLastPositionCheck > 800)
									{
										my_c->CheatDetected(MQWarpShadowStep, my_c->GetX(), my_c->GetY(), my_c->GetZ());
									}
								}
								else if(my_c->IsKnockBackExempted())
								{
									//still potential to trigger this if you're knocked back off a 
									//HUGE fall that takes > 2.5 seconds
									if(speed > 30.0f)
									{
										my_c->CheatDetected(MQWarpKnockBack, my_c->GetX(), my_c->GetY(), my_c->GetZ());
									}
								}
								else if(!my_c->IsPortExempted())
								{
									if(!my_c->IsMQExemptedArea(zone->GetZoneID(), my_c->GetX(), my_c->GetY(), my_c->GetZ()))
									{
										if(speed > (runs * 2 * RuleR(Zone, MQWarpDetectionDistanceFactor)))
										{
											my_c->m_TimeSinceLastPositionCheck = cur_time;
											my_c->m_DistanceSinceLastPositionCheck = 0.0f;
											my_c->CheatDetected(MQWarp, my_c->GetX(), my_c->GetY(), my_c->GetZ());
											//my_c->Death(my_c, 10000000, SPELL_UNKNOWN, _1H_BLUNT);
										}
										else
										{
											my_c->CheatDetected(MQWarpLight, my_c->GetX(), my_c->GetY(), my_c->GetZ());
										}
									}
								}
							}
						}
					}
					my_c->m_TimeSinceLastPositionCheck = cur_time;
					my_c->m_DistanceSinceLastPositionCheck = 0.0f;
				}
			}
		}
	}

	// notify caster (or their master) of buff that it's worn off
	Mob *p = entity_list.GetMob(buffs[slot].casterid);
	if (p && p != this && !IsBardSong(buffs[slot].spellid))
	{
		Mob *notify = p;
		if(p->IsPet())
			notify = p->GetOwner();
		if(p) {
			notify->Message_StringID(MT_WornOff, SPELL_WORN_OFF_OF,
				spells[buffs[slot].spellid].name, GetCleanName());
		}
	}

	buffs[slot].spellid = SPELL_UNKNOWN;
	if(IsPet() && GetOwner() && GetOwner()->IsClient()) {
		SendPetBuffsToClient();
	}
	if((IsClient() && !CastToClient()->GetPVP()) || (IsPet() && GetOwner() && GetOwner()->IsClient() && !GetOwner()->CastToClient()->GetPVP()))
	{
		EQApplicationPacket *outapp = MakeBuffsPacket();

		entity_list.QueueClientsByTarget(this, outapp, true, NULL, true, false, BIT_SoDAndLater);

		safe_delete(outapp);
	}

	if(IsClient() && CastToClient()->GetClientVersionBit() & BIT_UnderfootAndLater)
	{
		EQApplicationPacket *outapp = MakeBuffsPacket(false);
		CastToClient()->FastQueuePacket(&outapp);
	}

	if (iRecalcBonuses)
		CalcBonuses();
}

sint16 Client::CalcAAFocusEffect(focusType type, int16 focus_spell, int16 spell_id) 
{
	uint32 slots = 0;
	uint32 aa_AA = 0;
	uint32 aa_value = 0;
	
	sint32 value = 0;
	// Iterate through all of the client's AAs
	for (int i = 0; i < MAX_PP_AA_ARRAY; i++) 
	{	
		aa_AA = this->aa[i]->AA;
		aa_value = this->aa[i]->value;
		if (aa_AA > 0 || aa_value > 0) 
		{	
			slots = zone->GetTotalAALevels(aa_AA);
			if (slots > 0)
			for(int j = 1;j <= slots; j++)
			{
				switch (aa_effects[aa_AA][j].skill_id)
				{	
					case SE_TriggerOnCast:
						// If focus_spell matches the spell listed in the DB, load these restrictions
						if(type == focusTriggerOnCast && focus_spell == aa_effects[aa_AA][j].base1)
							value = CalcAAFocus(type, aa_AA, spell_id);
					break;
				}
			}
		}
	}
	return value;
}


sint16 Client::CalcAAFocus(focusType type, uint32 aa_ID, int16 spell_id) 
{
	const SPDat_Spell_Struct &spell = spells[spell_id];
	
	sint16 value = 0;
	int lvlModifier = 100;
	int lvldiff = 0;

	int32 effect = 0;
	sint32 base1 = 0;
	sint32 base2 = 0;
	int32 slot = 0;

	std::map<uint32, std::map<uint32, AA_Ability> >::const_iterator find_iter = aa_effects.find(aa_ID);
	if(find_iter == aa_effects.end())
	{
		return 0;
	}

	for (map<uint32, AA_Ability>::const_iterator iter = aa_effects[aa_ID].begin(); iter != aa_effects[aa_ID].end(); ++iter) 
	{
		effect = iter->second.skill_id;
		base1 = iter->second.base1;
		base2 = iter->second.base2;
		slot = iter->second.slot;
	
		switch (effect)
		{
			case SE_Blank:
				break;
			
			// Limits
			case SE_LimitResist:
				if(base1)
				{
					if(spell.resisttype != base1)
						return 0;
				}
			break;
			case SE_LimitInstant:
				if(spell.buffduration)
					return 0;
			break;
			case SE_LimitMaxLevel:
				lvldiff = (spell.classes[(GetClass()%16) - 1]) - base1;
				//every level over cap reduces the effect by base2 percent
				if(lvldiff > 0)
				{ 
					if(base2 > 0)
					{
						lvlModifier -= base2*lvldiff;
						if(lvlModifier < 1)
							return 0;
					}
					else {	
						return 0;
					}
				}
			break;
			case SE_LimitMinLevel:
				if((spell.classes[(GetClass()%16) - 1]) < base1)
					return 0;
			break;
			case SE_LimitCastTime:
				if (spell.cast_time < base1)
					return 0;
			break;
			case SE_LimitSpell:
				// Exclude spell(any but this)
				if(base1 < 0) {	
					if (spell_id == (base1*-1))
						return 0;
				} 
				else {
				// Include Spell(only this)
					if (spell_id != base1)
						return(0);
				}
			break;
			case SE_LimitMinDur:
				if (base1 > CalcBuffDuration_formula(GetLevel(), spell.buffdurationformula, spell.buffduration))
					return(0);
			break;
			case SE_LimitEffect:
				// Exclude effect(any but this)
				if(base1 < 0) {
					if(IsEffectInSpell(spell_id,(base1*-1)))
						return 0;
				}
				else {
					// Include effect(only this)
					if(!IsEffectInSpell(spell_id,base1)) 
						return 0;
				}
			break;
			case SE_LimitSpellType:
				switch(base1)
				{
					case 0:
						if (!IsDetrimentalSpell(spell_id))
							return 0;
						break;
					case 1:
						if (!IsBeneficialSpell(spell_id))
							return 0;
						break;
				}
			break;
			
			case SE_LimitManaCost:
				if(spell.mana < base1)
					return 0;
			break;
			
			case SE_LimitTarget:
			// Exclude
			if(base1 < 0){
				if(-base1 == spell.targettype) 
					return 0;
			}
			// Include
			else {
				if(base1 != spell.targettype)
					return 0;
			}
			break;
			
			case SE_CombatSkills:
				// 1 is for disciplines only
				if(base1 == 1 && !IsDiscipline(spell_id))
					return 0;
				// 0 is spells only
				else if(base1 == 0 && IsDiscipline(spell_id))
					return 0;
			break;
			
			case SE_LimitSpellGroup:
				if(base1 > 0 && base1 != spell.spellgroup)
					return 0;
				else if(base1 < 0 && base1 == spell.spellgroup)
					return 0;
			break;
			
			// Passive Focus Effects
			case SE_ImprovedDamage:
			switch (base2)
			{
				case 0:
					if (type == focusImprovedDamage && base1 > value)
						value = base1;
				break;
				case 1:
					if (type == focusImprovedCritical && base1 > value)
						value = base1;
				break;
				case 2:
					if (type == focusImprovedUndeadDamage && base1 > value)
						value = base1;
				break;
				case 3:
					if (type == 10 && base1 > value)
						value = base1;
				break;
			}
			break;
			
			case SE_ImprovedHeal:
				if (type == focusImprovedHeal && base1 > value)
					value = base1;

			break;
			
			case SE_SwarmPetDuration:
			if (type == focusSwarmPetDuration && base1 > value)
			{
				value = base1;
			}
			break;
			
			// Unique Focus Effects
			case SE_TriggerOnCast:
				if(type == focusTriggerOnCast)
					value = 1;
			break;
	
		}
	}
	return(value*lvlModifier/100);
}

//given an item/spell's focus ID and the spell being cast, determine the focus ammount, if any
//assumes that spell_id is not a bard spell and that both ids are valid spell ids
sint16 Mob::CalcFocusEffect(focusType type, int16 focus_id, int16 spell_id) {

	const SPDat_Spell_Struct &focus_spell = spells[focus_id];
	const SPDat_Spell_Struct &spell = spells[spell_id];

	sint16 value = 0;
	int lvlModifier = 100;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		switch (focus_spell.effectid[i]) {
		case SE_Blank:
			break;

		//check limits

		case SE_LimitResist:{
			if(focus_spell.base[i]){
				if(spell.resisttype != focus_spell.base[i])
					return(0);
			}
			break;
		}
		case SE_LimitInstant:{
			if(spell.buffduration)
				return(0);
			break;
		}

		case SE_LimitMaxLevel:{
			int lvldiff = (spell.classes[(GetClass()%16) - 1]) - focus_spell.base[i];

			if(lvldiff > 0){ //every level over cap reduces the effect by spell.base2[i] percent
				if(spell.base2[i] > 0)
				{
					lvlModifier -= spell.base2[i]*lvldiff;
					if(lvlModifier < 1)
						return 0;
				}
				else
				{
					return 0;
				}
			}
			break;
		}

		case SE_LimitMinLevel:
			if (spell.classes[(GetClass()%16) - 1] < focus_spell.base[i])
				return(0);
			break;

		case SE_LimitCastTime:
			if (spells[spell_id].cast_time < (uint16)focus_spell.base[i])
				return(0);
			break;

		case SE_LimitSpell:
			if(focus_spell.base[i] < 0) {	//exclude spell
				if (spell_id == (focus_spell.base[i]*-1))
					return(0);
			} else {
				//this makes the assumption that only one spell can be explicitly included...
				if (spell_id != focus_spell.base[i])
					return(0);
			}
			break;

		case SE_LimitMinDur:
				if (focus_spell.base[i] > CalcBuffDuration_formula(GetLevel(), spell.buffdurationformula, spell.buffduration))
					return(0);
			break;

		case SE_LimitEffect:
			if(focus_spell.base[i] < 0){
				if(IsEffectInSpell(spell_id,focus_spell.base[i])){ //we limit this effect, can't have
					return 0;
				}
			}
			else{
				if(focus_spell.base[i] == SE_SummonPet) //summoning haste special case
				{	//must have one of the three pet effects to qualify
					if(!IsEffectInSpell(spell_id, SE_SummonPet) &&
						!IsEffectInSpell(spell_id, SE_NecPet) &&
						!IsEffectInSpell(spell_id, SE_SummonBSTPet))
					{
						return 0;
					}
				}
				else if(!IsEffectInSpell(spell_id,focus_spell.base[i])){ //we limit this effect, must have
					return 0;
				}
			}
			break;


		case SE_LimitSpellType:
			switch( focus_spell.base[i] )
			{
				case 0:
					if (!IsDetrimentalSpell(spell_id))
						return 0;
					break;
				case 1:
					if (!IsBeneficialSpell(spell_id))
						return 0;
					break;
				default:
					LogFile->write(EQEMuLog::Normal, "CalcFocusEffect:  unknown limit spelltype %d", focus_spell.base[i]);
			}
			break;

		case SE_LimitManaCost:
				if(spell.mana < focus_spell.base[i])
					return 0;
			break;
		
		case SE_LimitTarget:
			// Exclude
			if((focus_spell.base[i] < 0) && -focus_spell.base[i] == spell.targettype)
				return 0;
			// Include
			else if (focus_spell.base[i] > 0 && focus_spell.base[i] != spell.targettype)
				return 0;

			break;
		
		case SE_CombatSkills:
				// 1 is for disciplines only
				if(focus_spell.base[i] == 1 && !IsDiscipline(spell_id))
					return 0;
				// 0 is for spells only
				else if(focus_spell.base[i] == 0 && IsDiscipline(spell_id))
					return 0;
			break;
		
		case SE_LimitSpellGroup:
				if(focus_spell.base[i] > 0 && focus_spell.base[i] != spell.spellgroup)
					return 0;
				else if(focus_spell.base[i] < 0 && focus_spell.base[i] == spell.spellgroup)
					return 0;
			break;
	
		//handle effects

		case SE_ImprovedDamage:
			switch (focus_spell.max[i])
			{
				case 0:
					if (type == focusImprovedDamage && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 1:
					if (type == focusImprovedCritical && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 2:
					if (type == focusImprovedUndeadDamage && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 3:
					if (type == 10 && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				default: //Resist stuff
					if (type == (focusType)focus_spell.max[i] && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
			}
			break;
		case SE_ImprovedHeal:
			if (type == focusImprovedHeal && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_IncreaseSpellHaste:
			if (type == focusSpellHaste && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_IncreaseSpellDuration:
			if (type == focusSpellDuration && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_SpellDurationIncByTic:
			if (type == focusSpellDurByTic && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;	
		case SE_SwarmPetDuration:
			if (type == focusSwarmPetDuration && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;	
		case SE_IncreaseRange:
			if (type == focusRange && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_ReduceReagentCost:
			if (type == focusReagentCost && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_ReduceManaCost:
			if (type == focusManaCost)
			{
				if(focus_spell.base[i] > value)
				{
					value = focus_spell.base[i];
				}
			}
			break;
		case SE_PetPowerIncrease:
			if (type == focusPetPower && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_SpellResistReduction:
			if (type == focusResistRate && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_SpellHateMod:
			if (type == focusSpellHateMod)
			{
				if(value != 0)
				{
					if(value > 0)
					{
						if(focus_spell.base[i] > value)
						{
							value = focus_spell.base[i];
						}
					}
					else
					{
						if(focus_spell.base[i] < value)
						{
							value = focus_spell.base[i];
						}
					}
				}
				else
					value = focus_spell.base[i];
			}
			break;
			
		case SE_ReduceReuseTimer:
		{ 
			if(type == focusReduceRecastTime)
				value = focus_spell.base[i] / 1000;
				
			break;
		}

		case SE_TriggerOnCast:
		{
			if(type == focusTriggerOnCast)
				value = 1;
		
			break;
		}
		case SE_SpellVulnerability:
		{
			if(type == focusSpellVulnerability)
			{
				value = 1;
			}
			break;
		}
		case SE_BlockNextSpellFocus:
		{
			if(type == focusBlockNextSpell)
			{
				if(MakeRandomInt(1, 100) <= focus_spell.base[i]) 
					value = 1;
			}
			break;
		}
		case SE_Twincast:
		{
			if(type == focusTwincast)
			{
				value = 1;
			}
			break;
		}
		case SE_SympatheticProc:
		{
			if(type == focusSympatheticProc)
			{
				if(MakeRandomInt(0, 1000) <= focus_spell.base[i])
				{
					value = focus_id;
				}
				else
				{
					value = 0;
				}
			}
			break;
		}
		case SE_SpellDamage:
		{
			if(type == focusSpellDamage)
				value = focus_spell.base[i];

			break;
		}

#if EQDEBUG >= 6
		//this spits up a lot of garbage when calculating spell focuses
		//since they have all kinds of extra effects on them.
		default:
			LogFile->write(EQEMuLog::Normal, "CalcFocusEffect:  unknown effectid %d", focus_spell.effectid[i]);
#endif
		}
	}
	return(value*lvlModifier/100);
}

sint16 Client::GetFocusEffect(focusType type, int16 spell_id) {
	if (IsBardSong(spell_id))
		return 0;
	
	const Item_Struct* TempItem = 0;
	const Item_Struct* UsedItem = 0;
	sint16 Total = 0;
	sint16 realTotal = 0;

	//item focus
	for(int x=0; x<=21; x++)
	{
		TempItem = NULL;
		ItemInst* ins = GetInv().GetItem(x);
		if (!ins)
			continue;
		TempItem = ins->GetItem();
		if (TempItem && TempItem->Focus.Effect > 0 && TempItem->Focus.Effect != SPELL_UNKNOWN) {
			Total = CalcFocusEffect(type, TempItem->Focus.Effect, spell_id);

			if (Total > 0 && realTotal >= 0 && Total > realTotal) {
				realTotal = Total;
				UsedItem = TempItem;
			} else if (Total < 0 && Total < realTotal) {
				realTotal = Total;
				UsedItem = TempItem;
			}
		}
		for(int y = 0; y < MAX_AUGMENT_SLOTS; ++y)
		{
			ItemInst *aug = NULL;
			aug = ins->GetAugment(y);
			if(aug)
			{
				const Item_Struct* TempItemAug = aug->GetItem();
				if (TempItemAug && TempItemAug->Focus.Effect > 0 && TempItemAug->Focus.Effect != SPELL_UNKNOWN) {
					Total = CalcFocusEffect(type, TempItemAug->Focus.Effect, spell_id);
					if (Total > 0 && realTotal >= 0 && Total > realTotal) {
						realTotal = Total;
						UsedItem = TempItem;
					} else if (Total < 0 && Total < realTotal) {
						realTotal = Total;
						UsedItem = TempItem;
					}
				}
			}
		}
	}
	
	//Tribute Focus
	for(int x = TRIBUTE_SLOT_START; x < (TRIBUTE_SLOT_START + MAX_PLAYER_TRIBUTES); ++x)
	{
		TempItem = NULL;
		ItemInst* ins = GetInv().GetItem(x);
		if (!ins)
			continue;
		TempItem = ins->GetItem();
		if (TempItem && TempItem->Focus.Effect > 0 && TempItem->Focus.Effect != SPELL_UNKNOWN)
		{
			Total = CalcFocusEffect(type, TempItem->Focus.Effect, spell_id);
			if (Total > 0 && realTotal >= 0 && Total > realTotal)
			{
				realTotal = Total;
				UsedItem = TempItem;
			}
			else if (Total < 0 && Total < realTotal)
			{
				realTotal = Total;
				UsedItem = TempItem;
			}
		}
	}
	
	if (realTotal != 0 && UsedItem && spells[spell_id].buffduration == 0) {
		Message_StringID(MT_Spells, BEGINS_TO_GLOW, UsedItem->Name);
	}

	//Spell Focus
	sint16 Total2 = 0;
	sint16 realTotal2 = 0;
	
	int buff_tracker = -1;
	int buff_slot = 0;
	int16 focusspellid  = 0;
	int16 focusspell_tracker  = 0;
	uint32 buff_max = GetMaxTotalSlots();
	for (buff_slot = 0; buff_slot < buff_max; buff_slot++) {
		focusspellid = buffs[buff_slot].spellid;
		if (focusspellid == 0 || focusspellid >= SPDAT_RECORDS)
			continue;

		Total2 = CalcFocusEffect(type, focusspellid, spell_id);
		if (Total2 > 0 && realTotal2 >= 0 && Total2 > realTotal2) {
				realTotal2 = Total2;
				buff_tracker = buff_slot;
				focusspell_tracker = focusspellid;
			} else if (Total2 < 0 && Total2 < realTotal2) {
				realTotal2 = Total2;
				buff_tracker = buff_slot;
				focusspell_tracker = focusspellid;
			}
	}
	// For effects like gift of mana that only fire once, save the spellid into an array that consists of all available buff slots.
	if(buff_tracker >= 0 && buffs[buff_tracker].numhits > 0) {
		m_spellHitsLeft[buff_tracker] = focusspell_tracker;
	}
	
	// AA Focus
	sint16 Total3 = 0;
	sint16 realTotal3 = 0;
	
	uint32 slots = 0;
	uint32 aa_AA = 0;
	uint32 aa_value = 0;
	
	for (int i = 0; i < MAX_PP_AA_ARRAY; i++) 
	{
		aa_AA = this->aa[i]->AA;
		aa_value = this->aa[i]->value;
		if (aa_AA < 1 || aa_value < 1) 
			continue;
			
		Total3 = CalcAAFocus(type, aa_AA, spell_id);
		if (Total3 > 0 && realTotal3 >= 0 && Total3 > realTotal3) {
			realTotal3 = Total3;
		} 
		else if (Total3 < 0 && Total3 < realTotal3) {
			realTotal3 = Total3;
		}
	}

	if(type == focusReagentCost && IsSummonPetSpell(spell_id) && GetAA(aaElementalPact))
		return 100;

	if(type == focusReagentCost && (IsEffectInSpell(spell_id, SE_SummonItem) || IsSacrificeSpell(spell_id))){
		return 0;
	//Summon Spells that require reagents are typically imbue type spells, enchant metal, sacrifice and shouldn't be affected
	//by reagent conservation for obvious reasons.
	}
	return realTotal + realTotal2 + realTotal3;
}

bool Mob::CheckHitsRemaining(uint32 buff_slot, bool when_spell_done, bool negate)
{
	// For spells with limited number of casts
	if(when_spell_done) {
		uint32 buff_max = GetMaxTotalSlots();
		// Go through all possible saved spells with limited hits, the place in the array is the same as the buff slot
		for(int d = 0; d < buff_max; d++) {
			if(!m_spellHitsLeft[d])
				continue;
			// Double check to make sure the saved spell matches the buff in that slot
			if (m_spellHitsLeft[d] == buffs[d].spellid) {
				if(buffs[d].numhits > 1) {
					buffs[d].numhits--;
					return true;
				}
				else {
					if(!TryFadeEffect(d))
						BuffFadeBySlot(d, true);
				}
			}
		}
	}
	// This is where hit limited DS and other effects are processed
	else if(spells[buffs[buff_slot].spellid].numhits > 0 || negate)  {
		if(buffs[buff_slot].numhits > 1) {
			buffs[buff_slot].numhits--;
			return true;
		}
		else {
			if(!TryFadeEffect(buff_slot)) 
				BuffFadeBySlot(buff_slot, true);
		}
	}
	return false;
}

//for some stupid reason SK procs return theirs one base off...
uint16 Mob::GetProcID(uint16 spell_id, uint8 effect_index) {
	bool sk = false;
	bool other = false;
	for(int x = 0; x < 16; x++)
	{
		if(x == 4){
			if(spells[spell_id].classes[4] < 255)
				sk = true;
		}
		else{
			if(spells[spell_id].classes[x] < 255)
				other = true;
		}
	}
	
	if(sk && !other)
	{
		return(spells[spell_id].base[effect_index] + 1);
	}
	else{
		return(spells[spell_id].base[effect_index]);
	}
}

bool Mob::TryDeathSave() {
	bool Result = false;

	int aaClientTOTD = IsClient() ? CastToClient()->GetAA(aaTouchoftheDivine) : -1;
	int8 SuccessChance = IsClient() ? (1.2 * CastToClient()->GetAA(aaTouchoftheDivine)) : 0;
	SuccessChance += spellbonuses.DivineSaveChance + itembonuses.DivineSaveChance;
	int SaveRoll = MakeRandomInt(0, 100);

	if (SuccessChance > 0)
	{
		LogFile->write(EQEMuLog::Debug, "%s chance for a divine save was %i and the roll was %i", GetCleanName(), SuccessChance, SaveRoll);
		if (SaveRoll <= SuccessChance)
		{
			Result = true;
			/*
			int touchHealSpellID = 4544;
			switch (aaClientTOTD) {
				case 1:
					touchHealSpellID = 4544;
					break;
				case 2:
					touchHealSpellID = 4545;
					break;
				case 3:
					touchHealSpellID = 4546;
					break;
				case 4:
					touchHealSpellID = 4547;
					break;
				case 5:
					touchHealSpellID = 4548;
					break;
			} */

			// The above spell effect is not currently working. So, do a manual heal instead:

			float touchHealAmount = 0;
			switch (aaClientTOTD) {
				case 1:
					touchHealAmount = 0.15;
					break;
				case 2:
					touchHealAmount = 0.3;
					break;
				case 3:
					touchHealAmount = 0.6;
					break;
				case 4:
					touchHealAmount = 0.8;
					break;
				case 5:
					touchHealAmount = 1.0;
					break;
				default:
					touchHealAmount = 0.05; // No AA, still get small heal from Divine Save
			}

			this->Message(0, "Divine power heals your wounds.");
			SetHP(this->max_hp * touchHealAmount);

			BuffFadeByEffect(SE_DivineSave);
			// and "Touch of the Divine", an Invulnerability/HoT/Purify effect, only one for all 5 levels
			SpellOnTarget(4789, this);

			return Result;
		}
	}

	int buffSlot = GetBuffSlotFromType(SE_DeathSave);

	if(buffSlot >= 0)
	{
		SuccessChance = buffs[buffSlot].deathSaveSuccessChance;
		int8 CasterUnfailingDivinityAARank = buffs[buffSlot].casterAARank;
		int16 BuffSpellID = buffs[buffSlot].spellid;
		SaveRoll = MakeRandomInt(0, 100);

		LogFile->write(EQEMuLog::Debug, "%s chance for a death save was %i and the roll was %i", GetCleanName(), SuccessChance, SaveRoll);

		if(SuccessChance >= SaveRoll) {
			// Success
			Result = true;

			if(IsFullDeathSaveSpell(BuffSpellID)) {
				// Lazy man's full heal.
				SetHP(50000);
			}
			else {
				SetHP(300);
			}

			entity_list.MessageClose_StringID(this, false, 200, MT_CritMelee, DIVINE_INTERVENTION, GetCleanName());

			// Fade the buff
			BuffFadeBySlot(buffSlot);

			if (spellbonuses.DivineSaveChance + itembonuses.DivineSaveChance == 0) // Don't turn this off if we still have Divine Save
				SetDeathSaveChance(false);
		}
		else if (CasterUnfailingDivinityAARank >= 1) {
			// Roll the virtual dice to see if the target atleast gets a heal out of this
			SuccessChance = 30;
			SaveRoll = MakeRandomInt(0, 100);

			LogFile->write(EQEMuLog::Debug, "%s chance for a Unfailing Divinity AA proc was %i and the roll was %i", GetCleanName(), SuccessChance, SaveRoll);

			if(SuccessChance >= SaveRoll) {
				// Yep, target gets a modest heal
				SetHP(1500);
				Result = true; // This is technically a save, was giving this heal but still killing you
			}
		}
	}

	return Result;
}

void Mob::TryDotCritical(int16 spell_id, int &damage)
{
	int critChance = 0;
	critChance += itembonuses.CriticalDoTChance + spellbonuses.CriticalDoTChance + aabonuses.CriticalDoTChance;

	// since DOTs are the Necromancer forte, give an innate bonus
	// however, no chance to crit unless they've trained atleast one level in the AA first
	if (GetClass() == NECROMANCER && critChance > 0){
		critChance += 5;
	}

	if (critChance > 0){
		if (MakeRandomFloat(0, 99) < critChance)
		{
			damage *= 2;
		}
	}
}

bool Mob::AffectedExcludingSlot(int slot, int effect)
{
	for (int i = 0; i <= EFFECT_COUNT; i++)
	{
		if (i == slot)
			continue;

		if (IsEffectInSpell(buffs[i].spellid, effect))
			return true;
	}
	return false;
}

