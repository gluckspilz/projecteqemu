/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2005  EQEMu Development Team (http://eqemulator.net)

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

/*

Assuming you want to add a new perl quest function named joe
that takes 1 integer argument....

1. Add the prototype to the quest manager:
questmgr.h: add (~line 50)
	void joe(int arg);

2. Define the actual function in questmgr.cpp:
void QuestManager::joe(int arg) {
	//... do something
}

3. Copy one of the XS routines in perlparser.cpp, preferably
 one with the same number of arguments as your routine. Rename
 as needed.
 Finally, add your routine to the list at the bottom of perlparser.cpp


4.
If you want it to work in old mode perl and .qst, edit parser.cpp
Parser::ExCommands (~line 777)
	else if (!strcmp(command,"joe")) {
		quest_manager.joe(atoi(arglist[0]));
	}

And then at then end of embparser.cpp, add:
"sub joe{push(@cmd_queue,{func=>'joe',args=>join(',',@_)});}"



*/

#include "../common/debug.h"
#include "entity.h"
#include "masterentity.h"

#include <sstream>
#include <iostream>
#include <list>
using namespace std;

#include "worldserver.h"
#include "net.h"
#include "../common/skills.h"
#include "../common/classes.h"
#include "../common/races.h"
#include "zonedb.h"
#include "../common/files.h"
#include "spdat.h"
#include "../common/packet_functions.h"
#include "spawn2.h"
#include "zone.h"
#include "parser.h"
#include "event_codes.h"
#include "guild_mgr.h"
#include "../common/rulesys.h"


extern Zone* zone;
extern WorldServer worldserver;
extern EntityList entity_list;

#ifdef EMBPERL
#include "embparser.h"
#endif

#include "questmgr.h"

//declare our global instance
QuestManager quest_manager;

QuestManager::QuestManager() {
	depop_npc = false;
	HaveProximitySays = false;
}

QuestManager::~QuestManager() {
}

void QuestManager::Process() {
	list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

	end = QTimerList.end();
	while (cur != end) {
		if (cur->Timer_.Enabled() && cur->Timer_.Check()) {
			//make sure the mob is still in zone.
			if(entity_list.IsMobInZone(cur->mob)){
#ifdef EMBPERL
				if(cur->mob->IsNPC()) {
					parse->Event(EVENT_TIMER, cur->mob->GetNPCTypeID(), cur->name.c_str(), cur->mob->CastToNPC(), NULL);
				}
				else{
					((PerlembParser*)parse)->Event(EVENT_TIMER, 0, cur->name.c_str(), (NPC*)NULL, cur->mob);
				}
#else
			if(entity_list.IsMobInZone(cur->mob)) {
				parse->Event(EVENT_TIMER, cur->mob->GetNPCTypeID(), cur->name.c_str(), cur->mob, NULL);
#endif

				//we MUST reset our iterator since the quest could have removed/added any
				//number of timers... worst case we have to check a bunch of timers twice
				cur = QTimerList.begin();
				end = QTimerList.end();	//dunno if this is needed, cant hurt...
			} else {
				tmp = cur;
				tmp++;
				QTimerList.erase(cur);
				cur = tmp;
			}
		} else
			cur++;
	}

	list<SignalTimer>::iterator curS, endS, tmpS;

	curS = STimerList.begin();
	endS = STimerList.end();
	while (curS != endS) {
		if(!curS->Timer_.Enabled()) {
			//remove the timer
			tmpS = curS;
			tmpS++;
			STimerList.erase(curS);
			curS = tmpS;
		} else if(curS->Timer_.Check()) {
			//disable the timer so it gets deleted.
			curS->Timer_.Disable();

			//signal the event...
			entity_list.SignalMobsByNPCID(curS->npc_id, curS->signal_id);

			//restart for the same reasons as above.
			curS = STimerList.begin();
			endS = STimerList.end();
		} else
			curS++;
	}
}

void QuestManager::StartQuest(Mob *_owner, Client *_initiator, ItemInst* _questitem) {
	quest_mutex.lock();
	owner = _owner;
	initiator = _initiator;
	questitem = _questitem;
	depop_npc = false;
}

void QuestManager::EndQuest() {
	quest_mutex.unlock();

	if(depop_npc && owner->IsNPC()) {
		//clear out any timers for them...
		list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

		end = QTimerList.end();
		while (cur != end) {
			if(cur->mob == owner) {
				tmp = cur;
				tmp++;
				QTimerList.erase(cur);
				cur = tmp;
			} else {
				cur++;
			}
		}

		owner->Depop();
		owner = NULL;	//just to be safe
	}
}


//quest perl functions
void QuestManager::echo(int colour, const char *str) {
   entity_list.MessageClose(initiator, false, 200, colour, str); 
}

void QuestManager::say(const char *str) {

       if(RuleB(NPC, EnableNPCQuestJournal) && initiator)
               owner->QuestJournalledSay(initiator, str);
       else
               owner->Say(str);
}

void QuestManager::say(const char *str, int8 language) {
	entity_list.ChannelMessage(owner, 8, language, str);
}

void QuestManager::me(const char *str) {
	if (!initiator)
		return;
	entity_list.MessageClose(initiator, false, 200, 10, str);
}

void QuestManager::summonitem(int32 itemid, uint8 charges) {
	if(!initiator)
		return;
	initiator->SummonItem(itemid, charges);
}

void QuestManager::write(const char *file, const char *str) {
	FILE * pFile;
	pFile = fopen (file, "a");
	if(!pFile)
		return;
	fprintf(pFile, "%s\n", str);
	fclose (pFile);
}

int16 QuestManager::spawn2(int npc_type, int grid, int unused, float x, float y, float z, float heading) {
	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{

		NPC* npc = new NPC(tmp, 0, x, y, z, heading);


		npc->AddLootTable();
		entity_list.AddNPC(npc,true,true);
		// Quag: Sleep in main thread? ICK!
		// Sleep(200);
		// Quag: check is irrelevent, it's impossible for npc to be 0 here
		// (we're in main thread, nothing else can possibly modify it)
//		if(npc != 0) {
			if(grid > 0)
			{
				npc->AssignWaypoints(grid);
			}
			npc->SendPosUpdate();
//		}
		return(npc->GetID());
	}
	return(0);
}

int16 QuestManager::unique_spawn(int npc_type, int grid, int unused, float x, float y, float z, float heading) {
	Mob *other = entity_list.GetMobByNpcTypeID(npc_type);
	if(other != NULL) {
		return(other->GetID());
	}


	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{

		NPC* npc = new NPC(tmp, 0, x, y, z, heading);


		npc->AddLootTable();
		entity_list.AddNPC(npc,true,true);
		// Quag: Sleep in main thread? ICK!
		// Sleep(200);
		// Quag: check is irrelevent, it's impossible for npc to be 0 here
		// (we're in main thread, nothing else can possibly modify it)
//		if(npc != 0) {
			if(grid > 0)
			{
				npc->AssignWaypoints(grid);
			}
			npc->SendPosUpdate();
//		}
		return(npc->GetID());
	}
	return(0);
}

void QuestManager::setstat(int stat, int value) {
	if (initiator)
		initiator->SetStats(stat, value);
}

void QuestManager::incstat(int stat, int value) { //old setstat command aza
	if (initiator)
		initiator->IncStats(stat, value);
}

void QuestManager::castspell(int spell_id, int target_id) {
	if (owner) {
		Mob *tgt = entity_list.GetMob(target_id);
		if(tgt != NULL)
			owner->SpellFinished(spell_id, tgt);
	}
}

void QuestManager::selfcast(int spell_id) {
	if (initiator)
		initiator->SpellFinished(spell_id, initiator,10,0);
}

void QuestManager::addloot(int item_id, int charges) {
	if(item_id != 0){
		if(owner->IsNPC())
			owner->CastToNPC()->AddItem(item_id, charges);
	}
}

void QuestManager::Zone(const char *zone_name) {
	if (initiator && initiator->IsClient())
	{
		ServerPacket* pack = new ServerPacket(ServerOP_ZoneToZoneRequest, sizeof(ZoneToZone_Struct));
		ZoneToZone_Struct* ztz = (ZoneToZone_Struct*) pack->pBuffer;
		ztz->response = 0;
		ztz->current_zone_id = zone->GetZoneID();
		ztz->current_instance_id = zone->GetInstanceID();
		ztz->requested_zone_id = database.GetZoneID(zone_name);
		ztz->admin = initiator->Admin();
		strcpy(ztz->name, initiator->GetName());
		ztz->guild_id = initiator->GuildID();
		ztz->ignorerestrictions = 3;
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
}

void QuestManager::settimer(const char *timer_name, int seconds) {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end;

	end = QTimerList.end();
	while (cur != end) {
		if (cur->mob == owner && cur->name == timer_name) {
			cur->mob = owner;
			cur->Timer_.Enable();
			cur->Timer_.Start(seconds * 1000, false);
			printf("Reseting: %s for %d seconds\n", cur->name.c_str(), seconds);
			return;
		}
		cur++;
	}

/*	timers * tmp = new timers;
	tmp->mob = npc;
	tmp->Timer_ = new Timer(seconds * 1000,0);
	tmp->Timer_->Start(seconds * 1000,false);
	tmp->name = timer_name;
	printf("Adding: %s for %d seconds\n", tmp->name.c_str(), seconds);
	QTimerList.push_back(tmp);*/
	QTimerList.push_back(QuestTimer(seconds * 1000, owner, timer_name));
}

void QuestManager::stoptimer(const char *timer_name) {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end;

	end = QTimerList.end();
	while (cur != end)
	{
		if(cur->mob == owner && cur->name == timer_name)
		{
			QTimerList.erase(cur);
			return;
		}
		cur++;
	}
}

void QuestManager::emote(const char *str) {
	owner->Emote(str);
}

void QuestManager::shout(const char *str) {
	owner->Shout(str);
}

void QuestManager::shout2(const char *str) {
	worldserver.SendEmoteMessage(0,0,0,13, "%s shouts, '%s'", owner->GetCleanName(), str);
}

void QuestManager::depop(int npc_type) {
	if(!owner->IsNPC())
		return;
	if (npc_type != 0){
		Mob * tmp = entity_list.GetMobByNpcTypeID(npc_type);
		if (tmp) {
			if(tmp != owner){
				tmp->CastToNPC()->Depop();
				entity_list.RemoveNPC(tmp->GetID());
				entity_list.RemoveMob(tmp->GetID());
			}
			else
				depop_npc = true;
		}
	}
	else {	//depop self
		depop_npc = true;
	}
}

void QuestManager::depopall(int npc_type) {
	if(owner->IsNPC() && npc_type > 0) {
		Mob* tmp = entity_list.GetMobByNpcTypeID(npc_type);
		while(tmp) {
			if(tmp != owner){
				tmp->CastToNPC()->Depop();
				entity_list.RemoveNPC(tmp->GetID());
				entity_list.RemoveMob(tmp->GetID());
			}
			else
				depop_npc = true;

			tmp = entity_list.GetMobByNpcTypeID(npc_type);
		}
	}
}

void QuestManager::depopzone(bool StartSpawnTimer) {
	if(zone)
		zone->Depop(StartSpawnTimer);
}

void QuestManager::repopzone() {
	if(zone)
		zone->Repop();
}

void QuestManager::settarget(const char *type, int target_id) {
	if(!owner->IsNPC())
		return;
	Mob* tmp = NULL;
	if (!strcasecmp(type,"npctype")) {
		tmp = entity_list.GetMobByNpcTypeID(target_id);
	}
	else if (!strcasecmp(type, "entity")) {
		tmp = entity_list.GetMob(target_id);
	}
	if(tmp != NULL) {
		owner->SetTarget(tmp);
	}
}

void QuestManager::follow(int entity_id) {
	if(!owner->IsNPC())
		return;
	owner->SetFollowID(entity_id);
}

void QuestManager::sfollow() {
	if(!owner->IsNPC())
		return;
	owner->SetFollowID(0);
}

/*
void QuestManager::cumflag() {
	npc->flag[50] = npc->flag[50] + 1;
}

void QuestManager::flagnpc(int32 flag_num, int8 flag_value) {
	if (flag_num >= (sizeof(npc->flag) / sizeof(npc->flag[0])))
		npc->flag[flag_num] = flag_value;
	else {
		// Quag: TODO: Script error here, handle it somehow?
	}
}

void QuestManager::flagcheck(int32 flag_to_check, int32 flag_to_set) {
	if (flag_to_check >= (sizeof(npc->flag) / sizeof(npc->flag[0])) || flag_to_set >= (sizeof(npc->flag) / sizeof(npc->flag[0]))) {
	if (initiator && initiator->flag[flag_to_check] != 0)
		initiator->flag[flag_to_set] = 0;
	} else {
	// Quag: TODO: Script error here, handle it somehow?
	}
// Quag: Orignal code, not sure how this is supposed to work
//				if (initiator->flag[atoi(arglist[0])] != 0)
//					if (initiator) initiator->flag[atoi(arg1)] = 0;

}*/

void QuestManager::changedeity(int diety_id) {
	//Cofruben:-Changes the deity.
	if (initiator && initiator->IsClient())
	{
		initiator->SetDeity(diety_id);
		initiator->Message(15,"Your Deity has been changed/set to: %i", diety_id);
		initiator->Save(1);
		initiator->Kick();
	}
	else
		initiator->Message(15,"Error changing Deity");

}

void QuestManager::exp(int amt) {
	if (initiator && initiator->IsClient())
		initiator->AddEXP(amt);
}

void QuestManager::level(int newlevel) {
	if (initiator && initiator->IsClient())
		initiator->SetLevel(newlevel, true);
}

void QuestManager::traindisc(int discipline_tome_item_id) {
	if (initiator && initiator->IsClient())
		initiator->TrainDiscipline(discipline_tome_item_id);
}

bool QuestManager::isdisctome(int item_id) {
//get the item info
	const Item_Struct *item = database.GetItem(item_id);
	if(item == NULL) {
		return(false);
	}

	if(item->ItemClass != ItemClassCommon || item->ItemType != ItemTypeSpell) {
		return(false);
	}

	//Need a way to determine the difference between a spell and a tome
	//so they cant turn in a spell and get it as a discipline
	//this is kinda a hack:
	if(!(
		item->Name[0] == 'T' &&
		item->Name[1] == 'o' &&
		item->Name[2] == 'm' &&
		item->Name[3] == 'e' &&
		item->Name[4] == ' '
		)) {
		return(false);
	}

	//we know for sure none of the int casters get disciplines
	uint32 cbit = 0;
	cbit |= 1 << (WIZARD-1);
	cbit |= 1 << (ENCHANTER-1);
	cbit |= 1 << (MAGICIAN-1);
	cbit |= 1 << (NECROMANCER-1);
	if(item->Classes & cbit) {
		return(false);
	}

	int32 spell_id = item->Scroll.Effect;
	if(!IsValidSpell(spell_id)) {
		return(false);
	}

	//we know for sure none of the int casters get disciplines
	const SPDat_Spell_Struct &spell = spells[spell_id];
	if(
		spell.classes[WIZARD - 1] != 255 &&
		spell.classes[ENCHANTER - 1] != 255 &&
		spell.classes[MAGICIAN - 1] != 255 &&
		spell.classes[NECROMANCER - 1] != 255
	) {
		return(false);
	}

	return(true);
}

void QuestManager::safemove() {
	if (initiator && initiator->IsClient())
		initiator->GoToSafeCoords(zone->GetZoneID(), 0);
}

void QuestManager::rain(int weather) {
	zone->zone_weather = weather;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Weather, 8);
	*((int32*) &outapp->pBuffer[4]) = (int32) weather; // Why not just use 0x01/2/3?
	entity_list.QueueClients(owner, outapp);
	safe_delete(outapp);
}

void QuestManager::snow(int weather) {
	zone->zone_weather = weather + 1;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Weather, 8);
	outapp->pBuffer[0] = 0x01;
	*((int32*) &outapp->pBuffer[4]) = (int32)weather;
	entity_list.QueueClients(initiator, outapp);
	safe_delete(outapp);
}

void QuestManager::surname(const char *name) {
	//Cofruben:-Changes the last name.
	if (initiator && initiator->IsClient())
	{
			initiator->ChangeLastName(name);
			initiator->Message(15,"Your surname has been changed/set to: %s", name);
	}
	else
		initiator->Message(15,"Error changing/setting surname");
}

void QuestManager::permaclass(int class_id) {
 	//Cofruben:-Makes the client the class specified
	initiator->SetBaseClass(class_id);
	initiator->Save(2);
	initiator->Kick();
}

void QuestManager::permarace(int race_id) {
 	//Cofruben:-Makes the client the race specified
	initiator->SetBaseRace(race_id);
	initiator->Save(2);
	initiator->Kick();
}

void QuestManager::permagender(int gender_id) {
 	//Cofruben:-Makes the client the gender specified
	initiator->SetBaseGender(gender_id);
	initiator->Save(2);
	initiator->Kick();
}

void QuestManager::scribespells(int spell_level) {
 	//Cofruben:-Scribe spells for user up to his actual level.
	int16 book_slot;
	int16 curspell;
	for(curspell = 0, book_slot = 0; curspell < SPDAT_RECORDS && book_slot < MAX_PP_SPELLBOOK; curspell++)
	{
	   if
	   (
		  spells[curspell].classes[WARRIOR] != 0 &&
		  spells[curspell].classes[initiator->GetPP().class_-1] <= spell_level &&
		  spells[curspell].skill != 52
	   )
	   {
		  initiator->ScribeSpell(curspell, book_slot++);
	   }
	}
}

void QuestManager::traindiscs(int disc_level) {
 	//Trevius: Train Disc for user up to their actual level.
	int16 book_slot;
	int16 curspell;
	for(curspell = 0, book_slot = 0; curspell < SPDAT_RECORDS && book_slot < MAX_PP_SPELLBOOK; curspell++)
	{
	   if
	   (
		  spells[curspell].classes[WARRIOR] != 0 &&
		  spells[curspell].classes[initiator->GetPP().class_-1] <= disc_level &&
		  spells[curspell].skill != 52
	   )
		{

			if(IsDiscipline(curspell)){
				for(int r = 0; r < MAX_PP_DISCIPLINES; r++) {
					if(initiator->GetPP().disciplines.values[r] == curspell) {
						initiator->Message(13, "You already know this discipline.");
						r = MAX_PP_DISCIPLINES;
					} else if(initiator->GetPP().disciplines.values[r] == 0) {
						initiator->GetPP().disciplines.values[r] = curspell;
						initiator->SendDisciplineUpdate();
						initiator->Message(0, "You have learned a new discipline!");
						r = MAX_PP_DISCIPLINES;
					}
				}
			}
	   	}
	}
}

void QuestManager::unscribespells() {
	//aza: unscribes all spells of the user
	initiator->UnscribeSpellAll();
	}

void QuestManager::givecash(int copper, int silver, int gold, int platinum) {
	if (initiator && initiator->IsClient())
	{
		initiator->AddMoneyToPP(copper, silver, gold, platinum,true);

		string tmp;
		if (platinum>0){
			tmp = "You receive ";
			tmp += itoa(platinum);
			tmp += " plat";
		}
		if (gold>0){
			if (tmp.length()==0){
				tmp = "You receive ";
			}
			else{
				tmp += ",";
			}
			tmp += itoa(gold);
			tmp += " gold";
		}
		if(silver>0){
			if (tmp.length()==0){
				tmp = "You receive ";
			}
			else{
				tmp += ",";
			}
			tmp += itoa(silver);
			tmp += " silver";
		}
		if(copper>0){
			if (tmp.length()==0){
				tmp = "You receive ";
			}
			else{
				tmp += ",";
			}
			tmp += itoa(copper);
			tmp += " copper";
		}
		tmp += " pieces.";
		if (initiator)
			initiator->Message(MT_OOC,tmp.c_str());
	}
}

void QuestManager::pvp(const char *mode) {
	if (!strcasecmp(mode,"on"))
		if (initiator)
			initiator->SetPVP(true);
	else
		if (initiator)
			initiator->SetPVP(false);
}

void QuestManager::movepc(int zone_id, float x, float y, float z, float heading) {
	if (initiator && initiator->IsClient())
		 initiator->MovePC(zone_id, x, y, z, heading);
}

void QuestManager::gmmove(float x, float y, float z) {
	if (initiator && initiator->IsClient())
		initiator->GMMove(x, y, z);
}

void QuestManager::movegrp(int zoneid, float x, float y, float z) {
#ifdef IPC
    if (initiator && initiator->IsClient()|| (initiator->IsNPC() && initiator->CastToNPC()->IsInteractive()) )
#else
    if (initiator && initiator->IsClient())
#endif
	{
		Group *g = entity_list.GetGroupByClient(initiator);
       	if (g != NULL){
			g->TeleportGroup(owner, zoneid, 0, x, y, z, 0.0f);
		}
		else {
			initiator->MovePC(zoneid, x, y, z, 0.0f);
		}
	}
}

void QuestManager::doanim(int anim_id) {
	owner->DoAnim(anim_id);
}

void QuestManager::addskill(int skill_id, int value) {
	if(skill_id < 0 || skill_id > HIGHEST_SKILL)	//must check before casting.
		return;
	if (initiator && initiator->IsClient())
		initiator->AddSkill((SkillType) skill_id, value);
}

void QuestManager::setlanguage(int skill_id, int value) {
	if (initiator && initiator->IsClient())
		initiator->SetLanguageSkill(skill_id, value);
}

void QuestManager::setskill(int skill_id, int value) {
	if(skill_id < 0 || skill_id > HIGHEST_SKILL)	//must check before casting.
		return;
	if (initiator && initiator->IsClient())
		initiator->SetSkill((SkillType) skill_id, value);
}

void QuestManager::setallskill(int value) {
	if (!initiator)
		return;
	if (initiator && initiator->IsClient()) {
		SkillType sk;
		for (sk = _1H_BLUNT; sk <= HIGHEST_SKILL; sk = (SkillType)(sk+1)) {
			initiator->SetSkill(sk, value);
		}
	}
}

void QuestManager::attack(const char *client_name) {
	if(!owner->IsNPC())
		return;
	Client* getclient = entity_list.GetClientByName(client_name);
	if(getclient && owner->IsAttackAllowed(getclient)) {
		owner->AddToHateList(getclient,1);
	} else {
		owner->Say("I am unable to attack %s.", client_name);
	}
}

void QuestManager::attacknpc(int npc_entity_id) {
	if(!owner->IsNPC())
		return;
	Mob *it = entity_list.GetMob(npc_entity_id);
	if(it && owner->IsAttackAllowed(it)) {
		owner->AddToHateList(it,1);
	} else {
		if(it)
			owner->Say("I am unable to attack %s.", it->GetName());
		else
			owner->Say("I am unable to locate NPC entity %i", npc_entity_id);
	}
}

void QuestManager::attacknpctype(int npc_type_id) {
	if(!owner->IsNPC())
		return;
	Mob *it = entity_list.GetMobByNpcTypeID(npc_type_id);
	if(it && owner->IsAttackAllowed(it)) {
		owner->AddToHateList(it,1);
	} else {
		if(it)
			owner->Say("I am unable to attack %s.", it->GetName());
		else
			owner->Say("I am unable to locate NPC type %i", npc_type_id);
	}
}

void QuestManager::save() {
	if (initiator && initiator->IsClient())
		initiator->Save();
}

void QuestManager::faction(int faction_id, int faction_value) {
	if (initiator && initiator->IsClient()) {
		if(faction_id != 0 && faction_value != 0) {
	// SCORPIOUS2K - fixed faction command
			//Client *p;
			initiator->SetFactionLevel2(
				initiator->CharacterID(),
				faction_id,
				initiator->GetBaseClass(),
				initiator->GetBaseRace(),
				initiator->GetDeity(),
				faction_value);

		}
	}
}

void QuestManager::setsky(uint8 new_sky) {
	if (zone)
		zone->newzone_data.sky = new_sky;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_NewZone, sizeof(NewZone_Struct));
	memcpy(outapp->pBuffer, &zone->newzone_data, outapp->size);
	entity_list.QueueClients(initiator, outapp);
	safe_delete(outapp);
}

void QuestManager::setguild(int32 new_guild_id, int8 new_rank) {
	if (initiator && initiator->IsClient()) {
		guild_mgr.SetGuild(initiator->CharacterID(), new_guild_id, new_rank);
	}
}

void QuestManager::settime(int8 new_hour, int8 new_min) {
	if (zone)
		zone->SetTime(new_hour + 1, new_min);
}

void QuestManager::itemlink(int item_id) {
	const ItemInst* inst = database.CreateItem(item_id, 20, 99999, 99999, 99999, 99999, 99998);
	char* link = 0;
	if (initiator->MakeItemLink(link, inst))
		initiator->Message(0, "%s tells you, %c%s%s%c", owner->GetCleanName(), 0x12, link, inst->GetItem()->Name, 0x12);
	safe_delete_array(link);
}

void QuestManager::signalwith(int npc_id, int signal_id, int wait_ms) {
// SCORPIOUS2K - signal command
	// signal(npcid) - generates EVENT_SIGNAL on specified npc
	if(wait_ms > 0) {
		STimerList.push_back(SignalTimer(wait_ms, npc_id, signal_id));
		return;
	}

	if (npc_id<1)
	{
		printf("signal() bad npcid=%i\n",npc_id);
	}
	else
	{
		//initiator* signalnpc=0;
		entity_list.SignalMobsByNPCID(npc_id, signal_id);
	}
}

void QuestManager::signal(int npc_id, int wait_ms) {
	signalwith(npc_id, 0, wait_ms);
}

void QuestManager::setglobal(const char *varname, const char *newvalue, int options, const char *duration) {
// SCORPIOUS2K - qglobal variable commands
	// setglobal(varname,value,options,duration)
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	//MYSQL_ROW row;
	int qgZoneid=zone->GetZoneID();
	int qgCharid=0;
	int qgNpcid = owner->GetNPCTypeID();

	/*	options value determines the availability of global variables to NPCs when a quest begins
	------------------------------------------------------------------
	  value		   npcid	  player		zone
	------------------------------------------------------------------
		0			this		this		this
		1			all			this		this
		2			this		all			this
		3			all			all			this
		4			this		this		all
		5			all			this		all
		6			this		all			all
		7			all			all			all
	*/
	if (initiator && initiator->IsClient())  // some events like waypoint and spawn don't have a player involved
	{
		qgCharid=initiator->CharacterID();
	}

	else
	{
		qgCharid=-qgNpcid;		// make char id negative npc id as a fudge
	}
	if (options < 0 || options > 7)
	{
		cerr << "Invalid options for global var " << varname << " using defaults" << endl;
	}	// default = 0 (only this npcid,player and zone)
	else
	{
		if (options & 1)
			qgNpcid=0;
		if (options & 2)
			qgCharid=0;
		if (options & 4)
			qgZoneid=0;
	}

	// clean up expired vars and get rid of the one we're going to set if there
	database.RunQuery(query, MakeAnyLenString(&query,
		"DELETE FROM quest_globals WHERE expdate < UNIX_TIMESTAMP() || (name='%s' && npcid=%i && charid=%i && zoneid=%i))"
		,varname,qgNpcid,qgCharid,qgZoneid), errbuf);
	safe_delete_array(query);

	InsertQuestGlobal(qgCharid, qgNpcid, qgZoneid, varname, newvalue, QGVarDuration(duration));
}

/* Inserts global variable into quest_globals table */
int QuestManager::InsertQuestGlobal(
									int charid, int npcid, int zoneid,
									const char *varname, const char *varvalue,
									int duration)
{
	char *query = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];

	// Make duration string either "unix_timestamp(now()) + xxx" or "NULL"
	stringstream duration_ss;
	if (duration == INT_MAX)
	{
		duration_ss << "NULL";
	}
	else
	{
		duration_ss << "unix_timestamp(now()) + " << duration;
	}

	//NOTE: this should be escaping the contents of arglist
	//npcwise a malicious script can arbitrarily alter the DB
	if (!database.RunQuery(query, MakeAnyLenString(&query,
		"REPLACE INTO quest_globals (charid, npcid, zoneid, name, value, expdate)"
		"VALUES (%i, %i, %i, '%s', '%s', %s)",
		charid, npcid, zoneid, varname, varvalue, duration_ss.str().c_str()
		), errbuf))
	{
		cerr << "setglobal error inserting " << varname << " : " << errbuf << endl;
	}
	safe_delete_array(query);

	return 0;
}

void QuestManager::targlobal(const char *varname, const char *value, const char *duration, int qgNpcid, int qgCharid, int qgZoneid) {
	// targlobal(varname,value,duration,npcid,charid,zoneid)
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	// clean up expired vars and get rid of the one we're going to set if there
	database.RunQuery(query, MakeAnyLenString(&query,
		"DELETE FROM quest_globals WHERE expdate < UNIX_TIMESTAMP()"
		" || (name='%s' && npcid=%i && charid=%i && zoneid=%i))"
		,varname,qgNpcid,qgCharid,qgZoneid), errbuf);
	safe_delete_array(query);

	InsertQuestGlobal(qgCharid, qgNpcid, qgZoneid, varname, value, QGVarDuration(duration));
}

void QuestManager::delglobal(const char *varname) {
	// delglobal(varname)
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int qgZoneid=zone->GetZoneID();
	int qgCharid=0;
	int qgNpcid=owner->GetNPCTypeID();
	if (initiator && initiator->IsClient())  // some events like waypoint and spawn don't have a player involved
	{
		qgCharid=initiator->CharacterID();
	}

	else
	{
		qgCharid=-qgNpcid;		// make char id negative npc id as a fudge
	}
	if (!database.RunQuery(query,
	  MakeAnyLenString(&query,
	  "DELETE FROM quest_globals WHERE name='%s'"
	  " && (npcid=0 || npcid=%i) && (charid=0 || charid=%i) && (zoneid=%i || zoneid=0)",
	  varname,qgNpcid,qgCharid,qgZoneid),errbuf))
	{
		cerr << "delglobal error deleting " << varname << " : " << errbuf << endl;
	}
	safe_delete_array(query);
}

// Converts duration string to duration value (in seconds)
// Return of INT_MAX indicates infinite duration
int QuestManager::QGVarDuration(const char *fmt)
{
	int duration = 0;

	// format:	Y#### or D## or H## or M## or S## or T###### or C#######

	int len = strlen(fmt);

	// Default to no duration
	if (len < 1)
		return 0;

	// Set val to value after type character
	// e.g., for "M3924", set to 3924
	int val = atoi(&fmt[0] + 1);

	switch (fmt[0])
	{
		// Forever
		case 'F':
		case 'f':
			duration = INT_MAX;
			break;
		// Years
		case 'Y':
		case 'y':
			duration = val * 31556926;
			break;
		case 'D':
		case 'd':
			duration = val * 86400;
			break;
		// Hours
		case 'H':
		case 'h':
			duration = val * 3600;
			break;
		// Minutes
		case 'M':
		case 'm':
			duration = val * 60;
			break;
		// Seconds
		case 'S':
		case 's':
			duration = val;
			break;
		// Invalid
		default:
			duration = 0;
			break;
	}

	return duration;
}

void QuestManager::ding() {
	//-Cofruben:makes a sound.
	if (initiator && initiator->IsClient())
		initiator->SendSound();

}

void QuestManager::rebind(int zoneid, float x, float y, float z) {
	if(initiator && initiator->IsClient()) {
		initiator->SetBindPoint(zoneid, x, y, z);
	}
}

void QuestManager::start(int wp) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->AssignWaypoints(wp);
}

void QuestManager::stop() {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->StopWandering();
}

void QuestManager::pause(int duration) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->PauseWandering(duration);
}

void QuestManager::moveto(float x, float y, float z) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->MoveTo(x, y, z);
}

void QuestManager::resume() {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->ResumeWandering();
}

void QuestManager::addldonpoints(sint32 points, int32 theme) {
	if(initiator)
		initiator->UpdateLDoNPoints(points, theme);
}

void QuestManager::setnexthpevent(int at) {
	owner->SetNextHPEvent( at );
}

void QuestManager::setnextinchpevent(int at) {
	owner->SetNextIncHPEvent( at );
}

void QuestManager::respawn(int npc_type, int grid) {
	if(!owner->IsNPC())
		return;
	//char tempa[100];
	float x,y,z,h;
	if ( !owner )
		return;

	x = owner->GetX();
	y = owner->GetY();
	z = owner->GetZ();
	h = owner->GetHeading();
	depop_npc = true;

	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{
		owner = new NPC(tmp, 0, x, y, z, h);
		owner->CastToNPC()->AddLootTable();
		entity_list.AddNPC(owner->CastToNPC(),true,true);
		if(grid > 0)
			owner->CastToNPC()->AssignWaypoints(grid);

		owner->SendPosUpdate();
	}
}

void QuestManager::set_proximity(float minx, float maxx, float miny, float maxy, float minz, float maxz) {
	if(!owner->IsNPC())
		return;

	entity_list.AddProximity(owner->CastToNPC());

	owner->CastToNPC()->proximity->min_x = minx;
	owner->CastToNPC()->proximity->max_x = maxx;
	owner->CastToNPC()->proximity->min_y = miny;
	owner->CastToNPC()->proximity->max_y = maxy;
	owner->CastToNPC()->proximity->min_z = minz;
	owner->CastToNPC()->proximity->max_z = maxz;

	owner->CastToNPC()->proximity->say = ((PerlembParser *)parse)->HasQuestSub(owner->CastToNPC()->GetNPCTypeID(),"EVENT_PROXIMITY_SAY");

	if(owner->CastToNPC()->proximity->say)
		HaveProximitySays = true;
}

void QuestManager::clear_proximity() {
	if(!owner->IsNPC())
		return;
	safe_delete(owner->CastToNPC()->proximity);
	entity_list.RemoveProximity(owner->GetID());
}

void QuestManager::setanim(int npc_type, int animnum) {
	//Cisyouc: adds appearance changes
	Mob* thenpc = entity_list.GetMobByNpcTypeID(npc_type);
	if(animnum < 0 || animnum >= _eaMaxAppearance)
		return;
	thenpc->SetAppearance(EmuAppearance(animnum));
}


//displays an in game path based on a waypoint grid
void QuestManager::showgrid(int grid) {
	if(initiator == NULL)
		return;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	FindPerson_Point pt;
	vector<FindPerson_Point> pts;

	pt.x = initiator->GetX();
	pt.y = initiator->GetY();
	pt.z = initiator->GetZ();
	pts.push_back(pt);

	// Retrieve all waypoints for this grid
	if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `x`,`y`,`z` FROM grid_entries WHERE `gridid`=%i AND `zoneid`=%i ORDER BY `number`",grid,zone->GetZoneID()),errbuf,&result))
	{
		while((row = mysql_fetch_row(result)))
		{
			pt.x = atof(row[0]);
			pt.y = atof(row[1]);
			pt.z = atof(row[2]);
			pts.push_back(pt);
		}
		mysql_free_result(result);

		initiator->SendPathPacket(pts);
	}
	else	// DB query error!
	{
		LogFile->write(EQEMuLog::Quest, "Error loading grid %d for showgrid(): %s", grid, errbuf);
		return;
	}
	safe_delete_array(query);
}

//displays an in game path based on path finding.
void QuestManager::showpath(float x, float y, float z) {
	say("showpath not implemented yet.");
}

//causes the npc to use path finding to walk to x,y,z
void QuestManager::pathto(float x, float y, float z) {
	say("pathto not implemented yet.");
}

//change the value of a spawn condition
void QuestManager::spawn_condition(const char *zone_short, uint16 condition_id, short new_value) {
	zone->spawn_conditions.SetCondition(zone_short, condition_id, new_value);
}

//get the value of a spawn condition
short QuestManager::get_spawn_condition(const char *zone_short, uint16 condition_id) {
	return(zone->spawn_conditions.GetCondition(zone_short, condition_id));
}

//toggle a spawn event
void QuestManager::toggle_spawn_event(int event_id, bool enable, bool reset_base) {
	zone->spawn_conditions.ToggleEvent(event_id, enable, reset_base);
}

bool QuestManager::has_zone_flag(int zone_id) {
	return(initiator->HasZoneFlag(zone_id));
}

void QuestManager::set_zone_flag(int zone_id) {
	initiator->SetZoneFlag(zone_id);
}

void QuestManager::clear_zone_flag(int zone_id) {
	initiator->ClearZoneFlag(zone_id);
}

void QuestManager::sethp(int hpperc) {
	int newhp;

	newhp = (owner->GetMaxHP()*(100-hpperc))/100;
	owner->Damage(owner, newhp, SPELL_UNKNOWN, HAND_TO_HAND, false, 0, false);
}

bool QuestManager::summonburriedplayercorpse(int32 char_id, float dest_x, float dest_y, float dest_z, float dest_heading) {
	bool Result = false;

	if(char_id > 0) {
		Corpse* PlayerCorpse = database.SummonBurriedPlayerCorpse(char_id, zone->GetZoneID(), zone->GetInstanceID(), dest_x, dest_y, dest_z, dest_heading);
		
		if(PlayerCorpse) {
			PlayerCorpse->Spawn();
			Result = true;
		}
	}

	return Result;
}

int32 QuestManager::getplayerburriedcorpsecount(int32 char_id) {
	int32 Result = 0;

	if(char_id > 0) {
		Result = database.GetPlayerBurriedCorpseCount(char_id);
	}

	return Result;
}

bool QuestManager::buryplayercorpse(int32 char_id) {
	bool Result = false;

	if(char_id > 0) {
		int32 PlayerCorpse = database.GetFirstCorpseID(char_id);

		if(PlayerCorpse > 0){
			database.BuryPlayerCorpse(PlayerCorpse);
			Corpse* CorpseEntity = entity_list.GetCorpseByDBID(PlayerCorpse);

				if(CorpseEntity > 0){
					CorpseEntity->Save();
					CorpseEntity->DepopCorpse();
					Result = true;
		}
		}
	}
	return Result;
}

void QuestManager::forcedooropen(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		if(GetInitiator())
			d->ForceOpen(GetInitiator());
		else if(GetOwner())
			d->ForceOpen(GetOwner());
	}
}

void QuestManager::forcedoorclose(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		if(GetInitiator())
			d->ForceClose(GetInitiator());
		else if(GetOwner())
			d->ForceClose(GetOwner());
	}
}

bool QuestManager::isdooropen(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		return d->IsDoorOpen();
	}
	return false;
}
void QuestManager::npcrace(int race_id)
{
		owner->SendIllusionPacket(race_id);
}

void QuestManager::npcgender(int gender_id)
{
		owner->SendIllusionPacket(owner->GetRace(), gender_id);
}
void QuestManager::npcsize(int newsize)
{
				owner->ChangeSize(newsize, true);
}
void QuestManager::npctexture(int newtexture)
{
			owner->SendIllusionPacket(owner->GetRace(), 0xFF, newtexture);
}

void QuestManager::playerrace(int race_id)
{
			initiator->SendIllusionPacket(race_id);
}

void QuestManager::playergender(int gender_id)
{
			initiator->SendIllusionPacket(initiator->GetRace(), gender_id);
}
void QuestManager::playersize(int newsize)
{
			initiator->ChangeSize(newsize, true);
}
void QuestManager::playertexture(int newtexture)
{
			initiator->SendIllusionPacket(initiator->GetRace(), 0xFF, newtexture);
}
void QuestManager::popup(char *title, char *text, int32 popupid, int32 buttons) 
{
         if(initiator) initiator->SendPopupToClient(title, text, popupid, buttons);
}

#ifdef EQBOTS

int QuestManager::createbotcount() {
	return RuleI(EQOffline, CreateBotCount);
}

int QuestManager::spawnbotcount() {
	return RuleI(EQOffline, SpawnBotCount);
}

bool QuestManager::botquest()
{
	return RuleB(EQOffline, BotQuest);
}

#endif //EQBOTS

void QuestManager::taskselector(int taskcount, int *tasks) {
	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner && taskmanager) 
		taskmanager->SendTaskSelector(initiator, owner, taskcount, tasks);
}
void QuestManager::enabletask(int taskcount, int *tasks) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager) 
		initiator->EnableTask(taskcount, tasks);
}

void QuestManager::disabletask(int taskcount, int *tasks) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager) 
		initiator->DisableTask(taskcount, tasks);
}

bool QuestManager::istaskenabled(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager) 
		return initiator->IsTaskEnabled(taskid);

	return false;
}

void QuestManager::tasksetselector(int tasksetid) {
	_log(TASKS__UPDATE, "TaskSetSelector called for task set %i", tasksetid);
	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner && taskmanager) 
		initiator->TaskSetSelector(owner, tasksetid);
}

bool QuestManager::istaskactive(int task) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskActive(task);

	return false;
}
bool QuestManager::istaskactivityactive(int task, int activity) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskActivityActive(task, activity);

	return false;
}

void QuestManager::updatetaskactivity(int task, int activity, int count) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->UpdateTaskActivity(task, activity, count);
}

void QuestManager::taskexploredarea(int exploreid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->UpdateTasksOnExplore(exploreid);
}

void QuestManager::assigntask(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		initiator->AssignTask(taskid, owner->GetID());
}

void QuestManager::failtask(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->FailTask(taskid);
}

int QuestManager::tasktimeleft(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->TaskTimeLeft(taskid);

	return -1;
}

int QuestManager::enabledtaskcount(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->EnabledTaskCount(taskset);

	return -1;
}
int QuestManager::firsttaskinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->FirstTaskInSet(taskset);

	return -1;
}
int QuestManager::lasttaskinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->LastTaskInSet(taskset);

	return -1;
}
int QuestManager::nexttaskinset(int taskset, int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->NextTaskInSet(taskset, taskid);

	return -1;
}
int QuestManager::activespeaktask() {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		return initiator->ActiveSpeakTask(owner->GetNPCTypeID());

	return 0;
}
int QuestManager::activespeakactivity(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		return initiator->ActiveSpeakActivity(owner->GetNPCTypeID(), taskid);

	return 0;
}
int QuestManager::istaskcompleted(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskCompleted(taskid);

	return -1;
}
int QuestManager::activetasksinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->ActiveTasksInSet(taskset);

	return -1;
}
int QuestManager::completedtasksinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->CompletedTasksInSet(taskset);

	return -1;
}

bool QuestManager::istaskappropriate(int task) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager)
		return taskmanager->AppropriateLevel(task, initiator->GetLevel());

	return false;
}
void QuestManager::clearspawntimers() {
	if(zone)  {
		//TODO: Dec 19, 2008, replace with code updated for current spawn timers.
		LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
		iterator.Reset();
		while (iterator.MoreElements()) 
		{
			char errbuf[MYSQL_ERRMSG_SIZE];
			char *query = 0;
			database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM respawn_times WHERE id=%lu AND "
				"instance_id=%lu",iterator.GetData()->GetID(), zone->GetInstanceID()), errbuf);
			safe_delete_array(query);		
			iterator.Advance();
		}
	}
}
void QuestManager::ze(int type, const char *str) {
	entity_list.Message(0, type, str);
}

void QuestManager::we(int type, const char *str) {
	worldserver.SendEmoteMessage(0, 0, type, str);
}

int QuestManager::getlevel(uint8 type)
{
	if (type == 0)
	{
		return (initiator->GetLevel());
	}
	else if(type == 1)
	{
		Group *g = entity_list.GetGroupByClient(initiator);
       	if (g != NULL)
			return (g->GetAvgLevel());
		else
			return 0;
	}
	else if(type == 2)
	{
		Raid *r = entity_list.GetRaidByClient(initiator);
       	if (r != NULL)
			return (r->GetAvgLevel());
		else
			return 0;
	}
	else if(type == 3)
	{
		Raid *r = entity_list.GetRaidByClient(initiator);
		if(r != NULL)
		{
			return (r->GetAvgLevel());
		}
		Group *g = entity_list.GetGroupByClient(initiator);
		if(g != NULL)
		{
			return (g->GetAvgLevel());
		}
		else
			return (initiator->GetLevel());
	}
	else
		return 0;
}

void QuestManager::CreateGroundObject(int32 itemid, float x, float y, float z, float heading, int32 decay_time)
{
	entity_list.CreateGroundObject(itemid, x, y, z, heading, decay_time);
}

void QuestManager::ModifyNPCStat(const char *identifier, const char *newValue)
{
	if(owner){
		if(owner->IsNPC())
		{
			owner->CastToNPC()->ModifyNPCStat(identifier, newValue);
		}
	}
}

int QuestManager::collectitems_processSlot(sint16 slot_id, uint32 item_id,
	bool remove)
{
	ItemInst *item;
	int quantity = 0;
 
	item = initiator->GetInv().GetItem(slot_id);
 
	// If we have found matching item, add quantity
	if (item && item->GetID() == item_id)
	{
		// If item is stackable, add its charges (quantity)
		if (item->IsStackable())
		{
			quantity = item->GetCharges();
		}
		else
		{
			quantity = 1;
		}
 
		// Remove item from inventory
		if (remove)
		{
			initiator->DeleteItemInInventory(slot_id, 0, true);
		}
	}
 
	return quantity;
}
 
// Returns number of item_id that exist in inventory
// If remove is true, items are removed as they are counted.
int QuestManager::collectitems(uint32 item_id, bool remove)
{
	int quantity = 0;
	int slot_id;
 
	for (slot_id = 22; slot_id <= 29; ++slot_id)
	{
		quantity += collectitems_processSlot(slot_id, item_id, remove);
	}
 
	for (slot_id = 251; slot_id <= 330; ++slot_id)
	{
		quantity += collectitems_processSlot(slot_id, item_id, remove);
	}
 
	return quantity;
}

void QuestManager::UpdateSpawnTimer(int32 id, int32 newTime)
{
	bool found = false;

	LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
	iterator.Reset();
	while (iterator.MoreElements()) 
	{
		if(iterator.GetData()->GetID() == id)
		{
			if(!iterator.GetData()->NPCPointerValid())
			{
				iterator.GetData()->SetTimer(newTime);
			}
			found = true;
			break;
		}
		iterator.Advance();
	}

	if(!found)
	{
		//Spawn wasn't in this zone...
		//Tell the other zones to update their spawn time for this spawn point
		ServerPacket *pack = new ServerPacket(ServerOP_UpdateSpawn, sizeof(UpdateSpawnTimer_Struct));
		UpdateSpawnTimer_Struct *ust = (UpdateSpawnTimer_Struct*)pack->pBuffer;
		ust->id = id;
		ust->duration = newTime;
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
}

// used to set the number of an item in the selected merchant's temp item list.  Defaults to zero if no quantity is specified.
void QuestManager::MerchantSetItem(int32 NPCid, int32 itemid, int32 quantity) {
	Mob* merchant = entity_list.GetMobByNpcTypeID(NPCid);
	
	if (merchant == 0 || !merchant->IsNPC() || (merchant->GetClass() != MERCHANT)) 
		return;	// don't do anything if NPCid isn't a merchant
	
	const Item_Struct* item = NULL;
	item = database.GetItem(itemid);
	if (!item) return;		// if the item id doesn't correspond to a real item, do nothing
	
	zone->SaveTempItem(merchant->CastToNPC()->MerchantType, NPCid, itemid, quantity);
}

int32 QuestManager::MerchantCountItem(int32 NPCid, int32 itemid) {
	Mob* merchant = entity_list.GetMobByNpcTypeID(NPCid);
	
	if (merchant == 0 || !merchant->IsNPC() || (merchant->GetClass() != MERCHANT))
		return 0;	// if it isn't a merchant, it doesn't have any items
	
	const Item_Struct* item = NULL;
	item = database.GetItem(itemid);
	if (!item) return 0;		// likewise, if it isn't a valid item, the merchant doesn't have any

	// look for the item in the merchant's temporary list
	std::list<TempMerchantList> MerchList = zone->tmpmerchanttable[NPCid];
	std::list<TempMerchantList>::const_iterator itr;
	TempMerchantList ml;
	int32 Quant = 0;
		
	for(itr = MerchList.begin(); itr != MerchList.end(); itr++){
		ml = *itr;
		if (ml.item == itemid) {	// if this is the item we're looking for
			Quant = ml.charges;
			break;
		}
	}

	return Quant;	// return the quantity of itemid (0 if it was never found)
}

// Item Link for use in Variables - "my $example_link = quest::varlink(item_id);"
const char* QuestManager::varlink(char* perltext, int item_id) {
	const ItemInst* inst = database.CreateItem(item_id, 20, 99999, 99999, 99999, 99999, 99998);
	if (!inst) return 0;	// return an empty string if the item is invalid
	char* link = 0;
	char* tempstr = 0;
	if (initiator->MakeItemLink(link, inst)) {	// make a link to the item
		MakeAnyLenString(&tempstr, "%c%s%s%c", 0x12, link, inst->GetItem()->Name, 0x12);
		strncpy(perltext, tempstr,250);	// the perl string is only 250 chars, so make sure the link isn't too large
		safe_delete_array(tempstr);	// MakeAnyLenString() uses new, so clean up after it
	}
	safe_delete_array(link);	// MakeItemLink() uses new also
	return perltext;
}

int16 QuestManager::CreateInstance(const char *zone, int16 version, int32 duration)
{
	if(initiator)
	{
		int32 zone_id = database.GetZoneID(zone);
		if(zone_id == 0)
			return 0;

		int16 id = 0;
		if(!database.GetUnusedInstanceID(id))
		{
			initiator->Message(13, "Server was unable to find a free instance id.");
			return 0;
		}

		if(!database.CreateInstance(id, zone_id, version, duration))
		{
			initiator->Message(13, "Server was unable to create a new instance.");
			return 0;
		}
		return id;
	}
	return 0;
}

void QuestManager::DestroyInstance(int16 instance_id)
{
	database.DeleteInstance(instance_id);
}

int16 QuestManager::GetInstanceID(const char *zone, int16 version)
{
	if(initiator)
	{
		return database.GetInstanceID(zone, initiator->CharacterID(), version);
	}
	return 0;
}

void QuestManager::AssignToInstance(int16 instance_id)
{
	if(initiator)
	{
		database.AddClientToInstance(instance_id, initiator->CharacterID());
	}
}

void QuestManager::AssignGroupToInstance(int16 instance_id)
{
	if(initiator)
	{
		Group *g = initiator->GetGroup();
		if(g)
		{
			int32 gid = g->GetID();
			database.AssignGroupToInstance(gid, instance_id);
		}
	}
}

void QuestManager::AssignRaidToInstance(int16 instance_id)
{
	if(initiator)
	{
		Raid *r = initiator->GetRaid();
		if(r)
		{
			int32 rid = r->GetID();
			database.AssignRaidToInstance(rid, instance_id);
		}
	}
}

void QuestManager::MovePCInstance(int zone_id, int instance_id, float x, float y, float z, float heading)
{
	if(initiator)
	{
		initiator->MovePC(zone_id, instance_id, x, y, z, heading);
	}
}

void QuestManager::FlagInstanceByGroupLeader(int32 zone, int16 version)
{
	if(initiator)
	{
		Group *g = initiator->GetGroup();
		if(g){
			database.FlagInstanceByGroupLeader(zone, version, initiator->CharacterID(), g->GetID());
		}
	}
}

void QuestManager::FlagInstanceByRaidLeader(int32 zone, int16 version)
{
	if(initiator)
	{
		Raid *r = initiator->GetRaid();
		if(r)
		{
			database.FlagInstanceByRaidLeader(zone, version, initiator->CharacterID(), r->GetID());
		}
	}
}

const char* QuestManager::saylink(char* Phrase) {

	const char *ERR_MYSQLERROR = "Error in saylink phrase queries";
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int sayid = 0;

	// Query for an existing phrase and id in the saylink table
	if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `id` FROM `saylink` WHERE `phrase` = '%s'", Phrase),errbuf,&result))
	{
		if (mysql_num_rows(result) >= 1)
		{
			while((row = mysql_fetch_row(result)))
			{
				sayid = atoi(row[0]);
			}
			mysql_free_result(result);
		}
		else   // Add a new saylink entry to the database and query it again for the new sayid number
		{
			safe_delete_array(query);

			database.RunQuery(query,MakeAnyLenString(&query,"INSERT INTO `saylink` (`phrase`) VALUES ('%s')", Phrase),errbuf);
			safe_delete_array(query);

			if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `id` FROM saylink WHERE `phrase` = '%s'", Phrase),errbuf,&result))
			{
				if (mysql_num_rows(result) >= 1)
				{
					while((row = mysql_fetch_row(result)))
					{
						sayid = atoi(row[0]);
					}
					mysql_free_result(result);
				}
			}
			else 
			{
				LogFile->write(EQEMuLog::Error, ERR_MYSQLERROR, errbuf);
			}
			safe_delete_array(query);
		}
	}
	safe_delete_array(query);

	//Create the say link as an item link hash
	char linktext[250];
	if (initiator->GetClientVersion() == EQClientSoF)
	{
		sprintf(linktext,"%c%06X%s%s%c",0x12,500000+sayid,"00000000000000000000000000000000000000000000",Phrase,0x12);
	}
	else
	{
		sprintf(linktext,"%c%06X%s%s%c",0x12,500000+sayid,"000000000000000000000000000000000000000",Phrase,0x12);
	}
	strcpy(Phrase,linktext);
	return Phrase;
}
