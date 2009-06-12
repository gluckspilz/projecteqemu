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
#include "embparser.h"
#include <math.h>
#include <assert.h>
#ifndef WIN32
#include <stdlib.h>
#include "../common/unix.h"
#endif

#include "StringIDs.h"

void Mob::CalcBonuses()
{	
	CalcSpellBonuses(&spellbonuses);

#ifdef EQBOTS

	if(CastToMob()->IsBot() || (CastToMob()->GetOwner() && CastToMob()->GetOwner()->IsBot())) {
		return;
	}

#endif //EQBOTS


	CalcMaxHP();
	CalcMaxMana();
	SetAttackTimer();
	
	rooted = FindType(SE_Root);
}

void NPC::CalcBonuses()
{
	Mob::CalcBonuses();

#ifdef EQBOTS

	if(CastToMob()->IsBot() || (CastToMob()->GetOwner() && CastToMob()->GetOwner()->IsBot())) {
		return;
	}

#endif //EQBOTS

	if(RuleB(NPC, UseItemBonusesForNonPets)){
		memset(&itembonuses, 0, sizeof(StatBonuses));
		CalcItemBonuses(&itembonuses);
	}
	else{
		if(GetOwner()){
			memset(&itembonuses, 0, sizeof(StatBonuses));
			CalcItemBonuses(&itembonuses);
		}
	}
}

void Client::CalcBonuses()
{
	_ZP(Client_CalcBonuses);
	memset(&itembonuses, 0, sizeof(StatBonuses));
	CalcItemBonuses(&itembonuses);
	CalcEdibleBonuses(&itembonuses);
	
	RecalcWeight();
	
	CalcSpellBonuses(&spellbonuses);

//	CalcAABonuses(&aabonuses);	//we're not quite ready for this
	
	CalcAC();
	CalcATK();
	CalcHaste();
	
	CalcSTR();
	CalcSTA();
	CalcDEX();
	CalcAGI();
	CalcINT();
	CalcWIS();
	CalcCHA();
	
	CalcMR();
	CalcFR();
	CalcDR();
	CalcPR();
	CalcCR();
	
	CalcMaxHP();
	CalcMaxMana();
	CalcMaxEndurance();
	
	rooted = FindType(SE_Root);
}

int Client::CalcRecommendedLevelBonus(int8 level, uint8 reclevel, int basestat)
{
	if( (reclevel > 0) && (level < reclevel) )
	{
		sint32 statmod = (level * 10000 / reclevel) * basestat;
	
		if( statmod < 0 )
		{
			statmod -= 5000;
			return (statmod/10000);
		}
		else
		{
			statmod += 5000;
			return (statmod/10000);
		}
	}

	return 0;
}

void Client::CalcItemBonuses(StatBonuses* newbon) {
	//memset assumed to be done by caller.
	
	int i;
	//should not include 21 (SLOT_AMMO)
	for (i=0; i<21; i++) {
		const ItemInst* inst = m_inv[i];
		if(inst == 0)
			continue;
		AddItemBonuses(inst, newbon);
	}
	
	//Power Source Slot
	if (GetClientVersion() == EQClientSoF)
	{
		const ItemInst* inst = m_inv[9999];
		if(inst)
			AddItemBonuses(inst, newbon);
	}

	//tribute items
	for (i = 0; i < MAX_PLAYER_TRIBUTES; i++) {
		const ItemInst* inst = m_inv[TRIBUTE_SLOT_START + i];
		if(inst == 0)
			continue;
		AddItemBonuses(inst, newbon, false, true);
	}
	
	//caps
	if(newbon->ManaRegen > (RuleI(Character, ItemManaRegenCap) + GetAA(aaExpansiveMind)))
		newbon->ManaRegen = RuleI(Character, ItemManaRegenCap) + GetAA(aaExpansiveMind);
	if(newbon->HPRegen > RuleI(Character, ItemHealthRegenCap))
		newbon->HPRegen = RuleI(Character, ItemHealthRegenCap);
	
	
	SetAttackTimer();
}
		
void Client::AddItemBonuses(const ItemInst *inst, StatBonuses* newbon, bool isAug, bool isTribute) {
	if(!inst || !inst->IsType(ItemClassCommon))
	{
		return;
	}

	if(inst->GetAugmentType()==0 && isAug == true)
	{
		return;
	}

	const Item_Struct *item = inst->GetItem();

	if(!isTribute && !inst->IsEquipable(GetBaseRace(),GetClass()))
	{
		if(item->ItemType != ItemTypeFood && item->ItemType != ItemTypeDrink)
			return;
	}

	if(GetLevel() < item->ReqLevel)
	{
		return;
	}

	if(GetLevel() >= item->RecLevel)
	{
		newbon->AC += item->AC;
		newbon->HP += item->HP;
		newbon->Mana += item->Mana;
		newbon->Endurance += item->Endur;
		newbon->STR += item->AStr;
		newbon->STA += item->ASta;
		newbon->DEX += item->ADex;
		newbon->AGI += item->AAgi;
		newbon->INT += item->AInt;
		newbon->WIS += item->AWis;
		newbon->CHA += item->ACha;
		
		newbon->MR += item->MR;
		newbon->FR += item->FR;
		newbon->CR += item->CR;
		newbon->PR += item->PR;
		newbon->DR += item->DR;
	}
	else
	{
		int lvl = GetLevel();
		int reclvl = item->RecLevel;

		newbon->AC += CalcRecommendedLevelBonus( lvl, reclvl, item->AC );
		newbon->HP += CalcRecommendedLevelBonus( lvl, reclvl, item->HP );
		newbon->Mana += CalcRecommendedLevelBonus( lvl, reclvl, item->Mana );
		newbon->Endurance += CalcRecommendedLevelBonus( lvl, reclvl, item->Endur );
		newbon->STR += CalcRecommendedLevelBonus( lvl, reclvl, item->AStr );
		newbon->STA += CalcRecommendedLevelBonus( lvl, reclvl, item->ASta );
		newbon->DEX += CalcRecommendedLevelBonus( lvl, reclvl, item->ADex );
		newbon->AGI += CalcRecommendedLevelBonus( lvl, reclvl, item->AAgi );
		newbon->INT += CalcRecommendedLevelBonus( lvl, reclvl, item->AInt );
		newbon->WIS += CalcRecommendedLevelBonus( lvl, reclvl, item->AWis );
		newbon->CHA += CalcRecommendedLevelBonus( lvl, reclvl, item->ACha );

		newbon->MR += CalcRecommendedLevelBonus( lvl, reclvl, item->MR );
		newbon->FR += CalcRecommendedLevelBonus( lvl, reclvl, item->FR );
		newbon->CR += CalcRecommendedLevelBonus( lvl, reclvl, item->CR );
		newbon->PR += CalcRecommendedLevelBonus( lvl, reclvl, item->PR );
		newbon->DR += CalcRecommendedLevelBonus( lvl, reclvl, item->DR );
	}
	
	//FatherNitwit: New style haste, shields, and regens
	if(newbon->haste < (sint8)item->Haste) {
		newbon->haste = item->Haste;
	}
	if(item->Regen > 0) {
		newbon->HPRegen += item->Regen;
	}
	if(item->ManaRegen > 0) {
		newbon->ManaRegen += item->ManaRegen;
	}
	if(item->Attack > 0) {
		newbon->ATK += item->Attack;
	}
	if(item->EnduranceRegen > 0){
		newbon->EnduranceRegen += item->EnduranceRegen;
	}
	if(item->DamageShield > 0) {
		if((newbon->DamageShield + item->DamageShield) > RuleI(Character, ItemDamageShieldCap))
			newbon->DamageShield = RuleI(Character, ItemDamageShieldCap);
		else
			newbon->DamageShield += item->DamageShield;
	}
	if(item->SpellShield > 0) {
		if((newbon->SpellDamageShield + item->SpellShield) > RuleI(Character, ItemSpellShieldingCap))
			newbon->SpellDamageShield = RuleI(Character, ItemSpellShieldingCap);
		else
			newbon->SpellDamageShield += item->SpellShield;
	}
	if(item->Shielding > 0) {
		if((newbon->MeleeMitigation + item->Shielding) > RuleI(Character, ItemShieldingCap))
			newbon->MeleeMitigation = RuleI(Character, ItemShieldingCap);
		else
			newbon->MeleeMitigation += item->Shielding;
	}
	if(item->StunResist > 0) {
		if((newbon->StunResist + item->StunResist) > RuleI(Character, ItemStunResistCap))
			newbon->StunResist = RuleI(Character, ItemStunResistCap);
		else
			newbon->StunResist += item->StunResist;
	}
	if(item->StrikeThrough > 0) {
		if((newbon->StrikeThrough + item->StrikeThrough) > RuleI(Character, ItemStrikethroughCap))
			newbon->StrikeThrough = RuleI(Character, ItemStrikethroughCap);
		else
			newbon->StrikeThrough += item->StrikeThrough;
	}
	if(item->Avoidance > 0) {
		if((newbon->AvoidMeleeChance + item->Avoidance) > RuleI(Character, ItemAvoidanceCap))
			newbon->AvoidMeleeChance = RuleI(Character, ItemAvoidanceCap);
		else
			newbon->AvoidMeleeChance += item->Avoidance;
	}
	if(item->Accuracy > 0) {
		if((newbon->HitChance + item->Accuracy) > RuleI(Character, ItemAccuracyCap))
			newbon->HitChance = RuleI(Character, ItemAccuracyCap);
		else
			newbon->HitChance += item->Accuracy;
	}
	if(item->CombatEffects > 0) {
		if((newbon->ProcChance + item->CombatEffects) > RuleI(Character, ItemCombatEffectsCap))
			newbon->ProcChance = RuleI(Character, ItemCombatEffectsCap);
		else
			newbon->ProcChance += item->CombatEffects;
	}
	if(item->DotShielding > 0) {
		if((newbon->DoTShielding + item->DotShielding) > RuleI(Character, ItemDoTShieldingCap))
			newbon->DoTShielding = RuleI(Character, ItemDoTShieldingCap);
		else
			newbon->DoTShielding += item->DotShielding;
	}
	if (item->Worn.Effect>0 && (item->Worn.Type == ET_WornEffect)) { // latent effects
		ApplySpellsBonuses(item->Worn.Effect, item->Worn.Level, newbon);
	}
	switch(item->BardType)
	{
	case 51: /* All (e.g. Singing Short Sword) */
		{
			if(item->BardValue > newbon->singingMod)
				newbon->singingMod = item->BardValue;
			if(item->BardValue > newbon->brassMod)
				newbon->brassMod = item->BardValue;
			if(item->BardValue > newbon->stringedMod)
				newbon->stringedMod = item->BardValue;
			if(item->BardValue > newbon->percussionMod)
				newbon->percussionMod = item->BardValue;
			if(item->BardValue > newbon->windMod)
				newbon->windMod = item->BardValue;
			break;
		}
	case 50: /* Singing */
		{
			if(item->BardValue > newbon->singingMod)
				newbon->singingMod = item->BardValue;
			break;
		}
	case 23: /* Wind */
		{
			if(item->BardValue > newbon->windMod)
				newbon->windMod = item->BardValue;
			break;
		}
	case 24: /* stringed */
		{
			if(item->BardValue > newbon->stringedMod)
				newbon->stringedMod = item->BardValue;
			break;
		}
	case 25: /* brass */
		{
			if(item->BardValue > newbon->brassMod)
				newbon->brassMod = item->BardValue;
			break;
		}
	case 26: /* Percussion */
		{
			if(item->BardValue > newbon->percussionMod)
				newbon->percussionMod = item->BardValue;
			break;
		}
	}
	
	if (item->SkillModValue != 0 && item->SkillModType < HIGHEST_SKILL){
		if (newbon->skillmod[item->SkillModType] < item->SkillModValue)
			newbon->skillmod[item->SkillModType] = (sint8)item->SkillModValue;
	}

	int i;
	for(i = 0; i < MAX_AUGMENT_SLOTS; i++) {
		AddItemBonuses(inst->GetAugment(i),newbon,true);
	}
}

void Client::CalcEdibleBonuses(StatBonuses* newbon) {
#if EQDEBUG >= 11
    cout<<"Client::CalcEdibleBonuses(StatBonuses* newbon)"<<endl;
#endif
  // Search player slots for skill=14(food) and skill=15(drink)
  	uint32 i;
  	
	bool food = false;
	bool drink = false;
	for (i = 22; i <= 29; i++)
	{
		if (food && drink)
			break;
		const ItemInst* inst = GetInv().GetItem(i);
		if (inst && inst->GetItem() && inst->IsType(ItemClassCommon)) {
			const Item_Struct *item=inst->GetItem();
			if (item->ItemType == ItemTypeFood && !food)
				food = true;
			else if (item->ItemType == ItemTypeDrink && !drink)
				drink = true;
			else
				continue;
			AddItemBonuses(inst, newbon);
		}
	}
	for (i = 251; i <= 330; i++)
	{
		if (food && drink)
			break;
		const ItemInst* inst = GetInv().GetItem(i);
		if (inst && inst->GetItem() && inst->IsType(ItemClassCommon)) {
			const Item_Struct *item=inst->GetItem();
			if (item->ItemType == ItemTypeFood && !food)
				food = true;
			else if (item->ItemType == ItemTypeDrink && !drink)
				drink = true;
			else
				continue;
			AddItemBonuses(inst, newbon);
		}
	}
}

void Client::CalcAABonuses(StatBonuses* newbon) {

	memset(newbon, 0, sizeof(StatBonuses));	//start fresh

	int i;
	SendAA_Struct* aa_struct = NULL;	//need a place to put the AA info
	int32 slots = 0;	//
	for (i = 0; i < aaHighestID; i++) {	//iterate through all of the client's AAs
		if (aa[i]->AA > 0) {	//do we have the AA?
			slots = database.GetTotalAALevels(i);	//find out how many effects from aa_effects table
			if (slots > 0) { //and does it have any effects? may be able to put this above, not sure if it runs on each iteration
													//todo: load GetTotalAALevels into memory, otherwise we run this query 1600+ times every time we calculate stats, not including queries from FillAAEffects
				aa_struct->id = aa[i]->AA;	//since FillAAEffects pulls from aa_struct
				aa_struct->current_level = aa[i]->value;	//not sure if we'll actually need this at any point
				database.FillAAEffects(aa_struct);	//pull info about the AA so we can work with it, since we don't know anything about it
				ApplyAABonuses(aa_struct, slots, newbon);	//add the bonuses
				memset(aa_struct, 0, sizeof(SendAA_Struct));	//need to clear out the old info for the next loop. is there a better way to do this since we really just need to clear abilities?
			}
		}
	}
}


//A lot of the normal spell functions are set for just spells. For now, we'll just put them directly into the code and comment with the corresponding normal function
//Maybe we'll fix it later? :-D
void Client::ApplyAABonuses(SendAA_Struct* aa_struct, int32 slots, StatBonuses* newbon) {

	if (!(slots > 0))	//sanity check. why bother if no slots to fill?
		return;

	//really don't want to have to type all the junk. from AA_Ability struct
	int32 effect = 0;
	int32 base1 = 0;
	int32 base2 = 0;	//only really used for SE_RaiseStatCap & SE_ReduceSkillTimer in aa_effects table
	int8 i; //slot
	for (i = 1; i <= slots; i++) {	//i guess this means we can ignore a slot if it is 0
		effect = aa_struct->abilities[i].skill_id;
		base1 = aa_struct->abilities[i].base1;
		base2 = aa_struct->abilities[i].base2;

		//IsBlankSpellEffect
		if (effect == SE_Blank || (effect == SE_CHA && base1 == 0) || effect == SE_StackingCommand_Block || effect == SE_StackingCommand_Overwrite)
			continue;

		switch (effect)
		{
			case SE_Accuracy:
				newbon->HitChance += base1;
				break;
			case SE_CurrentHP: //regens
				newbon->HPRegen += base1;
				break;
			case SE_MovementSpeed:
				newbon->movementspeed += base1;	//should we let these stack?
				/*if (base1 > newbon->movementspeed)	//or should we use a total value?
					newbon->movementspeed = base1;*/
				break;
			case SE_STR:
				newbon->STR += base1;
				break;
			case SE_DEX:
				newbon->DEX += base1;
				break;
			case SE_AGI:
				newbon->AGI += base1;
				break;
			case SE_STA:
				newbon->STA += base1;
				break;
			case SE_INT:
				newbon->INT += base1;
				break;
			case SE_WIS:
				newbon->WIS += base1;
				break;
			case SE_CHA:
				newbon->CHA += base1;
				break;
			case SE_WaterBreathing:
				//handled by client
				break;
			case SE_CurrentMana:
				break;
			case SE_ResistFire:
				newbon->FR += base1;
				break;
			case SE_ResistCold:
				newbon->CR += base1;
				break;
			case SE_ResistPoison:
				newbon->PR += base1;
				break;
			case SE_ResistDisease:
				newbon->DR += base1;
				break;
			case SE_ResistMagic:
				newbon->MR += base1;
				break;
			case SE_IncreaseSpellHaste:
				break;
			case SE_IncreaseRange:
				break;
			case SE_LimitEffect:
				break;
			case SE_LimitSpellType:
				break;
			case SE_LimitMinDur:
				break;
			case SE_LimitInstant:
				break;
			case SE_LimitCastTime:
				break;
			case SE_MaxHPChange:
				break;
			case SE_Packrat:
				break;
			case SE_TwoHandBash:
				break;
			case SE_ReduceSkillTimer:
				break;
			case SE_SetBreathLevel:
				break;
			case SE_RaiseStatCap:
				switch(base2)
				{
					//are these #define'd somewhere?
					case 0: //str
						newbon->STRCapMod += base1;
						break;
					case 1: //sta
						newbon->STACapMod += base1;
						break;
					case 2: //agi
						newbon->AGICapMod += base1;
						break;
					case 3: //dex
						newbon->DEXCapMod += base1;
						break;
					case 4: //wis
						newbon->WISCapMod += base1;
						break;
					case 5: //int
						newbon->INTCapMod += base1;
						break;
					case 6: //cha
						newbon->CHACapMod += base1;
						break;
					case 7: //mr
						newbon->MRCapMod += base1;
						break;
					case 8: //cr
						newbon->CRCapMod += base1;
						break;
					case 9: //fr
						newbon->FRCapMod += base1;
						break;
					case 10: //pr
						newbon->PRCapMod += base1;
						break;
					case 11: //dr
						newbon->DRCapMod += base1;
						break;
				}
				break;
			case SE_PetDiscipline2:
				break;
			case SE_BaseMovementSpeed:
				break;
			case SE_SpellSlotIncrease:
				break;
			case SE_MysticalAttune:
				break;
		}

	}

}

void Mob::CalcSpellBonuses(StatBonuses* newbon)
{
	int i;

	memset(newbon, 0, sizeof(StatBonuses));
	newbon->AggroRange = -1;
	newbon->AssistRange = -1;

	for(i = 0; i < BUFF_COUNT; i++) {
		if(buffs[i].spellid != SPELL_UNKNOWN)
			ApplySpellsBonuses(buffs[i].spellid, buffs[i].casterlevel, newbon, buffs[i].casterid);
	}
	
	//this prolly suffer from roundoff error slightly...
	newbon->AC = newbon->AC * 10 / 34;	//ratio determined impirically from client.
}

void Mob::ApplySpellsBonuses(int16 spell_id, int8 casterlevel, StatBonuses* newbon, int16 casterId)
{
	int i, effect_value;
	Mob *caster = NULL;

	if(!IsValidSpell(spell_id))
		return;

#ifdef EQBOTS

	if(IsBot()) {
		caster = entity_list.GetMob(casterId);
	}
	else

#endif //EQBOTS
	
	if(casterId > 0)
		caster = entity_list.GetClientByID(casterId);
	
	for (i = 0; i < EFFECT_COUNT; i++)
	{
		if(IsBlankSpellEffect(spell_id, i))
			continue;

		effect_value = CalcSpellEffectValue(spell_id, i, casterlevel, caster);

		switch (spells[spell_id].effectid[i])
		{

			case SE_Harmony:
			{
				// neotokyo: Harmony effect as buff - kinda tricky
				// harmony could stack with a lull spell, which has better aggro range
				// take the one with less range in any case
				if
				(
					newbon->AggroRange == -1 ||
					effect_value < newbon->AggroRange
				)
				{
					newbon->AggroRange = effect_value;
				}
				break;
			}

			case SE_ChangeFrenzyRad:
			{
				if
				(
					newbon->AssistRange == -1 ||
					effect_value < newbon->AssistRange
				)
				{
					newbon->AssistRange = effect_value;
				}
				break;
			}

			case SE_AttackSpeed:
			{
				if ((effect_value - 100) > 0) { // Haste
					if (newbon->haste < 0) break; // Slowed - Don't apply haste
					if ((effect_value - 100) > newbon->haste) {
						newbon->haste = effect_value - 100;
					}
				} else if ((effect_value - 100) < 0) { // Slow
					if ((effect_value - 100) < newbon->haste) {
						newbon->haste = effect_value - 100;
					}
				}
				break;
			}

 			case SE_AttackSpeed2:
			{
				if ((effect_value - 100) > 0) { // Haste V2 - Stacks with V1 but does not Overcap
					if ((effect_value - 100) > newbon->hastetype2) {
						newbon->hastetype2 = effect_value - 100;
					}
				}
				break;
 			}
 
 			case SE_AttackSpeed3:
 			{
				if (effect_value > 0) { // Haste V3 - Stacks and Overcaps
					if (effect_value > newbon->hastetype3) {
						newbon->hastetype3 = effect_value;
					}
				}
				break;
 			}

			case SE_TotalHP:
			{
				newbon->HP += effect_value;
				break;
			}

			case SE_ManaPool:
			{
				newbon->Mana += effect_value;
				break;
			}

			case SE_Stamina:
			{
				newbon->EnduranceReduction += effect_value;
				break;
			}
			
			case SE_ArmorClass:
			{
				newbon->AC += effect_value;
				break;
			}

			case SE_ATK:
			{
				newbon->ATK += effect_value;
				break;
			}

			case SE_STR:
			{
				newbon->STR += effect_value;
				break;
			}

			case SE_DEX:
			{
				newbon->DEX += effect_value;
				break;
			}

			case SE_AGI:
			{
				newbon->AGI += effect_value;
				break;
			}

			case SE_STA:
			{
				newbon->STA += effect_value;
				break;
			}

			case SE_INT:
			{
				newbon->INT += effect_value;
				break;
			}

			case SE_WIS:
			{
				newbon->WIS += effect_value;
				break;
			}

			case SE_CHA:
			{
				if (spells[spell_id].base[i] != 0) {
					newbon->CHA += effect_value;
				}
				break;
			}

			case SE_AllStats:
			{
				newbon->STR += effect_value;
				newbon->DEX += effect_value;
				newbon->AGI += effect_value;
				newbon->STA += effect_value;
				newbon->INT += effect_value;
				newbon->WIS += effect_value;
				newbon->CHA += effect_value;
				break;
			}

			case SE_ResistFire:
			{
				newbon->FR += effect_value;
				break;
			}

			case SE_ResistCold:
			{
				newbon->CR += effect_value;
				break;
			}

			case SE_ResistPoison:
			{
				newbon->PR += effect_value;
				break;
			}

			case SE_ResistDisease:
			{
				newbon->DR += effect_value;
				break;
			}

			case SE_ResistMagic:
			{
				newbon->MR += effect_value;
				break;
			}

			case SE_ResistAll:
			{
				newbon->MR += effect_value;
				newbon->DR += effect_value;
				newbon->PR += effect_value;
				newbon->CR += effect_value;
				newbon->FR += effect_value;
				break;
			}

			case SE_RaiseStatCap:
			{
				switch(spells[spell_id].base2[i])
				{
					//are these #define'd somewhere?
					case 0: //str
						newbon->STRCapMod += effect_value;
						break;
					case 1: //sta
						newbon->STACapMod += effect_value;
						break;
					case 2: //agi
						newbon->AGICapMod += effect_value;
						break;
					case 3: //dex
						newbon->DEXCapMod += effect_value;
						break;
					case 4: //wis
						newbon->WISCapMod += effect_value;
						break;
					case 5: //int
						newbon->INTCapMod += effect_value;
						break;
					case 6: //cha
						newbon->CHACapMod += effect_value;
						break;
					case 7: //mr
						newbon->MRCapMod += effect_value;
						break;
					case 8: //cr
						newbon->CRCapMod += effect_value;
						break;
					case 9: //fr
						newbon->FRCapMod += effect_value;
						break;
					case 10: //pr
						newbon->PRCapMod += effect_value;
						break;
					case 11: //dr
						newbon->DRCapMod += effect_value;
						break;
				}
				break;
			}

			case SE_CastingLevel:	// Brilliance of Ro
			{
				newbon->effective_casting_level += effect_value;
				break;
			}

			case SE_MovementSpeed:
			{
				newbon->movementspeed += effect_value;
				break;
			}

			case SE_DamageShield:
			{
				newbon->DamageShield += effect_value;
				newbon->DamageShieldSpellID = spell_id;
				newbon->DamageShieldType = GetDamageShieldType(spell_id);
				break;
			}
			
			case SE_SpellDamageShield:
			{
				newbon->SpellDamageShield += effect_value;
				break;
			}

			case SE_ReverseDS:
			{
				newbon->ReverseDamageShield += effect_value;
				newbon->ReverseDamageShieldSpellID = spell_id;
				newbon->ReverseDamageShieldType = GetDamageShieldType(spell_id);
				break;
			}

			case SE_Reflect:
			{
				newbon->reflect_chance += effect_value;
				break;
			}

			case SE_SingingSkill:
			{
				//newbon->skillmod[SINGING] += effect_value;
				if(effect_value > newbon->singingMod)
					newbon->singingMod = effect_value;
				break;
			}
			
			case SE_ChangeAggro:
			{
				newbon->hatemod += effect_value;
				break;
			}
			case SE_MeleeMitigation:
			{
				//for some reason... this value is negative for increased mitigation
				newbon->MeleeMitigation -= effect_value;
				break;
			}
			
			/*
				Assuming that none of these chances stack... they just pick the highest
				
				perhaps some smarter logic is needed here to handle the case
				where there is a chance increase and a decrease
				because right now, the increase will completely offset the decrease...
				
			*/
			case SE_CriticalHitChance:
			{
				if(newbon->CriticalHitChance < effect_value)
					newbon->CriticalHitChance = effect_value;
				break;
			}
				
			case SE_CrippBlowChance:
			{
				if(newbon->CrippBlowChance < effect_value)
					newbon->CrippBlowChance = effect_value;
				break;
			}
				
			case SE_AvoidMeleeChance:
			{
				//multiplier is to be compatible with item effects
				//watching for overflow too
				effect_value = effect_value<3000? effect_value * 10 : 30000;
				if(newbon->AvoidMeleeChance < effect_value)
					newbon->AvoidMeleeChance = effect_value;
				break;
			}
				
			case SE_RiposteChance:
			{
				if(newbon->RiposteChance < effect_value)
					newbon->RiposteChance = effect_value;
				break;
			}
				
			case SE_DodgeChance:
			{
				if(newbon->DodgeChance < effect_value)
					newbon->DodgeChance = effect_value;
				break;
			}
				
			case SE_ParryChance:
			{
				if(newbon->ParryChance < effect_value)
					newbon->ParryChance = effect_value;
				break;
			}
				
			case SE_DualWeildChance:
			{
				if(newbon->DualWeildChance < effect_value)
					newbon->DualWeildChance = effect_value;
				break;
			}
				
			case SE_DoubleAttackChance:
			{
				if(newbon->DoubleAttackChance < effect_value)
					newbon->DoubleAttackChance = effect_value;
				break;
			}
				
			case SE_MeleeLifetap:
			{
				newbon->MeleeLifetap = true;
				break;
			}
				
			case SE_AllInstrunmentMod:
			{
				if(effect_value > newbon->singingMod)
					newbon->singingMod = effect_value;
				if(effect_value > newbon->brassMod)
					newbon->brassMod = effect_value;
				if(effect_value > newbon->percussionMod)
					newbon->percussionMod = effect_value;
				if(effect_value > newbon->windMod)
					newbon->windMod = effect_value;
				if(effect_value > newbon->stringedMod)
					newbon->stringedMod = effect_value;
				break;
			}
				
			case SE_ResistSpellChance:
			{
				if(newbon->ResistSpellChance < effect_value)
					newbon->ResistSpellChance = effect_value;
				break;
			}
				
			case SE_ResistFearChance:
			{
				if(newbon->ResistFearChance < effect_value)
					newbon->ResistFearChance = effect_value;
				break;
			}
				
 			case SE_HundredHands:
 			{
				if(IsBeneficialSpell(spell_id)){ //If it's a beneficial spell we switch it cause
					effect_value *= -1; //of the way it's stored by sony, negative for both ben and det spells
				}
				effect_value = effect_value > 120 ? 120 : (effect_value < -120 ? -120 : effect_value);
				newbon->HundredHands = newbon->HundredHands > effect_value ? newbon->HundredHands : effect_value;
 				break;
 			}
				
			case SE_MeleeSkillCheck:
			{
				if(newbon->MeleeSkillCheck < effect_value) {
					newbon->MeleeSkillCheck = effect_value;
					newbon->MeleeSkillCheckSkill = spells[spell_id].base2[i]==-1?255:spells[spell_id].base2[i];
				}
				break;
			}
				
			case SE_HitChance:
			{
				//multiplier is to be compatible with item effects
				//watching for overflow too
				effect_value = effect_value<2000? effect_value * 15 : 30000;
				if(newbon->HitChance < effect_value) {
					newbon->HitChance = effect_value;
					newbon->HitChanceSkill = spells[spell_id].base2[i]==-1?255:spells[spell_id].base2[i];
				}
				break;
			}
				
			case SE_DamageModifier:
			{
				if(newbon->DamageModifier < effect_value) {
					newbon->DamageModifier = effect_value;
					newbon->DamageModifierSkill = spells[spell_id].base2[i]==-1?255:spells[spell_id].base2[i];
				}
				break;
			}
				
			case SE_MinDamageModifier:
			{
				if(newbon->MinDamageModifier < effect_value)
					newbon->MinDamageModifier = effect_value;
				break;
			}
				
			case SE_StunResist:
			{
				if(newbon->StunResist < effect_value)
					newbon->StunResist = effect_value;
				break;
			}
				
			case SE_ProcChance:
			{
				//multiplier is to be compatible with item effects
				//watching for overflow too
				effect_value = effect_value<3000? effect_value : 3000;
				if(newbon->ProcChance < effect_value)
					newbon->ProcChance = effect_value;
				break;
			}
				
			case SE_ExtraAttackChance:
			{
				if(newbon->ExtraAttackChance < effect_value)
					newbon->ExtraAttackChance = effect_value;
				break;
			}
				
		}
	}
}

void NPC::CalcItemBonuses(StatBonuses *newbon)
{
	if(newbon){

		for(int i = 0; i < 8; i++){
			const Item_Struct *cur = database.GetItem(equipment[i]);
			if(cur){
				//basic stats
				newbon->AC += cur->AC;
				newbon->HP += cur->HP;
				newbon->Mana += cur->Mana;
				newbon->Endurance += cur->Endur;
				newbon->STR += cur->AStr;
				newbon->STA += cur->ASta;
				newbon->DEX += cur->ADex;
				newbon->AGI += cur->AAgi;
				newbon->INT += cur->AInt;
				newbon->WIS += cur->AWis;
				newbon->CHA += cur->ACha;
				newbon->MR += cur->MR;
				newbon->FR += cur->FR;
				newbon->CR += cur->CR;
				newbon->PR += cur->PR;
				newbon->DR += cur->DR;
				

				//more complex stats
				if(cur->Regen > 0) {
					newbon->HPRegen += cur->Regen;
				}
				if(cur->ManaRegen > 0) {
					newbon->ManaRegen += cur->ManaRegen;
				}
				if(cur->Attack > 0) {
					newbon->ATK += cur->Attack;
				}
				if(cur->DamageShield > 0) {
					newbon->DamageShield += cur->DamageShield;
				}
				if(cur->SpellShield > 0) {
					newbon->SpellDamageShield += cur->SpellShield;
				}
				if(cur->Shielding > 0) {
					newbon->MeleeMitigation += cur->Shielding;
				}
				if(cur->StunResist > 0) {
					newbon->StunResist += cur->StunResist;
				}
				if(cur->StrikeThrough > 0) {
					newbon->StrikeThrough += cur->StrikeThrough;
				}
				if(cur->Avoidance > 0) {
					newbon->AvoidMeleeChance += cur->Avoidance;
				}
				if(cur->Accuracy > 0) {
					newbon->HitChance += cur->Accuracy;
				}
				if(cur->CombatEffects > 0) {
					newbon->ProcChance += cur->CombatEffects;
				}
				if (cur->Worn.Effect>0 && (cur->Worn.Type == ET_WornEffect)) { // latent effects
					ApplySpellsBonuses(cur->Worn.Effect, cur->Worn.Level, newbon);
				}
			}
		}
	
	}
}

void Client::CalcItemScale()
{
	bool changed = false;

	if(CalcItemScale(0, 21))
		changed = true;

	if(CalcItemScale(22, 30))
		changed = true;

	if(CalcItemScale(251, 341))
		changed = true;

	if(CalcItemScale(400, 405))
		changed = true;

	if(changed)
	{
		CalcBonuses();
	}
}

bool Client::CalcItemScale(int32 slot_x, int32 slot_y)
{
	bool changed = false;
	int i;
	for (i = slot_x; i < slot_y; i++) {
		const ItemInst* inst = m_inv[i];
		if(inst == 0)
			continue;
		
		bool update_slot = false;
		if(inst->IsScaling())
		{
			EvoItemInst* e_inst = (EvoItemInst*)inst;
			uint16 oldexp = e_inst->GetExp();
#ifdef EMBPERL
			((PerlembParser *)parse)->Event(EVENT_SCALE_CALC, e_inst->GetID(), "", e_inst, this);
#endif
			if (e_inst->GetExp() != oldexp) {	// if the scaling factor changed, rescale the item and update the client
				e_inst->ScaleItem();
				changed = true;
				update_slot = true;
			}
		}

		//iterate all augments
		for(int x = 0; x < MAX_AUGMENT_SLOTS; ++x) 
		{
			ItemInst * a_inst = inst->GetAugment(x);
			if(!a_inst)
				continue;

			if(a_inst->IsScaling())
			{
				EvoItemInst* e_inst = (EvoItemInst*)a_inst;
				uint16 oldexp = e_inst->GetExp();
#ifdef EMBPERL
				((PerlembParser *)parse)->Event(EVENT_SCALE_CALC, e_inst->GetID(), "", e_inst, this);
#endif
				if (e_inst->GetExp() != oldexp) 
				{
					e_inst->ScaleItem();
					changed = true;
					update_slot = true;
				}
			}
		}

		if(update_slot)
		{
			SendItemPacket(i, inst, ItemPacketCharmUpdate);
		}
	}
	return changed;
}
