/*  EQEMu:  Everquest Server Emulator
Copyright (C) 2001-2002  EQEMu Development Team (http://eqemu.org)

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
#include "features.h"
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include "masterentity.h"
#include "pathing.h"
#include "zone.h"
#include "spdat.h"
#include "../common/skills.h"
#include "map.h"
#include "StringIDs.h"
#include "../common/rulesys.h"

#include "../common/emu_opcodes.h"
#include "../common/eq_packet_structs.h"
#include "zonedb.h"
#include "../common/packet_dump.h"
#include "../common/packet_functions.h"
#include "../common/bodytypes.h"
#include "../common/guilds.h"
#include "../common/MiscFunctions.h"
#include <stdio.h>
extern EntityList entity_list;
#if !defined(NEW_LoadSPDat) && !defined(DB_LoadSPDat)
	extern SPDat_Spell_Struct spells[SPDAT_RECORDS];
#endif
extern bool spells_loaded;

extern Zone* zone;

Mob::Mob(const char*   in_name,
         const char*   in_lastname,
         sint32  in_cur_hp,
         sint32  in_max_hp,
         int8    in_gender,
         uint16    in_race,
         int8    in_class,
         bodyType in_bodytype,
         int8    in_deity,
         int8    in_level,
         int32	 in_npctype_id,
		 float	in_size,
		 float	in_runspeed,
         float	in_heading,
         float	in_x_pos,
         float	in_y_pos,
         float	in_z_pos,

         int8    in_light,
		 int8	 in_texture,
		 int8	 in_helmtexture,
		 int16	 in_ac,
		 int16	 in_atk,
		 int16	 in_str,
		 int16	 in_sta,
		 int16	 in_dex,
		 int16	 in_agi,
		 int16	 in_int,
		 int16	 in_wis,
		 int16	 in_cha,
		 int8	in_haircolor,
		 int8	in_beardcolor,
		 int8	in_eyecolor1, // the eyecolors always seem to be the same, maybe left and right eye?
		 int8	in_eyecolor2,
		 int8	in_hairstyle,
		 int8	in_luclinface,
		 int8	in_beard,
		 int32	in_drakkin_heritage,
		 int32	in_drakkin_tattoo,
		 int32	in_drakkin_details,
		 int32	in_armor_tint[MAX_MATERIALS],

		 int8	in_aa_title,
		 int8	in_see_invis,			// see through invis/ivu
		 int8   in_see_invis_undead,
		 int8   in_see_hide,
		 int8   in_see_improved_hide,
		 sint16 in_hp_regen,
		 sint16 in_mana_regen,
		 int8	in_qglobal

		 ) : 
		attack_timer(2000),
		attack_dw_timer(2000),
		ranged_timer(2000),
		tic_timer(6000),
		cheat_timer(0),
		threshold_timer(0),
		mana_timer(2000),
		spellend_timer(0),
		rewind_timer(30000), //Timer used for determining amount of time between actual player position updates for /rewind.
		stunned_timer(0),
		bardsong_timer(6000),
		flee_timer(FLEE_CHECK_TIMER),
		bindwound_timer(10000)
	//	mezzed_timer(0)
{
	targeted = 0;
	logpos = false;
	tar_ndx=0;
	tar_vector=0;
	tar_vx=0;
	tar_vy=0;
	tar_vz=0;
	tarx=0;
	tary=0;
	tarz=0;
	fear_walkto_x = -999999;
	fear_walkto_y = -999999;
	fear_walkto_z = -999999;
	curfp = false;

	AI_Init();
	SetMoving(false);
	moved=false;
 	rewind_x = 0;		//Stored x_pos for /rewind
 	rewind_y = 0;		//Stored y_pos for /rewind
 	rewind_z = 0;		//Stored z_pos for /rewind

	warp_threshold = 140;
	last_warp_distance = 0;	
	
	_egnode = NULL;
	adverrorinfo = 0;
	name[0]=0;
	clean_name[0]=0;
	lastname[0]=0;
	if(in_name)
		strncpy(name,in_name,64);
	if(in_lastname)
		strncpy(lastname,in_lastname,64);
	cur_hp		= in_cur_hp;
	max_hp		= in_max_hp;
	base_hp		= in_max_hp;
	gender		= in_gender;
	race		= in_race;
	base_gender	= in_gender;
	base_race	= in_race;
	class_		= in_class;
    bodytype    = in_bodytype;
	deity		= in_deity;
	level		= in_level;
	npctype_id	= in_npctype_id; // rembrant, Dec. 20, 2001
	size		= in_size;
	runspeed   = in_runspeed;

	
    // neotokyo: sanity check
    if (runspeed < 0 || runspeed > 20)
        runspeed = 1.25f;
	
	heading		= in_heading;
	x_pos		= in_x_pos;
	y_pos		= in_y_pos;
	z_pos		= in_z_pos;
	light		= in_light;
	texture		= in_texture;
	helmtexture	= in_helmtexture;
	haircolor	= in_haircolor;
	beardcolor	= in_beardcolor;
	eyecolor1	= in_eyecolor1;
	eyecolor2	= in_eyecolor2;
	hairstyle	= in_hairstyle;
	luclinface	= in_luclinface;
	beard		= in_beard;
	drakkin_heritage	= in_drakkin_heritage;
	drakkin_tattoo		= in_drakkin_tattoo;
	drakkin_details		= in_drakkin_details;
	attack_speed= 0;
	findable	= false;
	trackable	= true;

	if(in_aa_title>0)
		aa_title	= in_aa_title;
	else
		aa_title	=0xFF;
	AC		= in_ac;
	ATK		= in_atk;
	STR		= in_str;
	STA		= in_sta;
	DEX		= in_dex;
	AGI		= in_agi;
	INT		= in_int;
	WIS		= in_wis;
	CHA		= in_cha;
	MR = CR = FR = DR = PR = 0;
	
	ExtraHaste = 0;
	bEnraged = false;

	shield_target = NULL;
	cur_mana = 0;
	max_mana = 0;
	hp_regen = in_hp_regen;
	mana_regen = in_mana_regen;
	oocregen = RuleI(NPC, OOCRegen); //default Out of Combat Regen
	invisible = false;
	invisible_undead = false;
	invisible_animals = false;
	sneaking = false;
	hidden = false;
	improved_hidden = false;
	invulnerable = false;
	IsFullHP	= (cur_hp == max_hp);
	qglobal=0;

	int i = 0;

	uint32 j;
	for (j = 0; j < BUFF_COUNT; j++) {
		buffs[j].spellid = SPELL_UNKNOWN;
	}

    // clear the proc arrays
	for (j = 0; j < MAX_PROCS; j++)
    {
        PermaProcs[j].spellID = SPELL_UNKNOWN;
        PermaProcs[j].chance = 0;
        PermaProcs[j].pTimer = NULL;
        SpellProcs[j].spellID = SPELL_UNKNOWN;

		DefensiveProcs[j].spellID = SPELL_UNKNOWN;
		DefensiveProcs[j].chance = 0;
		DefensiveProcs[j].pTimer = NULL;
		RangedProcs[j].spellID = SPELL_UNKNOWN;
		RangedProcs[j].chance = 0;
		RangedProcs[j].pTimer = NULL;
    }

	for (i = 0; i < MAX_MATERIALS; i++)
	{
		if (in_armor_tint)
		{
			armor_tint[i] = in_armor_tint[i];
		}
		else
		{
			armor_tint[i] = 0;
		}
	}

	delta_heading = 0;
	delta_x = 0;
	delta_y = 0;
	delta_z = 0;
	animation = 0;

	logging_enabled = false;
	isgrouped = false;
	israidgrouped = false;
	_appearance = eaStanding;
	pRunAnimSpeed = 0;
//	guildeqid = GUILD_NONE;
	
    spellend_timer.Disable();
	cheat_timer.Disable();
	bardsong_timer.Disable();
	bardsong = 0;
	bardsong_target_id = 0;
	casting_spell_id = 0;
	target = 0;
	
	memset(&itembonuses, 0, sizeof(StatBonuses));
	memset(&spellbonuses, 0, sizeof(StatBonuses));
	//memset(&aabonuses, 0, sizeof(StatBonuses)); //don't need this until we start using Client::CalcAABonuses()
	spellbonuses.AggroRange = -1;
	spellbonuses.AssistRange = -1;
	pLastChange = 0;
	SetPetID(0);
	SetOwnerID(0);
	typeofpet = petCharmed;		//default to charmed...
	held = false;
	
	attacked_count = 0;
	mezzed = false;
	stunned = false;
	silenced = false;
	inWater = false;
    int m;
/*	for (m = 0; m < 60; m++) {
		flag[m]=0;
	}*/
	for (m = 0; m < MAX_SHIELDERS; m++)
	{
		shielder[m].shielder_id = 0;
		shielder[m].shielder_bonus = 0;
	}
	for (i=0; i<SPECATK_MAXNUM ; i++) {
		SpecAttacks[i] = false;
		SpecAttackTimers[i] = 0;
	}
    memset( RampageArray, 0, sizeof(RampageArray));
	wandertype=0;
	pausetype=0;
	cur_wp=0;
	cur_wp_x = 0;
	cur_wp_y = 0;
	cur_wp_z = 0;
	cur_wp_pause = 0;
	patrol=0;
	follow=0;
	flee_mode = false;
	fear_walkto_x = -999999;
	fear_walkto_y = -999999;
	fear_walkto_z = -999999;
	curfp = false;	
	flee_timer.Start();

	permarooted = (runspeed > 0) ? false : true;

	movetimercompleted = false;
	roamer = false;
	rooted = false;
	charmed = false;
	pStandingPetOrder = SPO_Follow;

	see_invis = in_see_invis;
	see_invis_undead = in_see_invis_undead;
	see_hide = in_see_hide;
	see_improved_hide = in_see_improved_hide;
	qglobal=in_qglobal;
	
	// Bind wound
	bindwound_timer.Disable();
	bindwound_target = 0;
	
	trade = new Trade(this);
	// hp event
	nexthpevent = -1;
	nextinchpevent = -1;
	
	fix_pathing = false;
	TempPets(false);
	SetHasRune(false);
	SetHasSpellRune(false);

	m_is_running = false;

	flymode = FlyMode3;
	// Pathing
	PathingLOSState = UnknownLOS;
	PathingLoopCount = 0;
	PathingLastNodeVisited = -1;
	PathingLOSCheckTimer = new Timer(RuleI(Pathing, LOSCheckFrequency));
	PathingRouteUpdateTimerShort = new Timer(RuleI(Pathing, RouteUpdateFrequencyShort));
	PathingRouteUpdateTimerLong = new Timer(RuleI(Pathing, RouteUpdateFrequencyLong));
	AggroedAwayFromGrid = 0;
	PathingTraversedNodes = 0;
#ifdef EQBOTS

	// eqoffline
	if(database.GetBotStatus(GetNPCTypeID()) == 1) {
		AmIaBot = true;
	}
	else {
		AmIaBot = false;
	}
	cast_last_time = false;
	BotOwner = NULL;
	BotRaidID = 0;
	BotRaiding = false;
	OrderBotAttack = false;
	BotArchery = false;
	BotArcheryRange = 0;
	BotCharmer = false;
	PetChooser = false;

#endif //EQBOTS

}

Mob::~Mob()
{
	// Our Entity ID is set to 0 in NPC::Death. This leads to mobs hanging around for a while in 
	// the entity list, even after they have been destroyed. Use our memory pointer to remove the mob 
	// if our EntityID is 0.
	//
	if(GetID() > 0)
		entity_list.RemoveMob(GetID());
	else
		entity_list.RemoveMob(this);

	AI_Stop();
	if (GetPet()) {
		if (GetPet()->Charmed())
			GetPet()->BuffFadeByEffect(SE_Charm);
		else
			SetPet(0);
	}
	for (int i=0; i<SPECATK_MAXNUM ; i++) {
		safe_delete(SpecAttackTimers[i]);
	}
	EQApplicationPacket app;
	CreateDespawnPacket(&app);
	Corpse* corpse = entity_list.GetCorpseByID(GetID());
	if(!corpse || (corpse && !corpse->IsPlayerCorpse()))
		entity_list.QueueClients(this, &app, true);
	
	entity_list.RemoveFromTargets(this);
	
	safe_delete(trade);
	if(HadTempPets()){
		entity_list.DestroyTempPets(this);
	}
	entity_list.UnMarkNPC(GetID());
	safe_delete(PathingLOSCheckTimer);
	safe_delete(PathingRouteUpdateTimerShort);
	safe_delete(PathingRouteUpdateTimerLong);
}

int32 Mob::GetAppearanceValue(EmuAppearance iAppearance) {
	switch (iAppearance) {
		// 0 standing, 1 sitting, 2 ducking, 3 lieing down, 4 looting
		case eaStanding: {
			return ANIM_STAND;
		}
		case eaSitting: {
			return ANIM_SIT;
		}
		case eaCrouching: {
			return ANIM_CROUCH;
		}
		case eaDead: {
			return ANIM_DEATH;
		}
		case eaLooting: {
			return ANIM_LOOT;
		}
		//to shup up compiler:
		case _eaMaxAppearance:
			break;
	}
	return(ANIM_STAND);
}
int32 Mob::GetPRange(float x, float y, float z){
	return 0;
}
void Mob::SetInvisible(bool state)
{
	invisible = state;
	SendAppearancePacket(AT_Invis, invisible);
    // Invis and hide breaks charms

    if ((this->GetPetType() == petCharmed) && (invisible || hidden || improved_hidden))
    {
        Mob* formerpet = this->GetPet();

        if(formerpet)
             formerpet->BuffFadeByEffect(SE_Charm);
    }
   
}

//check to see if `this` is invisible to `other`
bool Mob::IsInvisible(Mob* other) const
{
	if(!other)
		return(false);
	
	//check regular invisibility
	if (invisible && !other->SeeInvisible())
		return true;
	
	//check invis vs. undead
	if (other->GetBodyType() == BT_Undead || other->GetBodyType() == BT_SummonedUndead) {
		if(invisible_undead && !other->SeeInvisibleUndead())
			return true;
	}
	
	//check invis vs. animals...
	if (other->GetBodyType() == BT_Animal){
		if(invisible_animals && !other->SeeInvisible())
			return true;
	}

	if(hidden){
		if(!other->see_hide && !other->see_improved_hide){
			return true;
		}
	}

	if(improved_hidden){
		if(!other->see_improved_hide){
			return true;
		}
	}
	
	//handle sneaking
	if(sneaking) {
		if(BehindMob(other, GetX(), GetY()) )
			return true;
	}
	
	return(false);
}

float Mob::_GetMovementSpeed(int mod) const {
      // List of movement speed modifiers, including AAs & spells:
	// http://everquest.allakhazam.com/db/item.html?item=1721;page=1;howmany=50#m10822246245352
	if (IsRooted())
		return 0.0f;
	
	float speed_mod = 1.0f;
	
	if (IsClient()){
            speed_mod += ((CastToClient()->GetAA(aaInnateRunSpeed) * 0.10)
			+ (CastToClient()->GetAA(aaFleetofFoot) * 0.10)
			+ (CastToClient()->GetAA(aaSwiftJourney) * 0.10)
		);
		//Selo's Enduring Cadence should be +7% per level
	}
	
	int movemod = spellbonuses.movementspeed + itembonuses.movementspeed + mod;
	
	if(movemod < -85) //cap it at moving very very slow
		movemod = -85;
	
	if (movemod != 0)
		speed_mod += float(movemod) / 100.0f;

	if(speed_mod <= 0.0f)
		return(0.0001f);

	//runspeed cap.
	if(GetClass() == BARD) {
		//this extra-high bard cap should really only apply if they have AAs
		if(speed_mod > 1.74)
			speed_mod = 1.74;
	} else {
		if(speed_mod > 1.58)
			speed_mod = 1.58;
	}
	
	return(runspeed * speed_mod);
}

sint32 Mob::CalcMaxMana()
{

#ifdef EQBOTS

	if(IsBot()) {
		sint32 WisInt = 0;
		sint32 MindLesserFactor, MindFactor;
		switch (GetCasterClass()) {
		case 'I':
			WisInt = GetINT();
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				max_mana = (((5 * (MindFactor + 20)) / 2) * 3 * GetLevel() / 40);
			}
			else {
				max_mana = (((5 * (MindFactor + 200)) / 2) * 3 * GetLevel() / 100);
			}
			max_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		case 'W':
			WisInt = GetWIS();
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				max_mana = (((5 * (MindFactor + 20)) / 2) * 3 * GetLevel() / 40);
			}
			else {
				max_mana = (((5 * (MindFactor + 200)) / 2) * 3 * GetLevel() / 100);
			}
			max_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		default:
			max_mana = 0;
			break;
		}
		return max_mana;
	}

#endif //EQBOTS

	switch (GetCasterClass()) {
		case 'I':
			max_mana = (((GetINT()/2)+1) * GetLevel()) + spellbonuses.Mana + itembonuses.Mana;
			break;
		case 'W':
			max_mana = (((GetWIS()/2)+1) * GetLevel()) + spellbonuses.Mana + itembonuses.Mana;
			break;
		case 'N':
		default:
			max_mana = 0;
			break;
	}

	return max_mana;
}

char Mob::GetCasterClass() const {
	switch(class_)
	{
	case CLERIC:
	case PALADIN:
	case RANGER:
	case DRUID:
	case SHAMAN:
	case BEASTLORD:
	case CLERICGM:
	case PALADINGM:
	case RANGERGM:
	case DRUIDGM:
	case SHAMANGM:
	case BEASTLORDGM:
		return 'W';
		break;

	case SHADOWKNIGHT:
	case BARD:
	case NECROMANCER:
	case WIZARD:
	case MAGICIAN:
	case ENCHANTER:
	case SHADOWKNIGHTGM:
	case BARDGM:
	case NECROMANCERGM:
	case WIZARDGM:
	case MAGICIANGM:
	case ENCHANTERGM:
		return 'I';
		break;

	default:
		return 'N';
		break;
	}
}

void Mob::CreateSpawnPacket(EQApplicationPacket* app, Mob* ForWho) {
	app->SetOpcode(OP_NewSpawn);
	app->size = sizeof(NewSpawn_Struct);
	app->pBuffer = new uchar[app->size];
	memset(app->pBuffer, 0, app->size);	
	NewSpawn_Struct* ns = (NewSpawn_Struct*)app->pBuffer;
	FillSpawnStruct(ns, ForWho);
}

void Mob::CreateSpawnPacket(EQApplicationPacket* app, NewSpawn_Struct* ns) {
	app->SetOpcode(OP_NewSpawn);
	app->size = sizeof(NewSpawn_Struct);
	
	app->pBuffer = new uchar[sizeof(NewSpawn_Struct)];
	
	// Copy ns directly into packet
	memcpy(app->pBuffer, ns, sizeof(NewSpawn_Struct));
	
	// Custom packet data
	NewSpawn_Struct* ns2 = (NewSpawn_Struct*)app->pBuffer;
	strcpy(ns2->spawn.name, ns->spawn.name);
	/*if (ns->spawn.class_==MERCHANT)
		strcpy(ns2->spawn.lastName, "EQEmu Shopkeeper");
	else*/ if (ns->spawn.class_==TRIBUTE_MASTER)
		strcpy(ns2->spawn.lastName, "Tribute Master");
	else if (ns->spawn.class_==ADVENTURERECRUITER)
		strcpy(ns2->spawn.lastName, "Adventure Recruiter");
	else if (ns->spawn.class_==BANKER)
		strcpy(ns2->spawn.lastName, "Banker");
	else if (ns->spawn.class_==ADVENTUREMERCHANT)
		strcpy(ns->spawn.lastName,"Adventure Merchant");
	else if (ns->spawn.class_==WARRIORGM)
		strcpy(ns2->spawn.lastName, "GM Warrior");
	else if (ns->spawn.class_==PALADINGM)
		strcpy(ns2->spawn.lastName, "GM Paladin");
	else if (ns->spawn.class_==RANGERGM)
		strcpy(ns2->spawn.lastName, "GM Ranger");
	else if (ns->spawn.class_==SHADOWKNIGHTGM)
		strcpy(ns2->spawn.lastName, "GM ShadowKnight");
	else if (ns->spawn.class_==DRUIDGM)
		strcpy(ns2->spawn.lastName, "GM Druid");
	else if (ns->spawn.class_==BARDGM)
		strcpy(ns2->spawn.lastName, "GM Bard");
	else if (ns->spawn.class_==ROGUEGM)
		strcpy(ns2->spawn.lastName, "GM Rogue");
	else if (ns->spawn.class_==SHAMANGM)
		strcpy(ns2->spawn.lastName, "GM Shaman");
	else if (ns->spawn.class_==NECROMANCERGM)
		strcpy(ns2->spawn.lastName, "GM Necromancer");
	else if (ns->spawn.class_==WIZARDGM)
		strcpy(ns2->spawn.lastName, "GM Wizard");
	else if (ns->spawn.class_==MAGICIANGM)
		strcpy(ns2->spawn.lastName, "GM Magician");
	else if (ns->spawn.class_==ENCHANTERGM)
		strcpy(ns2->spawn.lastName, "GM Enchanter");
	else if (ns->spawn.class_==BEASTLORDGM)
		strcpy(ns2->spawn.lastName, "GM Beastlord");
	else if (ns->spawn.class_==BERSERKERGM)
		strcpy(ns2->spawn.lastName, "GM Berserker");
	else
		strcpy(ns2->spawn.lastName, ns->spawn.lastName);
	memset(&app->pBuffer[sizeof(Spawn_Struct)-7],0xFF,7);
}

void Mob::FillSpawnStruct(NewSpawn_Struct* ns, Mob* ForWho)
{
	int i;

	strcpy(ns->spawn.name, name);
	if(IsClient())
		strncpy(ns->spawn.lastName,lastname,sizeof(lastname));
	ns->spawn.heading	= FloatToEQ19(heading);
	ns->spawn.x			= FloatToEQ19(x_pos);//((sint32)x_pos)<<3;
	ns->spawn.y			= FloatToEQ19(y_pos);//((sint32)y_pos)<<3;
	ns->spawn.z			= FloatToEQ19(z_pos);//((sint32)z_pos)<<3;
	ns->spawn.spawnId	= GetID();
	ns->spawn.curHp	= (sint16)GetHPRatio();
	ns->spawn.max_hp	= 100;		//this field needs a better name
	ns->spawn.race		= race;
	ns->spawn.runspeed	= runspeed;
	ns->spawn.walkspeed	= runspeed * 0.5f;
	ns->spawn.class_	= class_;
	ns->spawn.gender	= gender;
	ns->spawn.level		= level;
	ns->spawn.deity		= deity;
	ns->spawn.animation	= 0;
	ns->spawn.findable	= findable?1:0;
// vesuvias - appearence fix
	ns->spawn.light		= light;
	ns->spawn.showhelm = 1;	

	ns->spawn.invis		= (invisible || hidden) ? 1 : 0;	// TODO: load this before spawning players
	ns->spawn.NPC		= IsClient() ? 0 : 1;
	ns->spawn.petOwnerId	= ownerid;

	ns->spawn.haircolor = haircolor;
	ns->spawn.beardcolor = beardcolor;
	ns->spawn.eyecolor1 = eyecolor1;
	ns->spawn.eyecolor2 = eyecolor2;
	ns->spawn.hairstyle = hairstyle;
	ns->spawn.face = luclinface;
	ns->spawn.beard = beard;
	ns->spawn.drakkin_heritage = drakkin_heritage;
	ns->spawn.drakkin_tattoo = drakkin_tattoo;
	ns->spawn.drakkin_details = drakkin_details;
	ns->spawn.equip_chest2  = texture;

//	ns->spawn.invis2 = 0xff;//this used to be labeled beard.. if its not FF it will turn
								   //mob invis

	if(helmtexture && helmtexture != 0xFF)
	{
		//ns->spawn.equipment[MATERIAL_HEAD] = helmtexture;
		ns->spawn.helm=helmtexture;
	} else {
		//ns->spawn.equipment[MATERIAL_HEAD] = 0;
		ns->spawn.helm = 0;
	}
	
	ns->spawn.guildrank	= 0xFF;
	ns->spawn.size			= size;
	ns->spawn.bodytype = bodytype;
	// The 'flymode' settings have the following effect:
	// 0 - Mobs in water sink like a stone to the bottom
	// 1 - Same as #flymode 1
	// 2 - Same as #flymode 2
	// 3 - Mobs in water do not sink. A value of 3 in this field appears to be the default setting for all mobs
	//     (in water or not) according to 6.2 era packet collects.
	if(IsClient())
		ns->spawn.flymode = 0;
	else
		ns->spawn.flymode = flymode;

	ns->spawn.lastName[0] = '\0';
	
	strncpy(ns->spawn.lastName, lastname, sizeof(lastname));

	for(i = 0; i < MAX_MATERIALS; i++)
	{
		ns->spawn.equipment[i] = GetEquipmentMaterial(i);
		if (armor_tint[i])
		{
			ns->spawn.colors[i].color = armor_tint[i];
		}
		else
		{
			ns->spawn.colors[i].color = GetEquipmentColor(i);
		}
	}
	
	memset(ns->spawn.set_to_0xFF, 0xFF, sizeof(ns->spawn.set_to_0xFF));
	
}

void Mob::CreateDespawnPacket(EQApplicationPacket* app)
{
	app->SetOpcode(OP_DeleteSpawn);
	app->size = sizeof(DeleteSpawn_Struct);
	app->pBuffer = new uchar[app->size];
	memset(app->pBuffer, 0, app->size);
	DeleteSpawn_Struct* ds = (DeleteSpawn_Struct*)app->pBuffer;
	ds->spawn_id = GetID();
}

void Mob::CreateHPPacket(EQApplicationPacket* app)
{ 
	this->IsFullHP=(cur_hp>=max_hp); 
	app->SetOpcode(OP_MobHealth); 
	app->size = sizeof(SpawnHPUpdate_Struct2); 
	app->pBuffer = new uchar[app->size]; 
	memset(app->pBuffer, 0, sizeof(SpawnHPUpdate_Struct2)); 
	SpawnHPUpdate_Struct2* ds = (SpawnHPUpdate_Struct2*)app->pBuffer; 

	ds->spawn_id = GetID(); 
	// they don't need to know the real hp
	ds->hp = (int)GetHPRatio();
 
	// hp event 
	if ( IsNPC() && ( GetNextHPEvent() > 0 ) ) { 
		if ( ds->hp < GetNextHPEvent() ) { 
			int lasthpevent = nexthpevent;
			parse->Event(EVENT_HP, GetNPCTypeID(), 0, CastToNPC(), NULL);
			if ( lasthpevent == nexthpevent ) {
				SetNextHPEvent(-1);
			}
		} 
	} 

	if ( IsNPC() && ( GetNextIncHPEvent() > 0 ) ) { 
		if ( ds->hp > GetNextIncHPEvent() ) { 
			int lastinchpevent = nextinchpevent;
			parse->Event(EVENT_HP, GetNPCTypeID(), 0, CastToNPC(), NULL);
			if ( lastinchpevent == nextinchpevent ) {
				SetNextIncHPEvent(-1);
			}
		} 
 	}   

#if 0	// solar: old stuff, leaving while testing changes
	// we dont give the actual hp of npcs
	if(IsNPC() || GetMaxHP() > 30000)
	{
		ds->cur_hp = (int)GetHPRatio();
		ds->max_hp = 100;
	}
	else
	{
		if(IsClient())
		{
			ds->cur_hp = CastToClient()->GetHP() - itembonuses.HP;
			ds->max_hp = CastToClient()->GetMaxHP() - itembonuses.HP;
#ifdef SOLAR
			Message(0, "HP: %d/%d", ds->cur_hp, ds->max_hp);
#endif
		}
		else
		{
			ds->cur_hp = GetHP();
			ds->max_hp = GetMaxHP();
		}
	}
#endif
} 

// sends hp update of this mob to people who might care
void Mob::SendHPUpdate()
{
	EQApplicationPacket hp_app;
	Group *group;
	
	// destructor will free the pBuffer
 	CreateHPPacket(&hp_app);

#ifdef MANAGE_HP_UPDATES
	entity_list.QueueManaged(this, &hp_app, true);
#else
	// send to people who have us targeted
 	entity_list.QueueClientsByTarget(this, &hp_app, false, 0, false);
	entity_list.QueueToGroupsForNPCHealthAA(this, &hp_app);

	// send to group
	if(IsGrouped())
	{
		group = entity_list.GetGroupByMob(this);
		if(group)	//not sure why this might be null, but it happens
			group->SendHPPacketsFrom(this);
	}	

	if(IsClient()){
		Raid *r = entity_list.GetRaidByClient(CastToClient());
		if(r){
			r->SendHPPacketsFrom(CastToClient());
		}
	}

	// send to master
	if(GetOwner() && GetOwner()->IsClient())
	{
		GetOwner()->CastToClient()->QueuePacket(&hp_app, false);
	}

	// send to pet
	if(GetPet() && GetPet()->IsClient())
	{
		GetPet()->CastToClient()->QueuePacket(&hp_app, false);
	}
#endif	//MANAGE_HP_PACKETS

	// send to self - we need the actual hps here
	if(IsClient())
	{
		EQApplicationPacket* hp_app2 = new EQApplicationPacket(OP_HPUpdate,sizeof(SpawnHPUpdate_Struct));
		SpawnHPUpdate_Struct* ds = (SpawnHPUpdate_Struct*)hp_app2->pBuffer; 
		ds->cur_hp = CastToClient()->GetHP() - itembonuses.HP;
		ds->spawn_id = GetID();
		ds->max_hp = CastToClient()->GetMaxHP() - itembonuses.HP;
		CastToClient()->QueuePacket(hp_app2);
		safe_delete(hp_app2);
	}
}

// this one just warps the mob to the current location
void Mob::SendPosition() {
	EQApplicationPacket* app = new EQApplicationPacket(OP_ClientUpdate, sizeof(PlayerPositionUpdateServer_Struct));
	PlayerPositionUpdateServer_Struct* spu = (PlayerPositionUpdateServer_Struct*)app->pBuffer;	
	MakeSpawnUpdateNoDelta(spu);
//?	spu->heading *= 8;
#ifdef PACKET_UPDATE_MANAGER
	entity_list.QueueManaged(this, app, true);
#else
	entity_list.QueueCloseClients(this, app, true, 800);
#endif
	safe_delete(app);
}

// this one just warps the mob to the current location
void Mob::SendAllPosition() {
	EQApplicationPacket* app = new EQApplicationPacket(OP_ClientUpdate, sizeof(PlayerPositionUpdateServer_Struct));
	PlayerPositionUpdateServer_Struct* spu = (PlayerPositionUpdateServer_Struct*)app->pBuffer;	
	MakeSpawnUpdateNoDelta(spu);
//?	spu->heading *= 8;
	entity_list.QueueClients(this, app, true);
	safe_delete(app);
}

// this one is for mobs on the move, with deltas - this makes them walk
void Mob::SendPosUpdate(int8 iSendToSelf) {
	EQApplicationPacket* app = new EQApplicationPacket(OP_ClientUpdate, sizeof(PlayerPositionUpdateServer_Struct));
	PlayerPositionUpdateServer_Struct* spu = (PlayerPositionUpdateServer_Struct*)app->pBuffer;
	MakeSpawnUpdate(spu);
	if (iSendToSelf == 2) {
		if (this->IsClient())
			this->CastToClient()->FastQueuePacket(&app,false);
	}
	else
#ifdef PACKET_UPDATE_MANAGER
		entity_list.QueueManaged(this, app, (iSendToSelf==0),false);
#else
		entity_list.QueueCloseClients(this, app, (iSendToSelf==0), 800, NULL, false);
#endif
	safe_delete(app);
}

// this is for SendPosition()
void Mob::MakeSpawnUpdateNoDelta(PlayerPositionUpdateServer_Struct *spu){
	memset(spu,0xff,sizeof(PlayerPositionUpdateServer_Struct));
	spu->spawn_id	= GetID();
	spu->x_pos		= FloatToEQ19(x_pos);
	spu->y_pos		= FloatToEQ19(y_pos);
	spu->z_pos		= FloatToEQ19(z_pos);
	spu->delta_x	= NewFloatToEQ13(0);
	spu->delta_y	= NewFloatToEQ13(0);
	spu->delta_z	= NewFloatToEQ13(0);
	spu->heading	= FloatToEQ19(heading);
	spu->animation	= 0;
	spu->delta_heading = NewFloatToEQ13(0);
	spu->padding0002	=0;
	spu->padding0006	=7;
	spu->padding0014	=0x7f;
	spu->padding0018	=0x5df27;

}

// this is for SendPosUpdate()
void Mob::MakeSpawnUpdate(PlayerPositionUpdateServer_Struct* spu) {
	spu->spawn_id	= GetID();
	spu->x_pos		= FloatToEQ19(x_pos);
	spu->y_pos		= FloatToEQ19(y_pos);
	spu->z_pos		= FloatToEQ19(z_pos);
	spu->delta_x	= NewFloatToEQ13(delta_x);
	spu->delta_y	= NewFloatToEQ13(delta_y);
	spu->delta_z	= NewFloatToEQ13(delta_z);
	spu->heading	= FloatToEQ19(heading);
	spu->padding0002	=0;
	spu->padding0006	=7;
	spu->padding0014	=0x7f;
	spu->padding0018	=0x5df27;
	if(this->IsClient())
		spu->animation = animation;
	else
		spu->animation	= pRunAnimSpeed;//animation;
	spu->delta_heading = NewFloatToEQ13(delta_heading);
}

void Mob::ShowStats(Client* client) {

	int16 attackRating = 0;
	int16 WornCap = GetATK();

	if(IsClient())
		attackRating = GetATK() + ((GetSTR() + GetSkill(OFFENSE)) * 9 / 10);
	else
		attackRating = GetATK() + (GetSTR() * 9 / 10);

	if(WornCap > 250)
		WornCap = 250;

	client->Message(0, "Name: %s %s", GetName(), lastname);
	client->Message(0, "  Level: %i  MaxHP: %i  CurHP: %i  AC: %i  Class: %i", GetLevel(), GetMaxHP(), GetHP(), GetAC(), GetClass());
	client->Message(0, "  MaxMana: %i  CurMana: %i  Size: %1.1f", GetMaxMana(), GetMana(), GetSize());
	client->Message(0, "  Total ATK: %i  Worn ATK: %i  Worn ATK Capped: %i  Server Used ATK: %i", this->CastToClient()->GetTotalATK(), GetATK(), WornCap, attackRating);
	client->Message(0, "  STR: %i  STA: %i  DEX: %i  AGI: %i  INT: %i  WIS: %i  CHA: %i", GetSTR(), GetSTA(), GetDEX(), GetAGI(), GetINT(), GetWIS(), GetCHA());
	client->Message(0, "  MR: %i  PR: %i  FR: %i  CR: %i  DR: %i", GetMR(), GetPR(), GetFR(), GetCR(), GetDR());
	client->Message(0, "  Race: %i  BaseRace: %i  Texture: %i  HelmTexture: %i  Gender: %i  BaseGender: %i", GetRace(), GetBaseRace(), GetTexture(), GetHelmTexture(), GetGender(), GetBaseGender());
	client->Message(0, "  Last Warp Distance: %f Threshold Remaining: %f", GetLWDistance(), GetWarpThreshold());
	if (client->Admin() >= 100) {
		client->Message(0, "  EntityID: %i  PetID: %i  OwnerID: %i  AIControlled: %i  Targetted: %i", 
				this->GetID(), this->GetPetID(), this->GetOwnerID(), this->IsAIControlled(), targeted);
		if (this->IsClient()) {
			client->Message(0, "  CharID: %i  PetID: %i", this->CastToClient()->CharacterID(), this->GetPetID());
			client->Message(0, "  Endurance: %i, Max Endurance %i",client->GetEndurance(), client->GetMaxEndurance());
		}
		else if (this->IsCorpse()) {
			if (this->IsPlayerCorpse()) {
				client->Message(0, "  CharID: %i  PlayerCorpse: %i", this->CastToCorpse()->GetCharID(), this->CastToCorpse()->GetDBID());
			}
			else {
				client->Message(0, "  NPCCorpse", this->GetID());
			}
		}
		else if (this->IsNPC()) {
			int32 spawngroupid = 0;
			if(this->CastToNPC()->respawn2 != 0)
				spawngroupid = this->CastToNPC()->respawn2->SpawnGroupID();
			client->Message(0, "  NPCID: %u  SpawnGroupID: %u LootTable: %u  FactionID: %i  SpellsID: %u MerchantID: %i", this->GetNPCTypeID(),spawngroupid, this->CastToNPC()->GetLoottableID(), this->CastToNPC()->GetNPCFactionID(), this->CastToNPC()->GetNPCSpellsID(),this->CastToNPC()->MerchantType);
			client->Message(0, "  Accuracy: %i", CastToNPC()->GetAccuracyRating());
		}
		if (this->IsAIControlled()) {
			client->Message(0, "  AIControlled: AggroRange: %1.0f  AssistRange: %1.0f", this->GetAggroRange(), this->GetAssistRange());
		}
	}
}

void Mob::DoAnim(const int animnum, int type, bool ackreq, eqFilterType filter) {
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Animation, sizeof(Animation_Struct));
	Animation_Struct* anim = (Animation_Struct*)outapp->pBuffer;
	anim->spawnid = GetID();
	if(type == 0){
		anim->action = 10;
		anim->value=animnum;
	}
	else{
		anim->action = animnum;
		anim->value=type;
	}
	entity_list.QueueCloseClients(this, outapp, false, 200, 0, ackreq, filter);
	safe_delete(outapp);
}

void Mob::ShowBuffs(Client* client) {
	if (!spells_loaded)
		return;
	client->Message(0, "Buffs on: %s", this->GetName());
	uint32 i;
	for (i=0; i < BUFF_COUNT; i++) {
		if (buffs[i].spellid != SPELL_UNKNOWN) {
			if (buffs[i].durationformula == DF_Permanent)
				client->Message(0, "  %i: %s: Permanent", i, spells[buffs[i].spellid].name);
			else
				client->Message(0, "  %i: %s: %i tics left", i, spells[buffs[i].spellid].name, buffs[i].ticsremaining);

		}
	}
	if (IsClient()){
		client->Message(0, "itembonuses:");
		client->Message(0, "Atk:%i Ac:%i HP(%i):%i Mana:%i", itembonuses.ATK, itembonuses.AC, itembonuses.HPRegen, itembonuses.HP, itembonuses.Mana);
		client->Message(0, "Str:%i Sta:%i Dex:%i Agi:%i Int:%i Wis:%i Cha:%i",
			itembonuses.STR,itembonuses.STA,itembonuses.DEX,itembonuses.AGI,itembonuses.INT,itembonuses.WIS,itembonuses.CHA);
		client->Message(0, "SvMagic:%i SvFire:%i SvCold:%i SvPoison:%i SvDisease:%i",
				itembonuses.MR,itembonuses.FR,itembonuses.CR,itembonuses.PR,itembonuses.DR);
		client->Message(0, "DmgShield:%i Haste:%i", itembonuses.DamageShield, itembonuses.haste );
		client->Message(0, "spellbonuses:");
		client->Message(0, "Atk:%i Ac:%i HP(%i):%i Mana:%i", spellbonuses.ATK, spellbonuses.AC, spellbonuses.HPRegen, spellbonuses.HP, spellbonuses.Mana);
		client->Message(0, "Str:%i Sta:%i Dex:%i Agi:%i Int:%i Wis:%i Cha:%i",
			spellbonuses.STR,spellbonuses.STA,spellbonuses.DEX,spellbonuses.AGI,spellbonuses.INT,spellbonuses.WIS,spellbonuses.CHA);
		client->Message(0, "SvMagic:%i SvFire:%i SvCold:%i SvPoison:%i SvDisease:%i",
				spellbonuses.MR,spellbonuses.FR,spellbonuses.CR,spellbonuses.PR,spellbonuses.DR);
		client->Message(0, "DmgShield:%i Haste:%i", spellbonuses.DamageShield, spellbonuses.haste );
	}
}

#ifdef EQBOTS //Angelox
void Mob::BotEffect(Client* client) {
	if (!spells_loaded)
		return;
	uint32 i;
	for (i=0; i < BUFF_COUNT; i++) {
		if ((buffs[i].spellid == 3197) ||
		(buffs[i].spellid == 45) ||
		(buffs[i].spellid == 47) ||
		(buffs[i].spellid == 501) ||
		(buffs[i].spellid == 208)) {
		   client->Message(0, "Target pacified.");
		}
	}
}
#endif //EQBOTS	


void Mob::ShowBuffList(Client* client) {
	if (!spells_loaded)
		return;

	client->Message(0, "Buffs on: %s", this->GetCleanName());
	uint32 i;
	for (i=0; i < BUFF_COUNT; i++) {
		if (buffs[i].spellid != SPELL_UNKNOWN) {
			if (buffs[i].durationformula == DF_Permanent)
				client->Message(0, "  %i: %s: Permanent", i, spells[buffs[i].spellid].name);
			else
				client->Message(0, "  %i: %s: %i tics left", i, spells[buffs[i].spellid].name, buffs[i].ticsremaining);
		}
	}
}

void Mob::GMMove(float x, float y, float z, float heading, bool SendUpdate) {

	Route.clear();

	x_pos = x;
	y_pos = y;
	z_pos = z;
	if (heading != 0.01)
		this->heading = heading;
	if(IsNPC())
		CastToNPC()->SaveGuardSpot(true);
	if(SendUpdate)
		SendAllPosition();
	//SendPosUpdate(1);
#ifdef PACKET_UPDATE_MANAGER
	if(IsClient()) {
		CastToClient()->GetUpdateManager()->FlushQueues();
	}
#endif
}

void Mob::SendIllusionPacket(int16 in_race, int8 in_gender, int8 in_texture, int8 in_helmtexture, int8 in_haircolor, int8 in_beardcolor, int8 in_eyecolor1, int8 in_eyecolor2, int8 in_hairstyle, int8 in_luclinface, int8 in_beard, int8 in_aa_title, int32 in_drakkin_heritage, int32 in_drakkin_tattoo, int32 in_drakkin_details, float in_size) {

	int16 BaseRace = GetBaseRace();

	if (in_race == 0) {
		this->race = BaseRace;
		if (in_gender == 0xFF)
			this->gender = GetBaseGender();
		else
			this->gender = in_gender;
	}
	else {
		this->race = in_race;
		if (in_gender == 0xFF) {
			int8 tmp = Mob::GetDefaultGender(this->race, gender);
			if (tmp == 2)
				gender = 2;
			else if (gender == 2 && GetBaseGender() == 2)
				gender = tmp;
			else if (gender == 2)
				gender = GetBaseGender();
		}
		else
			gender = in_gender;
	}
	if (in_texture == 0xFF) {
		if (in_race <= 12 || in_race == 128 || in_race == 130 || in_race == 330 || in_race == 522)
			this->texture = 0xFF;
		else
			this->texture = GetTexture();
	}
	else
		this->texture = in_texture;

	if (in_helmtexture == 0xFF) {
		if (in_race <= 12 || in_race == 128 || in_race == 130 || in_race == 330 || in_race == 522)
			this->helmtexture = 0xFF;
		else if (in_texture != 0xFF)
			this->helmtexture = in_texture;
		else
			this->helmtexture = GetHelmTexture();
	}
	else
		this->helmtexture = in_helmtexture;

	if (in_haircolor == 0xFF)
		this->haircolor = GetHairColor();
	else
		this->haircolor = in_haircolor;

	if (in_beardcolor == 0xFF)
		this->beardcolor = GetBeardColor();
	else
		this->beardcolor = in_beardcolor;

	if (in_eyecolor1 == 0xFF)
		this->eyecolor1 = GetEyeColor1();
	else
		this->eyecolor1 = in_eyecolor1;

	if (in_eyecolor2 == 0xFF)
		this->eyecolor2 = GetEyeColor2();
	else
		this->eyecolor2 = in_eyecolor2;

	if (in_hairstyle == 0xFF)
		this->hairstyle = GetHairStyle();
	else
		this->hairstyle = in_hairstyle;

	if (in_luclinface == 0xFF)
		this->luclinface = GetLuclinFace();
	else
		this->luclinface = in_luclinface;

	if (in_beard == 0xFF)
		this->beard	= GetBeard();
	else
		this->beard = in_beard;

	this->aa_title = 0xFF;

	if (in_drakkin_heritage == 0xFFFFFFFF)
		this->drakkin_heritage = GetDrakkinHeritage();
	else
		this->drakkin_heritage = in_drakkin_heritage;

	if (in_drakkin_tattoo == 0xFFFFFFFF)
		this->drakkin_tattoo = GetDrakkinTattoo();
	else
		this->drakkin_tattoo = in_drakkin_tattoo;

	if (in_drakkin_details == 0xFFFFFFFF)
		this->drakkin_details = GetDrakkinDetails();
	else
		this->drakkin_details = in_drakkin_details;

	if (in_size == 0xFFFFFFFF)
		this->size = GetSize();
	else
		this->size = in_size;

	// Forces the feature information to be pulled from the Player Profile
	if (this->IsClient() && in_race == 0) {
		this->race = CastToClient()->GetBaseRace();
		this->gender = CastToClient()->GetBaseGender();
		this->texture = 0xFF;
		this->helmtexture = 0xFF;
		this->haircolor = CastToClient()->GetBaseHairColor();
		this->beardcolor = CastToClient()->GetBaseBeardColor();
		this->eyecolor1 = CastToClient()->GetBaseEyeColor();
		this->eyecolor2 = CastToClient()->GetBaseEyeColor();
		this->hairstyle = CastToClient()->GetBaseHairStyle();
		this->luclinface = CastToClient()->GetBaseFace();
		this->beard	= CastToClient()->GetBaseBeard();
		this->aa_title = 0xFF;
		this->drakkin_heritage = CastToClient()->GetBaseHeritage();
		this->drakkin_tattoo = CastToClient()->GetBaseTattoo();
		this->drakkin_details = CastToClient()->GetBaseDetails();
		this->size = GetSize();
	}

	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Illusion, sizeof(Illusion_Struct));
	memset(outapp->pBuffer, 0, sizeof(outapp->pBuffer));
	Illusion_Struct* is = (Illusion_Struct*) outapp->pBuffer;
	is->spawnid = this->GetID();
	strcpy(is->charname, GetCleanName());
	is->race = this->race;
	is->gender = this->gender;
	is->texture = this->texture;
	is->helmtexture = this->helmtexture;
	is->haircolor = this->haircolor;
	is->beardcolor = this->beardcolor;
	is->beard = this->beard;
	is->eyecolor1 = this->eyecolor1;
	is->eyecolor2 = this->eyecolor2;
	is->hairstyle = this->hairstyle;
	is->face = this->luclinface;
	//is->aa_title = this->aa_title;
	is->drakkin_heritage = this->drakkin_heritage;
	is->drakkin_tattoo = this->drakkin_tattoo;
	is->drakkin_details = this->drakkin_details;
	is->size = this->size;

	entity_list.QueueClients(this, outapp);
	safe_delete(outapp);
	mlog(CLIENT__SPELLS, "Illusion: Race = %i, Gender = %i, Texture = %i, HelmTexture = %i, HairColor = %i, BeardColor = %i, EyeColor1 = %i, EyeColor2 = %i, HairStyle = %i, Face = %i, DrakkinHeritage = %i, DrakkinTattoo = %i, DrakkinDetails = %i, Size = %f",
		this->race, this->gender, this->texture, this->helmtexture, this->haircolor, this->beardcolor, this->eyecolor1, this->eyecolor2, this->hairstyle, this->luclinface, this->drakkin_heritage, this->drakkin_tattoo, this->drakkin_details, this->size);
}

int8 Mob::GetDefaultGender(int16 in_race, int8 in_gender) {
//cout << "Gender in:  " << (int)in_gender << endl;
	if ((in_race > 0 && in_race <= GNOME )
		|| in_race == IKSAR || in_race == VAHSHIR || in_race == FROGLOK || in_race == DRAKKIN
		|| in_race == 15 || in_race == 50 || in_race == 57 || in_race == 70 || in_race == 98 || in_race == 118) {
		if (in_gender >= 2) {
			// Female default for PC Races
			return 1;
		}
		else
			return in_gender;
	}
	else if (in_race == 44 || in_race == 52 || in_race == 55 || in_race == 65 || in_race == 67 || in_race == 88 || in_race == 117 || in_race == 127 ||
		in_race == 77 || in_race == 78 || in_race == 81 || in_race == 90 || in_race == 92 || in_race == 93 || in_race == 94 || in_race == 106 || in_race == 112 || in_race == 471) {
		// Male only races
		return 0;

	}
	else if (in_race == 25 || in_race == 56) {
		// Female only races
		return 1;
	}
	else {
		// Neutral default for NPC Races
		return 2;
	}
}

void Mob::SendAppearancePacket(int32 type, int32 value, bool WholeZone, bool iIgnoreSelf, Client *specific_target) {
	if (!GetID())
		return;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_SpawnAppearance, sizeof(SpawnAppearance_Struct));
	SpawnAppearance_Struct* appearance = (SpawnAppearance_Struct*)outapp->pBuffer;
	appearance->spawn_id = this->GetID();
	appearance->type = type;
	appearance->parameter = value;
	if (WholeZone)
		entity_list.QueueClients(this, outapp, iIgnoreSelf);
	else if(specific_target != NULL)
		specific_target->QueuePacket(outapp, false, Client::CLIENT_CONNECTED);
	else if (this->IsClient())
		this->CastToClient()->QueuePacket(outapp, false, Client::CLIENT_CONNECTED);
	safe_delete(outapp);
}

void Mob::SendLevelAppearance(){
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_LevelAppearance, sizeof(LevelAppearance_Struct));
	LevelAppearance_Struct* la = (LevelAppearance_Struct*)outapp->pBuffer;
	la->parm1 = 0x4D;
	la->parm2 = la->parm1 + 1;
	la->parm3 = la->parm2 + 1;
	la->parm4 = la->parm3 + 1;
	la->parm5 = la->parm4 + 1;
	la->spawn_id = GetID();
	la->value1a = 1;
	la->value2a = 2;
	la->value3a = 1;
	la->value3b = 1;
	la->value4a = 1;
	la->value4b = 1;
	la->value5a = 2;
	entity_list.QueueCloseClients(this,outapp);
	safe_delete(outapp);
}

const sint32& Mob::SetMana(sint32 amount)
{
	CalcMaxMana();
	sint32 mmana = GetMaxMana();
	cur_mana = amount < 0 ? 0 : (amount > mmana ? mmana : amount);
/*
	if(IsClient())
		LogFile->write(EQEMuLog::Debug, "Setting mana for %s to %d (%4.1f%%)", GetName(), amount, GetManaRatio());
*/

	return cur_mana;
}


void Mob::SetAppearance(EmuAppearance app, bool iIgnoreSelf) {
	if (_appearance != app) {
		_appearance = app;
		SendAppearancePacket(AT_Anim, GetAppearanceValue(app), true, iIgnoreSelf);
		if (this->IsClient() && this->IsAIControlled())
			SendAppearancePacket(AT_Anim, ANIM_FREEZE, false, false);
	}
}

void Mob::ChangeSize(float in_size = 0, bool bNoRestriction) {
	//Neotokyo's Size Code
	if (!bNoRestriction)
	{
		if (this->IsClient() || this->petid != 0)
			if (in_size < 3.0)
				in_size = 3.0;


			if (this->IsClient() || this->petid != 0)
				if (in_size > 15.0)
					in_size = 15.0;
	}


	if (in_size < 1.0)
		in_size = 1.0;

	if (in_size > 255.0)
		in_size = 255.0;
	//End of Neotokyo's Size Code
	this->size = in_size;
	SendAppearancePacket(AT_Size, (int32) in_size);
}

Mob* Mob::GetOwnerOrSelf() {
	if (!GetOwnerID())
		return this;
	Mob* owner = entity_list.GetMob(this->GetOwnerID());
	if (!owner) {
		SetOwnerID(0);
		return(this);
	}
	if (owner->GetPetID() == this->GetID()) {
		return owner;
	}
	if(IsNPC() && CastToNPC()->GetSwarmInfo()){
		return (CastToNPC()->GetSwarmInfo()->owner);
	}
	SetOwnerID(0);
	return this;
}

Mob* Mob::GetOwner() {
	Mob* owner = entity_list.GetMob(this->GetOwnerID());
	if (owner && owner->GetPetID() == this->GetID()) {

		return owner;
	}
	if(IsNPC() && CastToNPC()->GetSwarmInfo()){
		return (CastToNPC()->GetSwarmInfo()->owner);
	}
	SetOwnerID(0);
	return 0;
}

void Mob::SetOwnerID(int16 NewOwnerID) {
	if (NewOwnerID == GetID() && NewOwnerID != 0) // ok, no charming yourself now =p
		return;
	ownerid = NewOwnerID;
	if (ownerid == 0 && this->IsNPC() && this->GetPetType() != petCharmed)
		this->Depop();
}

//heko: for backstab
bool Mob::BehindMob(Mob* other, float playerx, float playery) const {
    if (!other)
        return true; // sure your behind your invisible friend?? (fall thru for sneak)
	//see if player is behind mob
	float angle, lengthb, vectorx, vectory;
	float mobx = -(other->GetX());	// mob xlocation (inverse because eq is confused)
	float moby = other->GetY();		// mobylocation
	float heading = other->GetHeading();	// mob heading
	heading = (heading * 360.0)/256.0;	// convert to degrees
	if (heading < 270)
		heading += 90;
	else
		heading -= 270;
	heading = heading*3.1415/180.0;	// convert to radians
	vectorx = mobx + (10.0 * cosf(heading));	// create a vector based on heading
	vectory = moby + (10.0 * sinf(heading));	// of mob length 10

	//length of mob to player vector
	//lengthb = (float)sqrtf(pow((-playerx-mobx),2) + pow((playery-moby),2));
	lengthb = (float) sqrtf( ( (-playerx-mobx) * (-playerx-mobx) ) + ( (playery-moby) * (playery-moby) ) );

	// calculate dot product to get angle
	angle = acosf(((vectorx-mobx)*(-playerx-mobx)+(vectory-moby)*(playery-moby)) / (10 * lengthb));
	angle = angle * 180 / 3.1415;
	if (angle > 90.0) //not sure what value to use (90*2=180 degrees is front)
		return true;
	else
		return false;
}

void Mob::SetZone(int32 zone_id, int32 instance_id)
{
	if(IsClient())
	{
		CastToClient()->GetPP().zone_id = zone_id;
		CastToClient()->GetPP().zoneInstance = instance_id;
	}
	Save();
}

void Mob::Kill() {
	Death(this, 0, SPELL_UNKNOWN, HAND_TO_HAND);
}

void Mob::SetAttackTimer() {
	float PermaHaste;
	if(GetHaste() > 0)
		PermaHaste = 1 / (1 + (float)GetHaste()/100);
	else if(GetHaste() < 0)
		PermaHaste = 1 * (1 - (float)GetHaste()/100);
	else
		PermaHaste = 1.0f;
	
	//default value for attack timer in case they have
	//an invalid weapon equipped:
	attack_timer.SetAtTrigger(4000, true);
	
	Timer* TimerToUse = NULL;
	const Item_Struct* PrimaryWeapon = NULL;
	
	for (int i=SLOT_RANGE; i<=SLOT_SECONDARY; i++) {
		
		//pick a timer
		if (i == SLOT_PRIMARY)
			TimerToUse = &attack_timer;
		else if (i == SLOT_RANGE)
			TimerToUse = &ranged_timer;
		else if(i == SLOT_SECONDARY)
			TimerToUse = &attack_dw_timer;
		else	//invalid slot (hands will always hit this)
			continue;
		
		const Item_Struct* ItemToUse = NULL;
		
		//find our item
		if (IsClient()) {
			ItemInst* ci = CastToClient()->GetInv().GetItem(i);
			if (ci)
				ItemToUse = ci->GetItem();
		} else if(IsNPC()) {
			int32 eid = CastToNPC()->GetEquipment(i);
			if(eid != 0)
				ItemToUse = database.GetItem(eid);
		}
		
		//special offhand stuff
		if(i == SLOT_SECONDARY) {
			//if we have a 2H weapon in our main hand, no dual
			if(PrimaryWeapon != NULL) {
				if(	PrimaryWeapon->ItemClass == ItemClassCommon
					&& (PrimaryWeapon->ItemType == ItemType2HS
					||	PrimaryWeapon->ItemType == ItemType2HB
					||	PrimaryWeapon->ItemType == ItemType2HPierce)) {
					attack_dw_timer.Disable();
					continue;
				}
			}

			//clients must have the skill to use it...
			if(IsClient()) {
				//if we cant dual weild, skip it
				if (!CanThisClassDualWield()) {
					attack_dw_timer.Disable();
					continue;
				}
			} else {
				//NPCs get it for free at 13
				if(GetLevel() < 13) {
					attack_dw_timer.Disable();
					continue;
				}
			}
		}
		
		//see if we have a valid weapon
		if(ItemToUse != NULL) {
			//check type and damage/delay
			if(ItemToUse->ItemClass != ItemClassCommon 
				|| ItemToUse->Damage == 0 
				|| ItemToUse->Delay == 0) {
				//no weapon
				ItemToUse = NULL;
			}
			// Check to see if skill is valid
			else if((ItemToUse->ItemType > ItemTypeThrowing) && (ItemToUse->ItemType != ItemTypeHand2Hand) && (ItemToUse->ItemType != ItemType2HPierce)) {
				//no weapon
				ItemToUse = NULL;
			}
		}

		//if we have no weapon..
		if (ItemToUse == NULL) {
			//above checks ensure ranged weapons do not fall into here
			// Work out if we're a monk
			if ((GetClass() == MONK) || (GetClass() == BEASTLORD)) {
				//we are a monk, use special delay
				int speed = (int)(GetMonkHandToHandDelay()*(100.0f+attack_speed)*PermaHaste);
				// neotokyo: 1200 seemed too much, with delay 10 weapons available
				if(speed < 800)	//lower bound
					speed = 800;
				TimerToUse->SetAtTrigger(speed, true);	// Hand to hand, delay based on level or epic
			} else {
				//not a monk... using fist, regular delay
				int speed = (int)(36*(100.0f+attack_speed)*PermaHaste);
				if(speed < 800 && IsClient())	//lower bound
					speed = 800;
				TimerToUse->SetAtTrigger(speed, true); 	// Hand to hand, non-monk 2/36
			}
		} else {
			//we have a weapon, use its delay
			// Convert weapon delay to timer resolution (milliseconds)
			//delay * 100
			int speed = (int)(ItemToUse->Delay*(100.0f+attack_speed)*PermaHaste);
			if(speed < 800)
				speed = 800;

			if(ItemToUse && (ItemToUse->ItemType == ItemTypeBow || ItemToUse->ItemType == ItemTypeThrowing))
			{
				if(IsClient())
				{
					float max_quiver = 0;
					for(int r = SLOT_PERSONAL_BEGIN; r <= SLOT_PERSONAL_END; r++) 
					{
						const ItemInst *pi = CastToClient()->GetInv().GetItem(r);
						if(!pi)
							continue;
						if(pi->IsType(ItemClassContainer) && pi->GetItem()->BagType == bagTypeQuiver)
						{
							float temp_wr = (pi->GetItem()->BagWR / 3);
							if(temp_wr > max_quiver)
							{
								max_quiver = temp_wr;
							}
						}
					}
					if(max_quiver > 0)
					{
						float quiver_haste = 1 / (1 + max_quiver / 100);
						speed *= quiver_haste;
					}
				}
			}
			TimerToUse->SetAtTrigger(speed, true);
		}
		
		if(i == SLOT_PRIMARY)
			PrimaryWeapon = ItemToUse;
	}
	
}

bool Mob::CanThisClassDualWield(void) const //Dual wield not Duel, busy someone else fix it (fixed! bUsh)
{

#ifdef EQBOTS

	if(IsBot()) {
		// Do nothing cuz we don't want all Bots to dual wield just because they are npc's level 13+
	}
	else

#endif //EQBOTS

	// All npcs over level 13 can dual wield
	if (this->IsNPC() && (this->GetLevel() >= 13))
		return true;
	
	// Kaiyodo - Check the classes that can DW, and make sure we're not using a 2 hander
	bool dh2h = false;
	switch(GetClass()) // Lets make sure they are the right level! -image
	{
	case WARRIOR:
	case BERSERKER:
	case ROGUE:
	case WARRIORGM:
	case BERSERKERGM:
	case ROGUEGM:
		{
			if(GetLevel() < 13)
				return false;
			break;
		}
	case BARD:
	case RANGER:
	case BARDGM:
	case RANGERGM:
		{
			if(GetLevel() < 17)
				return false;
			break;
		}
	case BEASTLORD:
	case BEASTLORDGM:
		{
			if(GetLevel() < 17)
				return false;
			dh2h = true;
			break;
		}
	case MONK:
	case MONKGM:
		{
			dh2h = true;
			break;
		}
	default:
		{
			return false;
		}
	}
	
	if (IsClient()) {
		const ItemInst* inst = CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
		// 2HS, 2HB, or 2HP
		if (inst && inst->IsType(ItemClassCommon)) {
			const Item_Struct* item = inst->GetItem();
			if ((item->ItemType == ItemType2HB) || (item->ItemType == ItemType2HS) || (item->ItemType == ItemType2HPierce))
				return false;
		} else {
			//No weapon in hand... using hand-to-hand...
			//only monks and beastlords? can dual weild their fists.
			return(dh2h);
		}
		
		return (this->CastToClient()->HasSkill(DUAL_WIELD));	// No skill = no chance
	}
	else
		return true;	//if we get here, we are the right class
						//and are at the right level, and are NPC
}

bool Mob::CanThisClassDoubleAttack(void) const
{
    // All npcs over level 26 can double attack
    if (IsNPC() && GetLevel() >= 26)
        return true;
	
	if(GetAA(aaBestialFrenzy) || GetAA(aaHarmoniousAttack) || 
		GetAA(aaKnightsAdvantage) || GetAA(aaFerocity)){
			return true;
		}

	// Kaiyodo - Check the classes that can DA
	switch(GetClass()) // Lets make sure they are the right level! -image
	{
	case BERSERKER:
	case BERSERKERGM:	
	case WARRIOR:
	case WARRIORGM:
	case MONK:
	case MONKGM:
		{
			if(GetLevel() < 15)
				return false;
			break;
		}
	case ROGUE:
	case ROGUEGM:
		{
			if(GetLevel() < 16)
				return false;
			break;
		}
	case RANGER:
	case RANGERGM:
	case PALADIN:
	case PALADINGM:
	case SHADOWKNIGHT:
	case SHADOWKNIGHTGM:
		{
			if(GetLevel() < 20)
				return false;
			break;
		}
	default:
		{
			return false;
		}
	}

	if (IsClient())
		return(CastToClient()->HasSkill(DOUBLE_ATTACK));	// No skill = no chance
	else
		return true;	//if we get here, we are the right class
						//and are at the right level, and are NPC
}

bool Mob::IsWarriorClass(void) const
{
	switch(GetClass())
	{
	case WARRIOR:
	case WARRIORGM:
	case ROGUE:
	case ROGUEGM:
	case MONK:
	case MONKGM:
	case PALADIN:
	case PALADINGM:
	case SHADOWKNIGHT:
	case SHADOWKNIGHTGM:
	case RANGER:
	case RANGERGM:
	case BEASTLORD:
	case BEASTLORDGM:
	case BERSERKER:
	case BERSERKERGM:
	case BARD:
	case BARDGM:
		{
			return true;
		}
	default:
		{
			return false;
		}
	}

}

bool Mob::CanThisClassParry(void) const
{
	// Trumpcard
	switch(GetClass()) // Lets make sure they are the right level! -image
	{
	case WARRIOR:
	case WARRIORGM:
		{
		if(GetLevel() < 10)
			return false;
		break;
		}
	case ROGUE:
	case BERSERKER:
	case ROGUEGM:
	case BERSERKERGM:
		{
		if(GetLevel() < 12)
			return false;
		break;
		}
	case BARD:
	case BARDGM:
		{
		if(GetLevel() < 53)
			return false;
		break;
		}
	case RANGER:
	case RANGERGM:
		{
		if(GetLevel() < 18)
			return false;
		break;
		}
	case SHADOWKNIGHT:
	case PALADIN:
	case SHADOWKNIGHTGM:
	case PALADINGM:
		{
		if(GetLevel() < 17)
			return false;
		break;
		}
	default:
		{
			return false;
		}
	}

	if (this->IsClient())
		return(this->CastToClient()->HasSkill(PARRY));	// No skill = no chance
	else
		return true;
}

bool Mob::CanThisClassDodge(void) const
{
	// Trumpcard
	switch(GetClass()) // Lets make sure they are the right level! -image
	{
	case WARRIOR:
	case WARRIORGM:
		{
			if(GetLevel() < 6)
				return false;
			break;
		}
	case MONK:
	case MONKGM:
		{
			break;
		}
	case ROGUE:
	case ROGUEGM:
		{
			if(GetLevel() < 4)
				return false;
			break;
		}
	case RANGER:
	case RANGERGM:
		{
			if(GetLevel() < 8)
				return false;
			break;
		}
	case BARD:
	case BEASTLORD:
	case SHADOWKNIGHT:
	case BERSERKER:
	case PALADIN:
	case BARDGM:
	case BEASTLORDGM:
	case SHADOWKNIGHTGM:
	case BERSERKERGM:
	case PALADINGM:
		{
			if(GetLevel() < 10)
				return false;
			break;
		}
	case CLERIC:
	case SHAMAN:
	case DRUID:
	case CLERICGM:
	case SHAMANGM:
	case DRUIDGM:
		{
			if( GetLevel() < 15 )
				return false;
			break;
		}
	case NECROMANCER:
	case ENCHANTER:
	case WIZARD:
	case MAGICIAN:
	case NECROMANCERGM:
	case ENCHANTERGM:
	case WIZARDGM:
	case MAGICIANGM:
		{
			if( GetLevel() < 22 )
				return false;
			break;
		}
	default:
		{
			return false;
		}
	}
	
	if (this->IsClient())
		return(this->CastToClient()->HasSkill(DODGE));	// No skill = no chance
	else
		return true;
}

bool Mob::CanThisClassRiposte(void) const //Could just check if they have the skill?
{
	// Trumpcard
	switch(GetClass()) // Lets make sure they are the right level! -image
	{
	case WARRIOR:
	case WARRIORGM:
		{
			if(GetLevel() < 25)
				return false;
			break;
		}
	case ROGUE:
	case RANGER:
	case SHADOWKNIGHT:
	case PALADIN:
	case BERSERKER:
	case ROGUEGM:
	case RANGERGM:
	case SHADOWKNIGHTGM:
	case PALADINGM:
	case BERSERKERGM:
		{
			if(GetLevel() < 30)
				return false;
			break;
		}
	case MONK:
	case MONKGM:
		{
			if(GetLevel() < 35)
				return false;
			break;
		}
	case BEASTLORD:
	case BEASTLORDGM:
		{
			if(GetLevel() < 40)
				return false;
			break;
		}
	case BARD:
	case BARDGM:
		{
			if(GetLevel() < 58)
				return false;
			break;
		}
	default:
		{
			return false;
		}
	}
	
	if (this->IsClient())
		return(this->CastToClient()->HasSkill(RIPOSTE));	// No skill = no chance
	else
		return true;
}

bool Mob::CanThisClassBlock(void) const
{
	switch(GetClass())
	{
	case BEASTLORDGM:
	case BEASTLORD:
		{
		if(GetLevel() < 25)
			return false;
		break;
		}
	case MONKGM:
	case MONK:
		{
		if(GetLevel() < 12)
			return false;
		break;
		}
	default:
		{
			return false;
		}
	}

	if (this->IsClient())
		return(this->CastToClient()->HasSkill(BLOCKSKILL));	// No skill = no chance
	else
		return true;
}

float Mob::Dist(const Mob &other) const {
	_ZP(Mob_Dist);
	float xDiff = other.x_pos - x_pos;
	float yDiff = other.y_pos - y_pos;
	float zDiff = other.z_pos - z_pos;

	return sqrtf( (xDiff * xDiff) 
	           + (yDiff * yDiff) 
		       + (zDiff * zDiff) );
}

float Mob::DistNoZ(const Mob &other) const {
	_ZP(Mob_DistNoZ);
	float xDiff = other.x_pos - x_pos;
	float yDiff = other.y_pos - y_pos;
	
	return sqrtf( (xDiff * xDiff) 
		       + (yDiff * yDiff) );
}

float Mob::DistNoRoot(const Mob &other) const {
	_ZP(Mob_DistNoRoot);
	float xDiff = other.x_pos - x_pos;
	float yDiff = other.y_pos - y_pos;
	float zDiff = other.z_pos - z_pos;

	return ( (xDiff * xDiff)  
	       + (yDiff * yDiff)  
	       + (zDiff * zDiff) );
}

float Mob::DistNoRoot(float x, float y, float z) const {
	_ZP(Mob_DistNoRoot);
	float xDiff = x - x_pos;
	float yDiff = y - y_pos;
	float zDiff = z - z_pos;

	return ( (xDiff * xDiff)  
	       + (yDiff * yDiff)  
	       + (zDiff * zDiff) );
}

float Mob::DistNoRootNoZ(const Mob &other) const {
	_ZP(Mob_DistNoRootNoZ);
	float xDiff = other.x_pos - x_pos;
	float yDiff = other.y_pos - y_pos;

	return ( (xDiff * xDiff) + (yDiff * yDiff) );
}

bool Mob::HateSummon() {
    // check if mob has ability to summon
    // we need to be hurt and level 51+ or ability checked to continue
// Sandy - fix so not automatic summon
	if (GetHPRatio() >= 95 || SpecAttacks[SPECATK_SUMMON] == false)
        return false;

    // now validate the timer
    if (!SpecAttackTimers[SPECATK_SUMMON])
    {
        SpecAttackTimers[SPECATK_SUMMON] = new Timer(6000);
        SpecAttackTimers[SPECATK_SUMMON]->Start();
    }

    // now check the timer
    if (!SpecAttackTimers[SPECATK_SUMMON]->Check())
        return false;

    // get summon target
    SetTarget(GetHateTop());
    if(target)
    {
		if (target->IsClient())
			target->CastToClient()->Message(15,"You have been summoned!");
		entity_list.MessageClose(this, true, 500, 10, "%s says,'You will not evade me, %s!' ", GetCleanName(), GetHateTop()->GetCleanName() );

		// RangerDown - GMMove doesn't seem to be working well with players, so use MovePC for them, GMMove for NPC's
		if (target->IsClient()) {
			target->CastToClient()->cheat_timer.Start(3500,false); //Lieka:  Prevent Mob Summons from tripping hack detector.
			target->CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), x_pos, y_pos, z_pos, target->GetHeading(), 0, SummonPC);
		}
		else

#ifdef EQBOTS

             {
                 if(target->IsBot()) {
                     target->Warp(x_pos, y_pos, z_pos);
                 }
                 else {
                     GetHateTop()->GMMove(x_pos, y_pos, z_pos, target->GetHeading());
                 }
        }

#else //EQBOTS

			GetHateTop()->GMMove(x_pos, y_pos, z_pos, target->GetHeading());

#endif //EQBOTS

        return true;
	}
	return false;
}

void Mob::FaceTarget(Mob* MobToFace) {
	Mob* facemob = MobToFace;
	if(!facemob) {
		if(!GetTarget()) {
			return;
		}
		else {
			facemob = GetTarget();
		}
	}

	float oldheading = GetHeading();
	float newheading = CalculateHeadingToTarget(facemob->GetX(), facemob->GetY());
	if(oldheading != newheading) {
		SetHeading(newheading);
		if(moving)
			SendPosUpdate();
		else
		{
			SendPosition();
		}
	}
}

bool Mob::RemoveFromHateList(Mob* mob) {
	SetRunAnimSpeed(0);
	bool bFound = false;
	if (this->IsEngaged())
	{
		bFound = hate_list.RemoveEnt(mob);	
		if  (hate_list.IsEmpty()){
			if (bFound)
				AI_Event_NoLongerEngaged();
			zone->DelAggroMob();
			//			cout << "Mobs currently Aggro: " << zone->MobsAggroCount() << endl; 
		}
	}
	if (GetTarget() == mob)
		SetTarget(hate_list.GetTop(this));
	return bFound;
}
void Mob::WipeHateList() {
	if (this->IsEngaged()) {
		AI_Event_NoLongerEngaged();
	}
	hate_list.Wipe();
}

int32 Mob::RandomTimer(int min,int max) {
    int r = 14000;
	if(min != 0 && max != 0 && min < max)
	{
	    r = (rand()  % (max - min)) + min;
	}
	return r;
}

int32 NPC::GetEquipment(int8 material_slot) const
{
	if(material_slot > 8)
		return 0;

	return equipment[material_slot];
}

void Mob::SendWearChange(int8 material_slot)
{
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_WearChange, sizeof(WearChange_Struct));
	WearChange_Struct* wc = (WearChange_Struct*)outapp->pBuffer;

	wc->spawn_id = GetID();
	wc->material = GetEquipmentMaterial(material_slot);
	wc->color.color = GetEquipmentColor(material_slot);
	wc->wear_slot_id = material_slot;

	entity_list.QueueClients(this, outapp);
	safe_delete(outapp);
}

void Mob::SendTextureWC(int8 slot, int16 texture)
{
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_WearChange, sizeof(WearChange_Struct));
	WearChange_Struct* wc = (WearChange_Struct*)outapp->pBuffer;

	wc->spawn_id = this->GetID();
	wc->material = texture;
	if (this->IsClient())
		wc->color.color = GetEquipmentColor(slot);
	else
		wc->color.color = this->GetArmorTint(slot);
	wc->wear_slot_id = slot;

	entity_list.QueueClients(this, outapp);
	safe_delete(outapp);
}

sint32 Mob::GetEquipmentMaterial(int8 material_slot) const
{
	const Item_Struct *item;
	
	item = database.GetItem(GetEquipment(material_slot));
	if(item != 0)
	{
		if	// for primary and secondary we need the model, not the material
		(
			material_slot == MATERIAL_PRIMARY ||
			material_slot == MATERIAL_SECONDARY
		)
		{
			if(strlen(item->IDFile) > 2)
				return atoi(&item->IDFile[2]);
			else	//may as well try this, since were going to 0 anyways
				return item->Material;
		}
		else
		{
			return item->Material;
		}
	}

	return 0;
}

uint32 Mob::GetEquipmentColor(int8 material_slot) const
{
	const Item_Struct *item;
	
	item = database.GetItem(GetEquipment(material_slot));
	if(item != 0)
	{
		return item->Color;
	}

	return 0;
}

//
// solar: works just like a printf
//
void Mob::Say(const char *format, ...)
{
	char buf[1000];
	va_list ap;
	
	va_start(ap, format);
	vsnprintf(buf, 1000, format, ap);
	va_end(ap);
	
	entity_list.MessageClose_StringID(this, false, 200, 10,
		GENERIC_SAY, GetCleanName(), buf);
}

//
// solar: this is like the above, but the first parameter is a string id
//
void Mob::Say_StringID(int32 string_id, const char *message3, const char *message4, const char *message5, const char *message6, const char *message7, const char *message8, const char *message9)
{
	char string_id_str[10];
	
	snprintf(string_id_str, 10, "%d", string_id);

	entity_list.MessageClose_StringID(this, false, 200, 10,
		GENERIC_STRINGID_SAY, GetCleanName(), string_id_str, message3, message4, message5,
		message6, message7, message8, message9
	);
}

void Mob::Shout(const char *format, ...)
{
	char buf[1000];
	va_list ap;
	
	va_start(ap, format);
	vsnprintf(buf, 1000, format, ap);
	va_end(ap);
	
	entity_list.Message_StringID(this, false, MT_Shout,
		GENERIC_SHOUT, GetCleanName(), buf);
}

void Mob::Emote(const char *format, ...)
{
	char buf[1000];
	va_list ap;
	
	va_start(ap, format);
	vsnprintf(buf, 1000, format, ap);
	va_end(ap);
	
	entity_list.MessageClose_StringID(this, false, 200, 10,
		GENERIC_EMOTE, GetCleanName(), buf);
}

void Mob::QuestJournalledSay(Client *QuestInitiator, const char *str)
{
        entity_list.QuestJournalledSayClose(this, QuestInitiator, 200, GetCleanName(), str);
}

const char *Mob::GetCleanName()
{
	if(!strlen(clean_name))
	{
		CleanMobName(GetName(), clean_name);
	}

	return clean_name;
}

// hp event 
void Mob::SetNextHPEvent( int hpevent ) 
{ 
	nexthpevent = hpevent;
}

void Mob::SetNextIncHPEvent( int inchpevent ) 
{ 
	nextinchpevent = inchpevent;
}
//warp for quest function,from sandy
void Mob::Warp( float x, float y, float z ) 
{ 
   x_pos = x; 
   y_pos = y; 
   z_pos = z; 

   Mob* target = GetTarget(); 
   if ( target ) { 
      FaceTarget( target ); 
   } 

   SendPosition(); 

}

bool Mob::DivineAura() const
{
	uint32 l;
	for (l = 0; l < BUFF_COUNT; l++)
	{
		if (buffs[l].spellid != SPELL_UNKNOWN)
		{
			for (int k = 0; k < EFFECT_COUNT; k++)
			{
				if (spells[buffs[l].spellid].effectid[k] == SE_DivineAura)
				{
					return true;
				}
			}
		}
	}
	return false;
}



/*bool Mob::SeeInvisibleUndead()
{
	if (IsNPC() && CastToNPC()->Undead() && CastToNPC()->immunities[0] == 0)
		return false;
	return true;
}

bool Mob::SeeInvisible()
{
	if (IsNPC() && CastToNPC()->immunities[0] > 0)
		return true;
	int l;
	for (l = 0; l < BUFF_COUNT; l++)
	{
		if (buffs[l].spellid != SPELL_UNKNOWN)
		{
			for (int k = 0; k < EFFECT_COUNT; k++)
			{
				if (spells[buffs[l].spellid].effectid[k] == SE_SeeInvis)
				{
					return true;
				}
			}
		}
	}
	return false;
}*/

sint16 Mob::GetResist(int8 type) const
{
	if (IsNPC())
	{
		if (type == 1)
			return MR + spellbonuses.MR + itembonuses.MR;
		else if (type == 2)
			return FR + spellbonuses.FR + itembonuses.FR;
		else if (type == 3)
			return CR + spellbonuses.CR + itembonuses.CR;
		else if (type == 4)
			return PR + spellbonuses.PR + itembonuses.PR;
		else if (type == 5)
			return DR + spellbonuses.DR + itembonuses.DR;
	}
	else if (IsClient())
	{
		if (type == 1)
			return CastToClient()->GetMR();
		else if (type == 2)
			return CastToClient()->GetFR();
		else if (type == 3)
			return CastToClient()->GetCR();
		else if (type == 4)
			return CastToClient()->GetPR();
		else if (type == 5)
			return CastToClient()->GetDR();
	}
	return 25;
}

int32 Mob::GetLevelHP(int8 tlevel)
{
	//cout<<"Tlevel: "<<(int)tlevel<<endl;
	int multiplier = 0;
	if (tlevel < 10)
	{
		multiplier = tlevel*20;
	}
	else if (tlevel < 20)
	{
		multiplier = tlevel*25;
	}
	else if (tlevel < 40)
	{
		multiplier = tlevel*tlevel*12*((tlevel*2+60)/100)/10;
	}
	else if (tlevel < 45)
	{
		multiplier = tlevel*tlevel*15*((tlevel*2+60)/100)/10;
	}
	else if (tlevel < 50)
	{
		multiplier = tlevel*tlevel*175*((tlevel*2+60)/100)/100;
	}
	else
	{
		multiplier = tlevel*tlevel*2*((tlevel*2+60)/100)*(1+((tlevel-50)*20/10));
	}
	return multiplier;
}

sint32 Mob::GetActSpellCasttime(int16 spell_id, sint32 casttime) {
	if (level >= 60 && casttime > 1000)
	{
		casttime = casttime / 2;
		if (casttime < 1000)
			casttime = 1000;
	} else if (level >= 50 && casttime > 1000) {
		sint32 cast_deduction = (casttime*(level - 49))/5;
		if (cast_deduction > casttime/2)
			casttime /= 2;
		else
			casttime -= cast_deduction;
	}
	return(casttime);
}
void Mob::ExecWeaponProc(uint16 spell_id, Mob *on) {
	// Trumpcard: Changed proc targets to look up based on the spells goodEffect flag.
	// This should work for the majority of weapons.
	if(spell_id == SPELL_UNKNOWN)
		return;
	if ( IsBeneficialSpell(spell_id) )
		SpellFinished(spell_id, this, 10, 0);
	else if(!(on->IsClient() && on->CastToClient()->dead))	//dont proc on dead clients
		SpellFinished(spell_id, on, 10, 0);
}

int32 Mob::GetZoneID() const {
	return(zone->GetZoneID());
}

int Mob::GetHaste() {
	int h = spellbonuses.haste + spellbonuses.hastetype2 + itembonuses.haste;
	int cap = 0;
	int level = GetLevel();

	if(level < 30) { // Rogean: Are these caps correct? Will use for now.
		cap = 50;
	} else if(level < 50) {
		cap = 74;
	} else if(level < 55) {
		cap = 84;
	} else if(level < 60) {
		cap = 94;
	} else {
		cap = 100;
	}
	

	if(h > cap) h = cap;

	h += spellbonuses.hastetype3;
	h += ExtraHaste;	//GM granted haste.

#ifdef EQBOTS

    if(IsBot() && (GetClass() == ROGUE || GetClass() == MONK )) // jadams: EQOffline, Not commented
        h += 15;

#endif //EQBOTS

	return(h); 
}

#ifdef EQBOTS

int32 Mob::GetBotLeader() {
	if((GetMaxHP()<0) || IsClient() || !IsBot())
		return 0;
	else {
		return database.GetBotLeader(GetNPCTypeID());
	}
}

void Mob::SendBotArcheryWearChange(int8 material_slot, uint32 material, uint32 color)
{
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_WearChange, sizeof(WearChange_Struct));
	WearChange_Struct* wc = (WearChange_Struct*)outapp->pBuffer;

	wc->spawn_id = GetID();
	wc->material = material;
	wc->color.color = color;
	wc->wear_slot_id = material_slot;

	entity_list.QueueClients(this, outapp);
	safe_delete(outapp);
}

sint16 Mob::CalcBotFocusEffect(botfocusType bottype, int16 focus_id, int16 spell_id) {

	const SPDat_Spell_Struct &focus_spell = spells[focus_id];
	const SPDat_Spell_Struct &spell = spells[spell_id];

	sint16 value = 0;
	int lvlModifier = 100;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		switch (focus_spell.effectid[i]) {
		case SE_Blank:
			break;

		//check limits

		//missing limits:
		//SE_LimitTarget

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
				lvlModifier -= spell.base2[i]*lvldiff;
				if(lvlModifier < 1)
					return 0;
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
				if(!IsEffectInSpell(spell_id,focus_spell.base[i])){ //we limit this effect, must have
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



		//handle effects

		case SE_ImprovedDamage:
			switch (focus_spell.max[i])
			{
				case 0:
					if (bottype == botfocusImprovedDamage && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 1:
					if (bottype == botfocusImprovedCritical && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 2:
					if (bottype == botfocusImprovedUndeadDamage && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				case 3:
					if (bottype == 10 && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
				default: //Resist stuff
					if (bottype == (botfocusType)focus_spell.max[i] && focus_spell.base[i] > value)
					{
						value = focus_spell.base[i];
					}
					break;
			}
			break;
		case SE_ImprovedHeal:
			if (bottype == botfocusImprovedHeal && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_IncreaseSpellHaste:
			if (bottype == botfocusSpellHaste && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_IncreaseSpellDuration:
			if (bottype == botfocusSpellDuration && BeneficialSpell(spell_id) && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_IncreaseRange:
			if (bottype == botfocusRange && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_ReduceReagentCost:
			if (bottype == botfocusReagentCost && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_ReduceManaCost:
			if (bottype == botfocusManaCost && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
		case SE_PetPowerIncrease:
			if (bottype == botfocusPetPower && focus_spell.base[i] > value)
			{
				value = focus_spell.base[i];
			}
			break;
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

sint16 Mob::GetBotFocusEffect(botfocusType bottype, int16 spell_id) {
	if (IsBardSong(spell_id))
		return 0;
	const Item_Struct* TempItem = 0;
	sint16 Total = 0;
	sint16 realTotal = 0;

	//item focus
	for(int x=0; x<=21; x++)
	{
		TempItem = database.GetItem(this->CastToNPC()->GetItemID(x));
		if (TempItem && TempItem->Focus.Effect > 0 && TempItem->Focus.Effect != SPELL_UNKNOWN) {
			Total = CalcBotFocusEffect(bottype, TempItem->Focus.Effect, spell_id);
			if(Total > realTotal) {
				realTotal = Total;
			}
		}
	}

	//Spell Focus
	sint16 Total2 = 0;
	sint16 realTotal2 = 0;

	for (int y = 0; y < BUFF_COUNT; y++) {
		int16 focusspellid = buffs[y].spellid;
		if (focusspellid == 0 || focusspellid >= SPDAT_RECORDS)
			continue;

		Total2 = CalcBotFocusEffect(bottype, focusspellid, spell_id);
		if(Total2 > realTotal2) {
			realTotal2 = Total2;
		}
	}

	int32 MagicianElementalPactAA = 0;
	if((GetClass() == MAGICIAN) && (GetLevel() >= 59)) {
		MagicianElementalPactAA = 1;
	}

	if(bottype == botfocusReagentCost && IsSummonPetSpell(spell_id) && MagicianElementalPactAA)
		return 100;

	if(bottype == botfocusReagentCost && (IsEffectInSpell(spell_id, SE_SummonItem) || IsSacrificeSpell(spell_id))){
		return 0;
	//Summon Spells that require reagents are typically imbue type spells, enchant metal, sacrifice and shouldn't be affected
	//by reagent conservation for obvious reasons.
	}

	return realTotal + realTotal2;
}

sint32 Mob::GetBotActSpellHealing(int16 spell_id, sint32 value) {

	sint32 modifier = 100;
	
	modifier += GetBotFocusEffect(botfocusImprovedHeal, spell_id);
						
	if(spells[spell_id].buffduration < 1) {
		uint8 botlevel = GetLevel();
		int8 botclass = GetClass();
		int chance = 0;

		if((botclass == BEASTLORD)||(botclass == CLERIC)||(botclass == DRUID)||(botclass == PALADIN)||(botclass == RANGER)||(botclass == SHAMAN)) {
			if(botlevel >= 57) { // Healing Adept AA
				modifier += 10;
			}
			else if(botlevel == 56) {
				modifier += 5;
			}
			else if(botlevel == 55) {
				modifier += 2;
			}

			if(botlevel >= 64) { // Advanced Healing Adept AA
				modifier += 9;
			}
			else if(botlevel == 63) {
				modifier += 6;
			}
			else if(botlevel == 62) {
				modifier += 3;
			}

			if(botlevel >= 57) { // Healing Gift AA
				chance = 10;
			}
			else if(botlevel == 56) {
				chance = 6;
			}
			else if(botlevel == 55) {
				chance = 3;
			}

			if(botlevel >= 64) { // Advanced Healing Gift AA
				chance += 6;
			}
			else if(botlevel == 63) {
				chance += 4;
			}
			else if(botlevel == 62) {
				chance += 2;
			}
		}

		if((botclass == NECROMANCER)||(botclass == SHADOWKNIGHT)) {
			if(spells[spell_id].targettype == ST_Tap) {
				if(botlevel >= 65) { // Theft of Life
					chance += 10;
				}
				else if(botlevel == 63) {
					chance += 5;
				}
				else if(botlevel == 61) {
					chance += 2;
				}

				if(botlevel >= 66) { // Advanced Theft of Life
					chance += 6;
				}
				else if(botlevel == 65) {
					chance += 3;
				}

				if(botlevel >= 69) { // Soul Thief
					chance += 6;
				}
				else if(botlevel == 68) {
					chance += 4;
				}
				else if(botlevel == 67) {
					chance += 2;
				}
			}
		}
		
		if(MakeRandomInt(1,100) < chance) {
			entity_list.MessageClose(this, false, 100, MT_SpellCrits, "%s performs an exceptional heal! (%d)", GetName(), ((value * modifier) / 50));		
			return (value * modifier) / 50;
		}
		else{
			return (value * modifier) / 100;
		}		
	}
					
	return (value * modifier) / 100;
}

sint32 Mob::GetBotActSpellDamage(int16 spell_id, sint32 value) {
	sint32 modifier = 100;
	int8 casterClass = GetClass();
	int8 casterLevel = GetLevel();

	//Dunno if this makes sense:
	if (spells[spell_id].resisttype > 0)
		modifier += 5;
	
	
	int tt = spells[spell_id].targettype;
	if (tt == ST_UndeadAE || tt == ST_Undead || tt == ST_Summoned) {
		//undead/summoned spells
		modifier += 10;
    } else {
    	//damage spells.
		modifier += 5;
	}

	//these spell IDs could be wrong
	if (spell_id == SPELL_LEECH_TOUCH) {	//leech touch
		if(casterLevel >= 65) { // Consumption of the Soul 3 AA
			value -= 1500;
		}
		else if(casterLevel >= 63) { // Consumption of the Soul 2 AA
			value -= 1000;
		}
		else if(casterLevel >= 61) { // Consumption of the Soul 1 AA
			value -= 500;
		}
	}
	if (spell_id == SPELL_IMP_HARM_TOUCH) {	//harm touch
		if(casterLevel >= 65) { // Unholy Touch 3 AA
			modifier += 75;
		}
		else if(casterLevel >= 63) { // Unholy Touch 2 AA
			modifier += 50;
		}
		else if(casterLevel >= 61) { // Unholy Touch 1 AA
			modifier += 25;
		}
	}
	
	//spell crits, dont make sense if cast on self.
	if(tt != ST_Self) {
		int chance = RuleI(Spells, BaseCritChance) + 1;
		sint32 ratio = RuleI(Spells, BaseCritRatio);

		//here's an idea instead of bloating code with unused cases there's this thing called:
		//case 'default'
		switch(casterClass)
		{
			case WIZARD:
			{
				if (casterLevel >= RuleI(Spells, WizCritLevel)) {
					chance += RuleI(Spells, WizCritChance);
					ratio += RuleI(Spells, WizCritRatio);
				}
				break;
			}

			default: 
				break;
		}
		
		if((casterClass == MONK) || (casterClass == ROGUE) || (casterClass == WARRIOR) || (casterClass == BERSERKER)) {
			if(casterLevel >= 65) { // Ingenuity 3 AA
				ratio += 100;
				chance += 3;
			}
			else if(casterLevel >= 63) { // Ingenuity 2 AA
				ratio += 75;
				chance += 2;
			}
			else if(casterLevel >= 61) { // Ingenuity 1 AA
				ratio += 50;
				chance += 1;
			}
		}

		if((casterClass != WARRIOR) && (casterClass != ROGUE) && (casterClass != MONK) && (casterClass != BERSERKER)) {
			if(casterLevel >= 66) { // Advanced Fury of Magic 2 AA
				chance += 20;
			}
			else if(casterLevel >= 65) { // Fury of Magic 3 AA or Advanced Fury of Magic 1 AA
				chance += 18;
			}
			else if(casterLevel >= 63) { // Fury of Magic 2 AA
				chance += 16;
			}
			else if(casterLevel >= 61) { // Fury of Magic 1 AA or Spell Casting Fury Mastery 3 AA
				chance += 14;
			}
			else if(casterLevel >= 60) { // Spell Casting Fury Mastery 2 AA
				chance += 12;
			}
			else if(casterLevel >= 59) { // Spell Casting Fury Mastery 1 AA
				chance += 10;
			}
			else if(casterLevel >= 57) { // Spell Casting Fury 3 AA
				chance += 7;
			}
			else if(casterLevel >= 56) { // Spell Casting Fury 2 AA
				chance += 4;
			}
			else if(casterLevel >= 55) { // Spell Casting Fury 1 AA
				chance += 2;
			}
		}

		if(tt == ST_Tap) {
			
			if(spells[spell_id].classes[SHADOWKNIGHT-1] >= 254 && spell_id != SPELL_LEECH_TOUCH) {
				if(ratio < 100)	//chance increase and ratio are made up, not confirmed
					ratio = 100;

				if(casterClass == SHADOWKNIGHT) {
					if(casterLevel >= 61) { // Soul Abrasion 3 AA
						modifier += 300;
					}
					else if(casterLevel >= 60) { // Soul Abrasion 2 AA
						modifier += 200;
					}
					else if(casterLevel >= 59) { // Soul Abrasion1 AA
						modifier += 100;
					}
				}
			}
		}

		//crit damage modifiers
		if (casterClass == WIZARD) { //wizards get an additional bonus
			if(casterLevel >= 70) { // Destructive Fury 3 AA
				ratio += 24;
			}
			else if(casterLevel >= 68) { // Destructive Fury 2 AA
				ratio += 16;
			}
			else if(casterLevel >= 66) { // Destructive Fury 1 AA
				ratio += 8;
			}
		}
		else {
			if(casterLevel >= 70) { // Destructive Fury 3 AA
				ratio += 16;
			}
			else if(casterLevel >= 68) { // Destructive Fury 2 AA
				ratio += 8;
			}
			else if(casterLevel >= 66) { // Destructive Fury 1 AA
				ratio += 4;
			}
		}
		
		if (chance > 0) {
			mlog(SPELLS__CRITS, "Attempting spell crit. Spell: %s (%d), Value: %d, Modifier: %d, Chance: %d, Ratio: %d", spells[spell_id].name, spell_id, value, modifier, chance, ratio);
			if(MakeRandomInt(1,100) <= chance) {
				modifier += modifier*ratio/100;
				mlog(SPELLS__CRITS, "Spell crit successful. Final damage modifier: %d, Final Damage: %d", modifier, (value * modifier) / 100);
				entity_list.MessageClose(this, false, 100, MT_SpellCrits, "%s delivers a critical blast! (%d)", GetName(), ((-value * modifier) / 100));	
			} else 
				mlog(SPELLS__CRITS, "Spell crit failed. Final Damage Modifier: %d, Final Damage: %d", modifier, (value * modifier) / 100);
		}
	}
	
	return (value * modifier) / 100;
}

void Mob::BotMeditate(bool isSitting) {

	if(isSitting) {
		// If the bot is a caster has less than 99% mana while its not engaged, he needs to sit to meditate
		if(GetManaRatio() < 99.0f) {
			if(mana_timer.Check(true)) {
				SetAppearance(eaSitting, false);
				if(!((int)GetManaRatio() % 24)) {
					Say("Medding for Mana. I have %3.1f%% of %d mana. It is: %d", GetManaRatio(), GetMaxMana(), GetMana());
				}
				int32 level = GetLevel();
				int32 regen = (((GetSkill(MEDITATE)/10)+(level-(level/4)))/4)+4;
				spellbonuses.ManaRegen = 0;
				for(int j=0; j<BUFF_COUNT; j++) {
					if(buffs[j].spellid != 65535) {
						const SPDat_Spell_Struct &spell = spells[buffs[j].spellid];
						for(int i=0; i<EFFECT_COUNT; i++) {
							if(IsBlankSpellEffect(buffs[j].spellid, i))
								continue;
							int effect = spell.effectid[i];
							switch(effect) {
								case SE_CurrentMana:
									spellbonuses.ManaRegen += CalcSpellEffectValue(buffs[j].spellid, i, buffs[j].casterlevel);
									break;
							}
						}
					}
				}
				regen += (spellbonuses.ManaRegen + itembonuses.ManaRegen);
				if(level >= 55) {
					regen += 1;//GetAA(aaMentalClarity);
				}
				if(level >= 56) {
					regen += 1;//GetAA(aaMentalClarity);
				}
				if(level >= 57) {
					regen += 1;//GetAA(aaMentalClarity);
				}
				if(level >= 71) {
					regen += 1;//GetAA(aaBodyAndMindRejuvenation);
				}
				if(level >= 72) {
					regen += 1;//GetAA(aaBodyAndMindRejuvenation);
				}
				if(level >= 73) {
					regen += 1;//GetAA(aaBodyAndMindRejuvenation);
				}
				if(level >= 74) {
					regen += 1;//GetAA(aaBodyAndMindRejuvenation);
				}
				if(level >= 75) {
					regen += 1;//GetAA(aaBodyAndMindRejuvenation);
				}
				regen = (regen * RuleI(Character, ManaRegenMultiplier)) / 100;

				float mana_regen_rate = RuleR(EQOffline, BotManaRegen);
				if(mana_regen_rate < 1.0f)
					mana_regen_rate = 1.0f;

				regen = regen / mana_regen_rate;

				SetMana(GetMana() + regen);
			}
		}
		else {
			SetAppearance(eaStanding, false);
		}
	}
	else {
		// Let's check our mana in fights..
		if(mana_timer.Check(true)) {
			if((!((int)GetManaRatio() % 12)) && ((int)GetManaRatio() < 10)) {
				Say("Medding for Mana. I have %3.1f%% of %d mana. It is: %d", GetManaRatio(), GetMaxMana(), GetMana());
			}
			int32 level = GetLevel();
			spellbonuses.ManaRegen = 0;
			for(int j=0; j<BUFF_COUNT; j++) {
				if(buffs[j].spellid != 65535) {
					const SPDat_Spell_Struct &spell = spells[buffs[j].spellid];
					for(int i=0; i<EFFECT_COUNT; i++) {
						if(IsBlankSpellEffect(buffs[j].spellid, i))
							continue;
						int effect = spell.effectid[i];
						switch(effect) {
							case SE_CurrentMana:
								spellbonuses.ManaRegen += CalcSpellEffectValue(buffs[j].spellid, i, buffs[j].casterlevel);
								break;
						}
					}
				}
			}
			int32 regen = 2 + spellbonuses.ManaRegen + itembonuses.ManaRegen + (level/5);
			if(level >= 55) {
				regen += 1;//GetAA(aaMentalClarity);
			}
			if(level >= 56) {
				regen += 1;//GetAA(aaMentalClarity);
			}
			if(level >= 57) {
				regen += 1;//GetAA(aaMentalClarity);
			}
			if(level >= 71) {
				regen += 1;//GetAA(aaBodyAndMindRejuvenation);
			}
			if(level >= 72) {
				regen += 1;//GetAA(aaBodyAndMindRejuvenation);
			}
			if(level >= 73) {
				regen += 1;//GetAA(aaBodyAndMindRejuvenation);
			}
			if(level >= 74) {
				regen += 1;//GetAA(aaBodyAndMindRejuvenation);
			}
			if(level >= 75) {
				regen += 1;//GetAA(aaBodyAndMindRejuvenation);
			}
			regen = (regen * RuleI(Character, ManaRegenMultiplier)) / 100;

			float mana_regen_rate = RuleR(EQOffline, BotManaRegen);
			if(mana_regen_rate < 1.0f)
				mana_regen_rate = 1.0f;

			regen = regen / mana_regen_rate;

			SetMana(GetMana() + regen);
		}
	}
}

sint32 Mob::GetBotActSpellDuration(int16 spell_id, sint32 duration) {

	int increase = 100;
	increase += GetBotFocusEffect(botfocusSpellDuration, spell_id);

	if(GetLevel() >= 57) { // Spell Casting Reinforcement AA
		increase += 30;
	}
	else if(GetLevel() == 56) {
		increase += 15;
	}
	else if(GetLevel() == 55) {
		increase += 5;
	}

	if(GetLevel() >= 59) { // Spell Casting Reinforcement Mastery AA
		increase += 20;
	}
	
	return (duration * increase) / 100;
}

float Mob::GetBotActSpellRange(int16 spell_id, float range) {
	float extrange = 100;
	extrange += GetBotFocusEffect(botfocusRange, spell_id);
	return (range * extrange) / 100;
}

sint32 Mob::GetBotActSpellCost(int16 spell_id, sint32 cost) {
	sint32 Result = 0;

	if(GetClass() == WIZARD || GetClass() == ENCHANTER || GetClass() == MAGICIAN || GetClass() == NECROMANCER || GetClass() == DRUID || GetClass() == SHAMAN || GetClass() == CLERIC || GetClass() == BARD) {
		// This formula was derived from the following resource:
		// http://www.eqsummoners.com/eq1/specialization-library.html
		// WildcardX
		float PercentManaReduction = 0;
		float PercentOfMaxSpecializeSkill = 0;
		float MaxSpecilizationSkillAllowed = GetSkill(spells[spell_id].skill);
		float SpecializeSkill = GetSpecializeSkillValue(spell_id);
		int SuccessChance = MakeRandomInt(1, 100);
		
		if(MaxSpecilizationSkillAllowed > 0)
			PercentOfMaxSpecializeSkill = SpecializeSkill / MaxSpecilizationSkillAllowed;
		
		if(SuccessChance <= (PercentOfMaxSpecializeSkill * 100))
			PercentManaReduction = (SpecializeSkill * .053) - 5.65;

		if(GetLevel() >= 57) { // Spell Casting Mastery
			PercentManaReduction += 30;
		}
		else if(GetLevel() == 56) {
			PercentManaReduction += 15;
		}
		else if(GetLevel() == 55) {
			PercentManaReduction += 5;
		}
		
		PercentManaReduction += GetBotFocusEffect(botfocusManaCost, spell_id);
		cost -= (cost * (PercentManaReduction / 100));
	}

	if(cost < 0)
		cost = 0;

	Result = cost;

	return Result;
}

sint32 Mob::GetBotActSpellCasttime(int16 spell_id, sint32 casttime) {
	
	sint32 cast_reducer = 0;
	cast_reducer += GetBotFocusEffect(botfocusSpellHaste, spell_id);

	uint8 botlevel = GetLevel();
	int8 botclass = GetClass();

	if (botlevel >= 51 && casttime >= 3000 && !BeneficialSpell(spell_id) 
		&& (botclass == SHADOWKNIGHT || botclass == RANGER || botclass == PALADIN || botclass == BEASTLORD ))
		cast_reducer += (GetLevel()-50)*3;
	
	if((casttime >= 4000) && BeneficialSpell(spell_id) && (CalcBuffDuration(this, this, spell_id) > 0)) {
		if((botclass == ENCHANTER)||(botclass == WIZARD)||(botclass == NECROMANCER)||(botclass == MAGICIAN)||(botclass == SHADOWKNIGHT)) {
			if(botlevel >= 57) { // Spell Casting Deftness AA
				cast_reducer += 25;
			}
			else if(botlevel == 56) {
				cast_reducer += 10;
			}
			else if(botlevel == 55) {
				cast_reducer += 5;
			}
		}

		if((botclass == ENCHANTER)||(botclass == SHAMAN)) {
			if(botlevel >= 61) { // Quick Buff AA
				cast_reducer += 50;
			}
			else if(botlevel == 60) {
				cast_reducer += 25;
			}
			else if(botlevel == 59) {
				cast_reducer += 10;
			}
		}
	}

	if(IsSummonSpell(spell_id)) {
		if(botclass == MAGICIAN) {
			if(botlevel >= 61) { // Quick Summoning AA
				cast_reducer += 50;
			}
			else if(botlevel == 60) {
				cast_reducer += 25;
			}
			else if(botlevel == 59) {
				cast_reducer += 10;
			}
		}
	}

	if(IsEvacSpell(spell_id)) {
		if((botclass == DRUID)||(botclass == WIZARD)) {
			if(botlevel >= 61) { // Quick Evacuation AA
				cast_reducer += 50;
			}
			else if(botlevel == 60) {
				cast_reducer += 25;
			}
			else if(botlevel == 59) {
				cast_reducer += 10;
			}
		}
	}

	if(IsDamageSpell(spell_id) && spells[spell_id].cast_time >= 4000) {
		if((botclass == DRUID)||(botclass == WIZARD)) {
			if(botlevel >= 61) { // Quick Damage AA
				cast_reducer += 10;
			}
			else if(botlevel == 60) {
				cast_reducer += 5;
			}
			else if(botlevel == 59) {
				cast_reducer += 2;
			}
		}
	}
	if (cast_reducer > 50)
		cast_reducer = 50;	//is this just an arbitrary limit?
	
	casttime = (casttime*(100 - cast_reducer)/100);
	
	return casttime;
}

void Mob::CalcBotStats(bool showtext) {
	if(showtext) {
		BotOwner->Message(15, "Bot updating...");
	}
	// base stats
	int brace = GetBaseRace(); // Angelox
	int bclass = GetClass();
	int blevel = GetLevel();
	
	// Check Race/Class combos
	bool isComboAllowed = false;
	switch(brace) {
		case 1: // Human
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 3: // Paladin
				case 4: // Ranger
				case 5: // Shadowknight
				case 6: // Druid
				case 7: // Monk
				case 8: // Bard
				case 9: // Rogue
				case 11: // Necromancer
				case 12: // Wizard
				case 13: // Magician
				case 14: // Enchanter
					isComboAllowed = true;
					break;
			}
			break;
		case 2: // Barbarian
			switch(bclass) {
				case 1: // Warrior
				case 9: // Rogue
				case 10: // Shaman
				case 15: // Beastlord
				case 16: // Berserker
					isComboAllowed = true;
					break;
			}
			break;
		case 3: // Erudite
			switch(bclass) {
				case 2: // Cleric
				case 3: // Paladin
				case 5: // Shadowknight
				case 11: // Necromancer
				case 12: // Wizard
				case 13: // Magician
				case 14: // Enchanter
					isComboAllowed = true;
					break;
			}
			break;
		case 4: // Wood Elf
			switch(bclass) {
				case 1: // Warrior
				case 4: // Ranger
				case 6: // Druid
				case 8: // Bard
				case 9: // Rogue
					isComboAllowed = true;
					break;
			}
			break;
		case 5: // High Elf
			switch(bclass) {
				case 2: // Cleric
				case 3: // Paladin
				case 12: // Wizard
				case 13: // Magician
				case 14: // Enchanter
					isComboAllowed = true;
					break;
			}
			break;
		case 6: // Dark Elf
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 5: // Shadowknight
				case 9: // Rogue
				case 11: // Necromancer
				case 12: // Wizard
				case 13: // Magician
				case 14: // Enchanter
					isComboAllowed = true;
					break;
			}
			break;
		case 7: // Half Elf
			switch(bclass) {
				case 1: // Warrior
				case 3: // Paladin
				case 4: // Ranger
				case 6: // Druid
				case 8: // Bard
				case 9: // Rogue
					isComboAllowed = true;
					break;
			}
			break;
		case 8: // Dwarf
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 3: // Paladin
				case 9: // Rogue
				case 16: // Berserker
					isComboAllowed = true;
					break;
			}
			break;
		case 9: // Troll
			switch(bclass) {
				case 1: // Warrior
				case 5: // Shadowknight
				case 10: // Shaman
				case 15: // Beastlord
				case 16: // Berserker
					isComboAllowed = true;
					break;
			}
			break;
		case 10: // Ogre
			switch(bclass) {
				case 1: // Warrior
				case 5: // Shadowknight
				case 10: // Shaman
				case 15: // Beastlord
				case 16: // Berserker
					isComboAllowed = true;
					break;
			}
			break;
		case 11: // Halfling
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 3: // Paladin
				case 4: // Ranger
				case 6: // Druid
				case 9: // Rogue
					isComboAllowed = true;
					break;
			}
			break;
		case 12: // Gnome
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 3: // Paladin
				case 5: // Shadowknight
				case 9: // Rogue
				case 11: // Necromancer
				case 12: // Wizard
				case 13: // Magician
				case 14: // Enchanter
					isComboAllowed = true;
					break;
			}
			break;
		case 128: // Iksar
			switch(bclass) {
				case 1: // Warrior
				case 5: // Shadowknight
				case 7: // Monk
				case 10: // Shaman
				case 11: // Necromancer
				case 15: // Beastlord
					isComboAllowed = true;
					break;
			}
			break;
		case 130: // Vah Shir
			switch(bclass) {
				case 1: // Warrior
				case 8: // Bard
				case 9: // Rogue
				case 10: // Shaman
				case 15: // Beastlord
				case 16: // Berserker
					isComboAllowed = true;
					break;
			}
			break;
		case 330: // Froglok
			switch(bclass) {
				case 1: // Warrior
				case 2: // Cleric
				case 3: // Paladin
				case 5: // Shadowknight
				case 9: // Rogue
				case 10: // Shaman
				case 11: // Necromancer
				case 12: // Wizard
					isComboAllowed = true;
					break;
			}
			break;
	}
	if(!isComboAllowed) {
		this->BotOwner->Message(15, "A %s - %s bot was detected. Is this Race/Class combination allowed?.", GetRaceName(brace), GetEQClassName(bclass, blevel));
		this->BotOwner->Message(15, "Previous Bots Code releases did not check Race/Class combinations during create.");
		this->BotOwner->Message(15, "Unless you are experiencing heavy lag, you should delete and remake this bot.");
	}
	isComboAllowed = false;

	int spellid = 0;
	// base stats
	sint16 bstr = 75;
	sint16 bsta = 75;
	sint16 bdex = 75;
	sint16 bagi = 75;
	sint16 bwis = 75;
	sint16 bint = 75;
	sint16 bcha = 75;
	sint16 bATK = 5;
	sint16 bMR = 25;
	sint16 bFR = 25;
	sint16 bDR = 15;
	sint16 bPR = 15;
	sint16 bCR = 25;

	switch(bclass) {
		case 1: // Warrior
			bstr += 10;
			bsta += 20;
			bagi += 10;
			bdex += 10;
			bATK += 12;
			bMR += (blevel / 2 + 1);
			break;
		case 2: // Cleric
			spellid = 701;
			bstr += 5;
			bsta += 5;
			bagi += 10;
			bwis += 30;
			bATK += 8;
			break;
		case 3: // Paladin
			spellid = 708;
			bstr += 15;
			bsta += 5;
			bwis += 15;
			bcha += 10;
			bdex += 5;
			bATK += 17;
			bDR += 8;
			if(blevel > 50) {
				bDR += (blevel - 50);
			}
			break;
		case 4: // Ranger
			spellid = 710;
			bstr += 15;
			bsta += 10;
			bagi += 10;
			bwis += 15;
			bATK += 17;
			bFR += 4;
			if(blevel > 50) {
				bFR += (blevel - 50);
			}
			bCR += 4;
			if(blevel > 50) {
				bCR += (blevel - 50);
			}
			break;
		case 5: // Shadowknight
			spellid = 709;
			bstr += 10;
			bsta += 15;
			bint += 20;
			bcha += 5;
			bATK += 17;
			bPR += 4;
			if(blevel > 50) {
				bPR += (blevel - 50);
			}
			bDR += 4;
			if(blevel > 50) {
				bDR += (blevel - 50);
			}
			break;
		case 6: // Druid
			spellid = 707;
			bsta += 15;
			bwis += 35;
			bATK += 5;
			break;
		case 7: // Monk
			bstr += 5;
			bsta += 15;
			bagi += 15;
			bdex += 15;
			bATK += 17;
			break;
		case 8: // Bard
			spellid = 711;
			bstr += 15;
			bdex += 10;
			bcha += 15;
			bint += 10;
			bATK += 17;
			break;
		case 9: // Rogue
			bstr += 10;
			bsta += 20;
			bagi += 10;
			bdex += 10;
			bATK += 12;
			bPR += 8;
			if(blevel > 50) {
				bPR += (blevel - 50);
			}
			break;
		case 10: // Shaman
			spellid = 706;
			bsta += 10;
			bwis += 30;
			bcha += 10;
			bATK += 28;
			break;
		case 11: // Necromancer
			spellid = 703;
			bdex += 10;
			bagi += 10;
			bint += 30;
			bATK += 5;
			break;
		case 12: // Wizard
			spellid = 702;
			bsta += 20;
			bint += 30;
			bATK += 5;
			break;
		case 13: // Magician
			spellid = 704;
			bsta += 20;
			bint += 30;
			bATK += 5;
			break;
		case 14: // Enchanter
			spellid = 705;
			bint += 25;
			bcha += 25;
			bATK += 5;
			break;
		case 15: // Beastlord
			spellid = 712;
			bsta += 10;
			bagi += 10;
			bdex += 5;
			bwis += 20;
			bcha += 5;
			bATK += 31;
			break;
		case 16: // Berserker
			bstr += 10;
			bsta += 15;
			bdex += 15;
			bagi += 10;
			bATK += 25;
			break;
	}

	float bsize = 6.0;
	switch(brace) {
		case 1: // Humans have no race bonus
			break;
		case 2: // Barbarian
			bstr += 28;
			bsta += 20;
			bagi += 7;
			bdex -= 5;
			bwis -= 5;
			bint -= 10;
			bcha -= 20;
			bsize = 7;
			bCR += 10;
			break;
		case 3: // Erudite
			bstr -= 15;
			bsta -= 5;
			bagi -= 5;
			bdex -= 5;
			bwis += 8;
			bint += 32;
			bcha -= 5;
			bMR += 5;
			bDR -= 5;
			break;
		case 4: // Wood Elf
			bstr -= 10;
			bsta -= 10;
			bagi += 20;
			bdex += 5;
			bwis += 5;
			bsize = 5;
			break;
		case 5: // High Elf
			bstr -= 20;
			bsta -= 10;
			bagi += 10;
			bdex -= 5;
			bwis += 20;
			bint += 12;
			bcha += 5;
			break;
		case 6: // Dark Elf
			bstr -= 15;
			bsta -= 10;
			bagi += 15;
			bwis += 8;
			bint += 24;
			bcha -= 15;
			bsize = 5;
			break;
		case 7: // Half Elf
			bstr -= 5;
			bsta -= 5;
			bagi += 15;
			bdex += 10;
			bwis -= 15;
			bsize = 5.5;
			break;
		case 8: // Dwarf
			bstr += 15;
			bsta += 15;
			bagi -= 5;
			bdex += 15;
			bwis += 8;
			bint -= 15;
			bcha -= 30;
			bsize = 4;
			bMR -= 5;
			bPR += 5;
			break;
		case 9: // Troll
			bstr += 33;
			bsta += 34;
			bagi += 8;
			bwis -= 15;
			bint -= 23;
			bcha -= 35;
			bsize = 8;
			bFR -= 20;
			break;
		case 10: // Ogre
			bstr += 55;
			bsta += 77;
			bagi -= 5;
			bdex -= 5;
			bwis -= 8;
			bint -= 15;
			bcha -= 38;
			bsize = 9;
			break;
		case 11: // Halfling
			bstr -= 5;
			bagi += 20;
			bdex += 15;
			bwis += 5;
			bint -= 8;
			bcha -= 25;
			bsize = 3.5;
			bPR += 5;
			bDR += 5;
			break;
		case 12: // Gnome
			bstr -= 15;
			bsta -= 5;
			bagi += 10;
			bdex += 10;
			bwis -= 8;
			bint += 23;
			bcha -= 15;
			bsize = 3;
			break;
		case 128: // Iksar
			bstr -= 5;
			bsta -= 5;
			bagi += 15;
			bdex += 10;
			bwis += 5;
			bcha -= 20;
			bMR -= 5;
			bFR -= 5;
			break;
		case 130: // Vah Shir
			bstr += 15;
			bagi += 15;
			bdex -= 5;
			bwis -= 5;
			bint -= 10;
			bcha -= 10;
			bsize = 7;
			bMR -= 5;
			bFR -= 5;
			break;
		case 330: // Froglok
			bstr -= 5;
			bsta += 5;
			bagi += 25;
			bdex += 25;
			bcha -= 25;
			bsize = 5;
			bMR -= 5;
			bFR -= 5;
			break;
	}

	// General AA bonus'
	if(blevel >= 51 ) {
		bstr += 2;	// Innate Strength AA 1
		bsta += 2;	// Innate Stamina AA 1
		bagi += 2;	// Innate Agility AA 1
		bdex += 2;	// Innate Dexterity AA 1
		bint += 2;	// Innate Intelligence AA 1
		bwis += 2;	// Innate Wisdom AA 1
		bcha += 2;	// Innate Charisma AA 1
		bFR += 2;	// Innate Fire Protection AA 1
		bCR += 2;	// Innate Cold Protection AA 1
		bMR += 2;	// Innate Magic Protection AA 1
		bPR += 2;	// Innate Poison Protection AA 1
		bDR += 2;	// Innate Disease AA 1
	}
	if(blevel >= 52 ) {
		bstr += 2;	// Innate Strength AA 2
		bsta += 2;	// Innate Stamina AA 2
		bagi += 2;	// Innate Agility AA 2
		bdex += 2;	// Innate Dexterity AA 2
		bint += 2;	// Innate Intelligence AA 2
		bwis += 2;	// Innate Wisdom AA 2
		bcha += 2;	// Innate Charisma AA 2
		bFR += 2;	// Innate Fire Protection AA 2
		bCR += 2;	// Innate Cold Protection AA 2
		bMR += 2;	// Innate Magic Protection AA 2
		bPR += 2;	// Innate Poison Protection AA 2
		bDR += 2;	// Innate Disease AA 2
	}
	if(blevel >= 53 ) {
		bstr += 2;	// Innate Strength AA 3
		bsta += 2;	// Innate Stamina AA 3
		bagi += 2;	// Innate Agility AA 3
		bdex += 2;	// Innate Dexterity AA 3
		bint += 2;	// Innate Intelligence AA 3
		bwis += 2;	// Innate Wisdom AA 3
		bcha += 2;	// Innate Charisma AA 3
		bFR += 2;	// Innate Fire Protection AA 3
		bCR += 2;	// Innate Cold Protection AA 3
		bMR += 2;	// Innate Magic Protection AA 3
		bPR += 2;	// Innate Poison Protection AA 3
		bDR += 2;	// Innate Disease AA 3
	}
	if(blevel >= 54 ) {
		bstr += 2;	// Innate Strength AA 4
		bsta += 2;	// Innate Stamina AA 4
		bagi += 2;	// Innate Agility AA 4
		bdex += 2;	// Innate Dexterity AA 4
		bint += 2;	// Innate Intelligence AA 4
		bwis += 2;	// Innate Wisdom AA 4
		bcha += 2;	// Innate Charisma AA 4
		bFR += 2;	// Innate Fire Protection AA 4
		bCR += 2;	// Innate Cold Protection AA 4
		bMR += 2;	// Innate Magic Protection AA 4
		bPR += 2;	// Innate Poison Protection AA 4
		bDR += 2;	// Innate Disease AA 4
	}
	if(blevel >= 55 ) {
		bstr += 2;	// Innate Strength AA 5
		bsta += 2;	// Innate Stamina AA 5
		bagi += 2;	// Innate Agility AA 5
		bdex += 2;	// Innate Dexterity AA 5
		bint += 2;	// Innate Intelligence AA 5
		bwis += 2;	// Innate Wisdom AA 5
		bcha += 2;	// Innate Charisma AA 5
		bFR += 2;	// Innate Fire Protection AA 5
		bCR += 2;	// Innate Cold Protection AA 5
		bMR += 2;	// Innate Magic Protection AA 5
		bPR += 2;	// Innate Poison Protection AA 5
		bDR += 2;	// Innate Disease AA 5
	}
	if(blevel >= 61 ) {
		bstr += 2;	// Advanced Innate Strength AA 1
		bsta += 2;	// Advanced Innate Stamina AA 1
		bagi += 2;	// Advanced Innate Agility AA 1
		bdex += 2;	// Advanced Innate Dexterity AA 1
		bint += 2;	// Advanced Innate Intelligence AA 1
		bwis += 2;	// Advanced Innate Wisdom AA 1
		bcha += 2;	// Advanced Innate Charisma AA 1
		bFR += 2;	// Warding of Solusek AA 1
		bCR += 2;	// Blessing of E'ci AA 1
		bMR += 2;	// Marr's Protection AA 1
		bPR += 2;	// Shroud of the Faceless AA 1
		bDR += 2;	// Bertoxxulous' Gift AA 1
	}
	if(blevel >= 62 ) {
		bstr += 2;	// Advanced Innate Strength AA 2
		bsta += 2;	// Advanced Innate Stamina AA 2
		bagi += 2;	// Advanced Innate Agility AA 2
		bdex += 2;	// Advanced Innate Dexterity AA 2
		bint += 2;	// Advanced Innate Intelligence AA 2
		bwis += 2;	// Advanced Innate Wisdom AA 2
		bcha += 2;	// Advanced Innate Charisma AA 2
		bFR += 2;	// Warding of Solusek AA 2
		bCR += 2;	// Blessing of E'ci AA 2
		bMR += 2;	// Marr's Protection AA 2
		bPR += 2;	// Shroud of the Faceless AA 2
		bDR += 2;	// Bertoxxulous' Gift AA 2
	}
	if(blevel >= 63 ) {
		bstr += 2;	// Advanced Innate Strength AA 3
		bsta += 2;	// Advanced Innate Stamina AA 3
		bagi += 2;	// Advanced Innate Agility AA 3
		bdex += 2;	// Advanced Innate Dexterity AA 3
		bint += 2;	// Advanced Innate Intelligence AA 3
		bwis += 2;	// Advanced Innate Wisdom AA 3
		bcha += 2;	// Advanced Innate Charisma AA 3
		bFR += 2;	// Warding of Solusek AA 3
		bCR += 2;	// Blessing of E'ci AA 3
		bMR += 2;	// Marr's Protection AA 3
		bPR += 2;	// Shroud of the Faceless AA 3
		bDR += 2;	// Bertoxxulous' Gift AA 3
	}
	if(blevel >= 64 ) {
		bstr += 2;	// Advanced Innate Strength AA 4
		bsta += 2;	// Advanced Innate Stamina AA 4
		bagi += 2;	// Advanced Innate Agility AA 4
		bdex += 2;	// Advanced Innate Dexterity AA 4
		bint += 2;	// Advanced Innate Intelligence AA 4
		bwis += 2;	// Advanced Innate Wisdom AA 4
		bcha += 2;	// Advanced Innate Charisma AA 4
		bFR += 2;	// Warding of Solusek AA 4
		bCR += 2;	// Blessing of E'ci AA 4
		bMR += 2;	// Marr's Protection AA 4
		bPR += 2;	// Shroud of the Faceless AA 4
		bDR += 2;	// Bertoxxulous' Gift AA 4
	}
	if(blevel >= 65 ) {
		bstr += 2;	// Advanced Innate Strength AA 5
		bsta += 2;	// Advanced Innate Stamina AA 5
		bagi += 2;	// Advanced Innate Agility AA 5
		bdex += 2;	// Advanced Innate Dexterity AA 5
		bint += 2;	// Advanced Innate Intelligence AA 5
		bwis += 2;	// Advanced Innate Wisdom AA 5
		bcha += 2;	// Advanced Innate Charisma AA 5
		bFR += 2;	// Warding of Solusek AA 5
		bCR += 2;	// Blessing of E'ci AA 5
		bMR += 2;	// Marr's Protection AA 5
		bPR += 2;	// Shroud of the Faceless AA 5
		bDR += 2;	// Bertoxxulous' Gift AA 5
	}
	if(blevel >= 66 ) {
		bstr += 2;	// Advanced Innate Strength AA 6
		bsta += 2;	// Advanced Innate Stamina AA 6
		bagi += 2;	// Advanced Innate Agility AA 6
		bdex += 2;	// Advanced Innate Dexterity AA 6
		bint += 2;	// Advanced Innate Intelligence AA 6
		bwis += 2;	// Advanced Innate Wisdom AA 6
		bcha += 2;	// Advanced Innate Charisma AA 6
		bFR += 2;	// Warding of Solusek AA 6
		bCR += 2;	// Blessing of E'ci AA 6
		bMR += 2;	// Marr's Protection AA 6
		bPR += 2;	// Shroud of the Faceless AA 6
		bDR += 2;	// Bertoxxulous' Gift AA 6
	}
	if(blevel >= 67 ) {
		bstr += 2;	// Advanced Innate Strength AA 7
		bsta += 2;	// Advanced Innate Stamina AA 7
		bagi += 2;	// Advanced Innate Agility AA 7
		bdex += 2;	// Advanced Innate Dexterity AA 7
		bint += 2;	// Advanced Innate Intelligence AA 7
		bwis += 2;	// Advanced Innate Wisdom AA 7
		bcha += 2;	// Advanced Innate Charisma AA 7
		bFR += 2;	// Warding of Solusek AA 7
		bCR += 2;	// Blessing of E'ci AA 7
		bMR += 2;	// Marr's Protection AA 7
		bPR += 2;	// Shroud of the Faceless AA 7
		bDR += 2;	// Bertoxxulous' Gift AA 7
	}
	if(blevel >= 68 ) {
		bstr += 2;	// Advanced Innate Strength AA 8
		bsta += 2;	// Advanced Innate Stamina AA 8
		bagi += 2;	// Advanced Innate Agility AA 8
		bdex += 2;	// Advanced Innate Dexterity AA 8
		bint += 2;	// Advanced Innate Intelligence AA 8
		bwis += 2;	// Advanced Innate Wisdom AA 8
		bcha += 2;	// Advanced Innate Charisma AA 8
		bFR += 2;	// Warding of Solusek AA 8
		bCR += 2;	// Blessing of E'ci AA 8
		bMR += 2;	// Marr's Protection AA 8
		bPR += 2;	// Shroud of the Faceless AA 8
		bDR += 2;	// Bertoxxulous' Gift AA 8
	}
	if(blevel >= 69 ) {
		bstr += 2;	// Advanced Innate Strength AA 9
		bsta += 2;	// Advanced Innate Stamina AA 9
		bagi += 2;	// Advanced Innate Agility AA 9
		bdex += 2;	// Advanced Innate Dexterity AA 9
		bint += 2;	// Advanced Innate Intelligence AA 9
		bwis += 2;	// Advanced Innate Wisdom AA 9
		bcha += 2;	// Advanced Innate Charisma AA 9
		bFR += 2;	// Warding of Solusek AA 9
		bCR += 2;	// Blessing of E'ci AA 9
		bMR += 2;	// Marr's Protection AA 9
		bPR += 2;	// Shroud of the Faceless AA 9
		bDR += 2;	// Bertoxxulous' Gift AA 9
	}
	if(blevel >= 70 ) {
		bstr += 2;	// Advanced Innate Strength AA 10
		bsta += 2;	// Advanced Innate Stamina AA 10
		bagi += 2;	// Advanced Innate Agility AA 10
		bdex += 2;	// Advanced Innate Dexterity AA 10
		bint += 2;	// Advanced Innate Intelligence AA 10
		bwis += 2;	// Advanced Innate Wisdom AA 10
		bcha += 2;	// Advanced Innate Charisma AA 10
		bFR += 2;	// Warding of Solusek AA 10
		bCR += 2;	// Blessing of E'ci AA 10
		bMR += 2;	// Marr's Protection AA 10
		bPR += 2;	// Shroud of the Faceless AA 10
		bDR += 2;	// Bertoxxulous' Gift AA 10
	}


	// Base AC
	int bac = (blevel * 3) * 4;
	switch(bclass) {
		case WARRIOR:
		case SHADOWKNIGHT:
		case PALADIN: bac = bac*1.5;
	}

	// Calc Base Hit Points
	int16 lm = GetClassLevelFactor();
	int16 Post255;
	if((bsta-255)/2 > 0)
		Post255 = (bsta-255)/2;
	else
		Post255 = 0;
	sint32 bot_hp = (5)+(blevel*lm/10) + (((bsta-Post255)*blevel*lm/3000)) + ((Post255*blevel)*lm/6000);


	// Now, we need to calc the base mana.
	sint32 bot_mana = 0;
	sint32 WisInt = 0;
	sint32 MindLesserFactor, MindFactor;
	switch (GetCasterClass()) {
		case 'I':
			WisInt = bint;
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				bot_mana = (((5 * (MindFactor + 20)) / 2) * 3 * blevel / 40);
			}
			else {
				bot_mana = (((5 * (MindFactor + 200)) / 2) * 3 * blevel / 100);
			}
			bot_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		case 'W':
			WisInt = bwis;
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				bot_mana = (((5 * (MindFactor + 20)) / 2) * 3 * blevel / 40);
			}
			else {
				bot_mana = (((5 * (MindFactor + 200)) / 2) * 3 * blevel / 100);
			}
			bot_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		default:
			bot_mana = 0;
			break;
	}

	base_hp = bot_hp;
	max_mana = cur_mana = bot_mana;
	AC = bac;
	AGI = bagi;
	ATK = bATK;
	CHA = bcha;
	CR = bCR;
	DEX = bdex;
	DR = bDR;
	FR = bFR;
	INT = bint;
	MR = bMR;
	PR = bPR;
	STA = bsta;
	STR = bstr;
	WIS = bwis;

	// Special Attacks
	if(((bclass == MONK) || (bclass == WARRIOR) || (bclass == RANGER) || (bclass == BERSERKER))	&& (blevel >= 60)) {
		SpecAttacks[SPECATK_TRIPLE] = true;
	}

	if(showtext) {
		BotOwner->Message(15, "Base stats:");
		BotOwner->Message(15, "Level: %i HP: %i AC: %i Mana: %i STR: %i STA: %i DEX: %i AGI: %i INT: %i WIS: %i CHA: %i", blevel, base_hp, AC, max_mana, STR, STA, DEX, AGI, INT, WIS, CHA);
		BotOwner->Message(15, "Resists-- Magic: %i, Poison: %i, Fire: %i, Cold: %i, Disease: %i.",MR,PR,FR,CR,DR);
	}

	// Let's find the items in the bot inventory
	sint32 items_hp = 0;
	sint32 items_mana = 0;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;
	bool ret = false;			

	/* Update to the DB with base stats*/
	if(database.RunQuery(query, MakeAnyLenString(&query, "update npc_types set level=%i, hp=%i, size=%f, npc_spells_id=%i, runspeed=%f, MR=%i, CR=%i, DR=%i, FR=%i, PR=%i, AC=%i, STR=%i, STA=%i, DEX=%i, AGI=%i, _INT=%i, WIS=%i, CHA=%i, ATK=%i where id=%i",blevel,base_hp,bsize,spellid,2.501f,MR,CR,DR,FR,PR,AC,STR,STA,DEX,AGI,INT,WIS,CHA,ATK,GetNPCTypeID()), errbuf)) {
		safe_delete_array(query);
	}
	else {
		Say("My database update failed!!!");
	}

	query = 0;
	memset(&itembonuses, 0, sizeof(StatBonuses));
	for(int i=0; i<22; i++) {
		if(database.RunQuery(query, MakeAnyLenString(&query, "SELECT itemid FROM botinventory WHERE npctypeid=%i AND botslotid=%i", GetNPCTypeID(), i), errbuf, &result)) {
			safe_delete_array(query);
			if(mysql_num_rows(result) == 1) {
				row = mysql_fetch_row(result);
				int iteminslot = atoi(row[0]);
				mysql_free_result(result);

				if(iteminslot > 0) {
					const Item_Struct *itemtmp = database.GetItem(iteminslot);
					if(itemtmp->AC != 0)
						itembonuses.AC += itemtmp->AC;
					if(itemtmp->HP != 0)
						itembonuses.HP += itemtmp->HP;
					if(itemtmp->Mana != 0)
						itembonuses.Mana += itemtmp->Mana;
					if(itemtmp->Endur != 0)
						itembonuses.Endurance += itemtmp->Endur;
					if(itemtmp->AStr != 0)
						itembonuses.STR += itemtmp->AStr;
					if(itemtmp->ASta != 0)
						itembonuses.STA += itemtmp->ASta;
					if(itemtmp->ADex != 0)
						itembonuses.DEX += itemtmp->ADex;
					if(itemtmp->AAgi != 0)
						itembonuses.AGI += itemtmp->AAgi;
					if(itemtmp->AInt != 0)
						itembonuses.INT += itemtmp->AInt;
					if(itemtmp->AWis != 0)
						itembonuses.WIS += itemtmp->AWis;
					if(itemtmp->ACha != 0)
						itembonuses.CHA += itemtmp->ACha;
					if(itemtmp->MR != 0)
						itembonuses.MR += itemtmp->MR;
					if(itemtmp->FR != 0)
						itembonuses.FR += itemtmp->FR;
					if(itemtmp->CR != 0)
						itembonuses.CR += itemtmp->CR;
					if(itemtmp->PR != 0)
						itembonuses.PR += itemtmp->PR;
					if(itemtmp->DR != 0)
						itembonuses.DR += itemtmp->DR;
					if(itemtmp->Regen != 0)
						itembonuses.HPRegen += itemtmp->Regen;
					if(itemtmp->ManaRegen != 0)
						itembonuses.ManaRegen += itemtmp->ManaRegen;
					if(itemtmp->Attack != 0)
						itembonuses.ATK += itemtmp->Attack;
					if(itemtmp->DamageShield != 0)
						itembonuses.DamageShield += itemtmp->DamageShield;
					if(itemtmp->SpellShield != 0)
						itembonuses.SpellDamageShield += itemtmp->SpellShield;
					if(itemtmp->Shielding != 0)
						itembonuses.MeleeMitigation += itemtmp->Shielding;
					if(itemtmp->StunResist != 0)
						itembonuses.StunResist += itemtmp->StunResist;
					if(itemtmp->StrikeThrough != 0)
						itembonuses.StrikeThrough += itemtmp->StrikeThrough;
					if(itemtmp->Avoidance != 0)
						itembonuses.AvoidMeleeChance += itemtmp->Avoidance;
					if(itemtmp->Accuracy != 0)
						itembonuses.HitChance += itemtmp->Accuracy;
					if(itemtmp->CombatEffects != 0)
						itembonuses.ProcChance += itemtmp->CombatEffects;
					if ((itemtmp->Worn.Effect != 0) && (itemtmp->Worn.Type == ET_WornEffect)) { // latent effects
						ApplySpellsBonuses(itemtmp->Worn.Effect, itemtmp->Worn.Level, &itembonuses);
					}
				}
			}
		}
	}

	bMR += itembonuses.MR;
	bCR += itembonuses.CR;
	bDR += itembonuses.DR;
	bFR += itembonuses.FR;
	bPR += itembonuses.PR;
	bac += itembonuses.AC;
	bstr += itembonuses.STR;
	bsta += itembonuses.STA;
	bdex += itembonuses.DEX;
	bagi += itembonuses.AGI;
	bint += itembonuses.INT;
	bwis += itembonuses.WIS;
	bcha += itembonuses.CHA;
	bATK += itembonuses.ATK;

	bMR += spellbonuses.MR;
	bCR += spellbonuses.CR;
	bDR += spellbonuses.DR;
	bFR += spellbonuses.FR;
	bPR += spellbonuses.PR;
	bac += spellbonuses.AC;
	bstr += spellbonuses.STR;
	bsta += spellbonuses.STA;
	bdex += spellbonuses.DEX;
	bagi += spellbonuses.AGI;
	bint += spellbonuses.INT;
	bwis += spellbonuses.WIS;
	bcha += spellbonuses.CHA;
	bATK += spellbonuses.ATK;

	if((bsta-255)/2 > 0)
		Post255 = (bsta-255)/2;
	else
		Post255 = 0;
	bot_hp = (5)+(blevel*lm/10) + (((bsta-Post255)*blevel*lm/3000));
	bot_hp += itembonuses.HP;

	// Hitpoint AA's
	int32 nd = 10000;
	if(blevel >= 69) {
		nd += 1650;	// Planar Durablility AA 3
		if(bclass == WARRIOR) { // Sturdiness AA 5
			nd += 500;
		}
	}
	else if(blevel >= 68) {
		nd += 1650;	// Planar Durablility AA 3
		if(bclass == WARRIOR) { // Sturdiness AA 4
			nd += 400;
		}
	}
	else if(blevel >= 67) {
		nd += 1650;	// Planar Durablility AA 3
		if(bclass == WARRIOR) { // Sturdiness AA 3
			nd += 300;
		}
	}
	else if(blevel >= 66) {
		nd += 1650;	// Planar Durablility AA 3
		if(bclass == WARRIOR) { // Sturdiness AA 2
			nd += 200;
		}
	}
	else if(blevel >= 65) {
		nd += 1650;	// Planar Durablility AA 3
		if(bclass == WARRIOR) { // Sturdiness AA 1
			nd += 100;
		}
	}
	else if(blevel >= 63) {
		nd += 1500;	// Planar Durablility AA 2
	}
	else if(blevel >= 61) {
		nd += 1350;	// Planar Durablility AA 1
	}
	else if(blevel >= 59) {
		nd += 1200;	// Physical Enhancememt AA 1
	}
	else if(blevel >= 57) {
		nd += 1000;	// Natural Durablility AA 3
	}
	else if(blevel >= 56) {
		nd += 500;	// Natural Durablility AA 2
	}
	else if(blevel >= 55) {
		nd += 200;	// Natural Durablility AA 1
	}
	bot_hp = bot_hp * nd / 10000;
	bot_hp += spellbonuses.HP;
	max_hp = cur_hp = bot_hp;

	switch (GetCasterClass()) {
		case 'I':
			WisInt = bint;
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				bot_mana = (((5 * (MindFactor + 20)) / 2) * 3 * blevel / 40);
			}
			else {
				bot_mana = (((5 * (MindFactor + 200)) / 2) * 3 * blevel / 100);
			}
			bot_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		case 'W':
			WisInt = bwis;
			if((( WisInt - 199 ) / 2) > 0) {
				MindLesserFactor = ( WisInt - 199 ) / 2;
			}
			else {
				MindLesserFactor = 0;
			}
			MindFactor = WisInt - MindLesserFactor;
			if(WisInt > 100) {
				bot_mana = (((5 * (MindFactor + 20)) / 2) * 3 * blevel / 40);
			}
			else {
				bot_mana = (((5 * (MindFactor + 200)) / 2) * 3 * blevel / 100);
			}
			bot_mana += (itembonuses.Mana + spellbonuses.Mana);
			break;

		default:
			bot_mana = 0;
			break;
	}
	max_mana = cur_mana = bot_mana;
	CastToNPC()->AI_AddNPCSpells(spellid);

	if(showtext) {
		BotOwner->Message(15, "I'm updated.");
		BotOwner->Message(15, "Level: %i HP: %i AC: %i Mana: %i STR: %i STA: %i DEX: %i AGI: %i INT: %i WIS: %i CHA: %i", blevel, max_hp, bac, max_mana, bstr, bsta, bdex, bagi, bint, bwis, bcha);
		BotOwner->Message(15, "Resists-- Magic: %i, Poison: %i, Fire: %i, Cold: %i, Disease: %i.",bMR,bPR,bFR,bCR,bDR);
	}
}

#endif //EQBOTS

void Mob::SetTarget(Mob* mob) {
	if (target == mob) return;
	target = mob;
	entity_list.UpdateHoTT(this);
}

float Mob::FindGroundZ(float new_x, float new_y, float z_offset)
{
	float ret = -999999;
	if (zone->map != 0)
	{
		NodeRef pnode = zone->map->SeekNode( zone->map->GetRoot(), new_x, new_y );
		if (pnode != NODE_NONE)
		{
			VERTEX me;
			me.x = new_x;
			me.y = new_y;
			me.z = z_pos+z_offset;
			VERTEX hit;
			FACE *onhit;
			float best_z = zone->map->FindBestZ(pnode, me, &hit, &onhit);
			if (best_z != -999999)
			{
				ret = best_z;
			}
		}
	}
	return ret;
}

//helper function for npc AI; needs to be mob:: cause we need to be able to count buffs on other clients and npcs
int Mob::CountDispellableBuffs()
{
	int val = 0;
	for(int x = 0; x < BUFF_COUNT; x++)
	{
		if(!IsValidSpell(buffs[x].spellid))
			continue;

		if(buffs[x].diseasecounters || buffs[x].poisoncounters || buffs[x].cursecounters)
			continue;
		
		if(spells[buffs[x].spellid].goodEffect == 0)
			continue;

		if(buffs[x].spellid != SPELL_UNKNOWN &&	buffs[x].durationformula != DF_Permanent)
			val++;
	}
	return val;
}

// Returns the % that a mob is snared (as a positive value). -1 means not snared
int Mob::GetSnaredAmount()
{
	int worst_snare = -1;

	for (int i = 0; i < BUFF_COUNT; i++)
	{
		if (!IsValidSpell(buffs[i].spellid))
			continue;

		for(int j = 0; j < EFFECT_COUNT; j++)
		{
			if (spells[buffs[i].spellid].effectid[j] == SE_MovementSpeed)
			{
				int val = CalcSpellEffectValue_formula(spells[buffs[i].spellid].formula[j], spells[buffs[i].spellid].base[j], spells[buffs[i].spellid].max[j], buffs[i].casterlevel, buffs[i].spellid);
				//int effect = CalcSpellEffectValue(buffs[i].spellid, spells[buffs[i].spellid].effectid[j], buffs[i].casterlevel);
				if (val < 0 && abs(val) > worst_snare)
					worst_snare = abs(val);
			}
		}
	}

	return worst_snare;
}

void Mob::TriggerDefensiveProcs(Mob *on)
{
	if (this->HasDefensiveProcs()) {
		this->TryDefensiveProc(on);
	}

	return;
}

void Mob::SetDeltas(float dx, float dy, float dz, float dh) {
	delta_x = dx;
	delta_y = dy;
	delta_z = dz;
	delta_heading = dh;
}


bool Mob::HasBuffIcon(Mob *caster, Mob *target, int16 spell_id)
{
	if((caster->CalcBuffDuration(caster, target, spell_id)-1) > 0)
		return true;
	else
		return false;
}
