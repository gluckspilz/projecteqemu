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
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
#define snprintf	_snprintf
#endif

#include "forage.h"
#include "entity.h"
#include "masterentity.h"
#include "npc.h"
#include "watermap.h"
#include "StringIDs.h"
#include "../common/MiscFunctions.h"
#include "../common/rulesys.h"

#include "zonedb.h"
#ifdef WIN32
#define snprintf	_snprintf
#endif



//max number of items which can be in the foraging table
//for a given zone.
#define FORAGE_ITEM_LIMIT 50

/*

The fishing and foraging need some work...
foraging currently gives each item an equal chance of dropping
fishing gives items which come in last from the select a very
very low chance of dropping.


Schema:
CREATE TABLE forage (
  id int(11) NOT NULL auto_increment,
  zoneid int(4) NOT NULL default '0',
  Itemid int(11) NOT NULL default '0',
  level smallint(6) NOT NULL default '0',
  chance smallint(6) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

old table upgrade:
alter table forage add chance smallint(6) NOT NULL default '0';
update forage set chance=100;


CREATE TABLE fishing (
  id int(11) NOT NULL auto_increment,
  zoneid int(4) NOT NULL default '0',
  Itemid int(11) NOT NULL default '0',
  skill_level smallint(6) NOT NULL default '0',
  chance smallint(6) NOT NULL default '0',
  npc_id int NOT NULL default 0,
  npc_chance int NOT NULL default 0,
  PRIMARY KEY  (id)
) TYPE=MyISAM;


*/

// This allows EqEmu to have zone specific foraging - BoB
int32 ZoneDatabase::GetZoneForage(int32 ZoneID, int8 skill) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	
	int8 index = 0, rindex;
	int32 item[FORAGE_ITEM_LIMIT];
	int8 chance[FORAGE_ITEM_LIMIT];
	int32 ret;
	
	for (int c=0; c < FORAGE_ITEM_LIMIT; c++) 	{
		item[c] = 0;
	}
	
	int32 csum = 0;
	
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT itemid,chance FROM forage WHERE zoneid= '%i' and level <= '%i' LIMIT %i", ZoneID, skill, FORAGE_ITEM_LIMIT), errbuf, &result))
	{
		safe_delete_array(query);
		while ((row = mysql_fetch_row(result)) && (index < FORAGE_ITEM_LIMIT)) 	{
			item[index] = atoi(row[0]);
			chance[index] = atoi(row[1]);
LogFile->write(EQEMuLog::Error, "Possible Forage: %d with a %d chance", item[index], chance[index]);
			csum += chance[index];
			index++;
		}
		
		mysql_free_result(result);
	}
	else {
		LogFile->write(EQEMuLog::Error, "Error in Forage query '%s': %s", query, errbuf);
		safe_delete_array(query);
		return 0;
	}
	
	if(csum == 0 || index < 1)
		return(0);
	
	if(index == 1) {
		return(item[0]);
	}
	
	ret = 0;
	
	while(ret == 0) {
		rindex = MakeRandomInt(0, index-1);
		if(MakeRandomInt(0, 100) < chance[rindex])
			ret = item[rindex];
	}
	
	return ret;
}

int32 ZoneDatabase::GetZoneFishing(int32 ZoneID, int8 skill, uint32 &npc_id, uint8 &npc_chance)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	
	int8 index = 0;
	int32 item[50];
	int32 chance[50];
	int32 npc_ids[50];
	int32 npc_chances[50];
	int32 chancepool = 0;
	int32 ret = 0;
	
	for (int c=0; c<50; c++) 	{
		item[c]=0;
		chance[c]=0;
	}
	
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT itemid,chance,npc_id,npc_chance FROM fishing WHERE (zoneid= '%i' || zoneid = 0) and skill_level <= '%i'",ZoneID, skill ), errbuf, &result))
	{
		safe_delete_array(query);
		while ((row = mysql_fetch_row(result))&&(index<50)) 	{
			item[index] = atoi(row[0]);
			chance[index] = atoi(row[1])+chancepool;
			chancepool = chance[index];
			
			npc_ids[index] = atoi(row[2]);
			npc_chances[index] = atoi(row[3]);
			index++;
		}
		
		mysql_free_result(result);
	}
	else {
		cerr << "Error in Fishing query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}
	
	npc_id = 0;
	npc_chance = 0;
	if (index>0) {
		int32 random = (rand()%chancepool)+1;
		for (int i = 0; i < index; i++)
		{
			if (random <= chance[i])
			{
				ret = item[i];
				npc_id = npc_ids[i];
				npc_chance = npc_chances[i];
				break;
			}
		}
	} else {
		ret = 0;
	}
	
	return ret;
}

void Client::GoFish()
{
	
	fishing_timer.Disable();
	
	//multiple entries yeilds higher probability of dropping...
	uint32 common_fish_ids[MAX_COMMON_FISH_IDS] = {
		1038, // Tattered Cloth Sandals
		1038, // Tattered Cloth Sandals
		1038, // Tattered Cloth Sandals
		13019, // Fresh Fish
		13076, // Fish Scales
		13076, // Fish Scales
            7007, // Rusty Dagger
		7007, // Rusty Dagger
            7007 // Rusty Dagger
		
	};
	
	//success formula is not researched at all
	
	int fishing_skill = GetSkill(FISHING);	//will take into account skill bonuses on pole & bait
	
	//make sure we still have a fishing pole on:
	const ItemInst* Pole = m_inv[SLOT_PRIMARY];
	sint32 bslot = m_inv.HasItemByUse(ItemTypeFishingBait, 1, invWhereWorn|invWherePersonal);
	const ItemInst* Bait = NULL;
	if(bslot != SLOT_INVALID)
		Bait = m_inv.GetItem(bslot);
	
	if(!Pole || !Bait) {
		Message(0, "You are missing a fishing pole or bait.");
		return;
	}
	
	if(!Pole->IsType(ItemClassCommon) || Pole->GetItem()->ItemType != ItemTypeFishingPole) {
		Message(0, "You do not have a fishing pole equipped.");
		return;
	}
	
	if(!Bait->IsType(ItemClassCommon) || Bait->GetItem()->ItemType != ItemTypeFishingBait) {
		Message(0, "You do not have any bait.");
		return;
	}

	if(zone->map!=NULL && zone->watermap != NULL && RuleB(Watermap, CheckForWaterWhenFishing)) {
		float RodX, RodY, RodZ;
		// Tweak Rod and LineLength if required
		const float RodLength = RuleR(Watermap, FishingRodLength);
		const float LineLength = RuleR(Watermap, FishingLineLength);
		int HeadingDegrees;

		HeadingDegrees = (int) ((GetHeading()*360)/256);
		HeadingDegrees = HeadingDegrees % 360;
		
		RodX = x_pos + RodLength * sin(HeadingDegrees * M_PI/180.0f);
		RodY = y_pos + RodLength * cos(HeadingDegrees * M_PI/180.0f);
		
		// Do BestZ to find where the line hanging from the rod intersects the water (if it is water).
		// and go 1 unit into the water.
		VERTEX dest;
		dest.x = RodX;
		dest.y = RodY;
		dest.z = z_pos+10;
		NodeRef n = zone->map->SeekNode( zone->map->GetRoot(), dest.x, dest.y);
		if(n != NODE_NONE) {
			RodZ = zone->map->FindBestZ(n, dest, NULL, NULL) - 1;
			bool in_water = zone->watermap->InWater(RodX, RodY, RodZ);
			//Message(0, "Rod is at %4.3f, %4.3f, %4.3f, InWater says %d", RodX, RodY, RodZ, in_water);
			if((z_pos-RodZ)>LineLength) {
				// The water is too far below us
				Message(0, "Trying to catch land sharks perhaps?");
				return;
			}
			if(!in_water) {
				Message(0, "Trying to catch land sharks perhaps?");
				return;
			}
		}
	}


	//if the bait isnt equipped, need to add its skill bonus
	if(bslot >= IDX_INV && Bait->GetItem()->SkillModType == FISHING) {
		fishing_skill += Bait->GetItem()->SkillModValue;
	}
	
	if (fishing_skill > 100)
	{
		fishing_skill = 100+((fishing_skill-100)/2);
	}
	
	if (MakeRandomInt(0,175) < fishing_skill) {
		uint32 food_id = 0;
		
		//25% chance to fish an item.
        if (MakeRandomInt(0, 399) <= fishing_skill ) {
			uint32 npc_id = 0;
			uint8 npc_chance = 0;
			food_id = database.GetZoneFishing(m_pp.zone_id, fishing_skill, npc_id, npc_chance);
			
			//check for add NPC
			if(npc_chance > 0 && npc_id) {
				if(npc_chance < MakeRandomInt(0, 99)) {
					const NPCType* tmp = database.GetNPCType(npc_id);
					if(tmp != NULL) {
						NPC* npc = new NPC(tmp, NULL, GetX()+3, GetY(), GetZ(), GetHeading());
						npc->AddLootTable();
						
						npc->AddToHateList(this, 1, 0, false);	//no help yelling
						
						entity_list.AddNPC(npc);
						
						Message(MT_Emote, "You fish up a little more than you bargained for...");
					}
				}
			}
		}
		
		//consume bait, should we always consume bait on success?
		DeleteItemInInventory(bslot, 1, true);	//do we need client update?
		
		if(food_id == 0) {
			int index = MakeRandomInt(0, MAX_COMMON_FISH_IDS-1);
			food_id = common_fish_ids[index];
		}
		
		const Item_Struct* food_item = database.GetItem(food_id);
		
		Message_StringID(MT_Skills, FISHING_SUCCESS);
		const ItemInst* inst = database.CreateItem(food_item, 1);
		if(inst != NULL) {
			PutItemInInventory(SLOT_CURSOR, *inst);
			SendItemPacket(SLOT_CURSOR,inst,ItemPacketSummonItem);
			if(RuleB(TaskSystem, EnableTaskSystem))
				UpdateTasksForItem(ActivityFish, food_id);
		}
	}
	else
	{
		//chance to use bait when you dont catch anything...
		if (MakeRandomInt(0, 4) == 1) {
			DeleteItemInInventory(bslot, 1, true);	//do we need client update?
			Message_StringID(MT_Skills, FISHING_LOST_BAIT);	//lost bait
		} else {
			Message_StringID(MT_Skills, FISHING_FAILED);
		}
	}
	
	//chance to break fishing pole...
	//this is potentially exploitable in that they can fish
	//and then swap out items in primary slot... too lazy to fix right now
	if (MakeRandomInt(0, 49) == 1) {
		Message_StringID(MT_Skills,169);
		DeleteItemInInventory(13,0,true);
	}

	CheckIncreaseSkill(FISHING,5);
}

void Client::ForageItem() {
	
	int skill_level = GetSkill(FORAGE);
	
	//be wary of the string ids in switch below when changing this.
	uint32 common_food_ids[MAX_COMMON_FOOD_IDS] = {
		13046, // Fruit
		13045, // Berries
		13419, // Vegetables
		13048, // Rabbit Meat
		13047, // Roots
		13044, // Pod Of Water
		14905, // mushroom
		13106 // Fishing Grubs
	};
	
	// these may need to be fine tuned, I am just guessing here
	if (MakeRandomInt(0,199) < skill_level) {
		uint32 foragedfood = 0;
		int32 stringid = FORAGE_NOEAT;
		
		if (MakeRandomInt(0,99) <= 25) {
			foragedfood = database.GetZoneForage(m_pp.zone_id, skill_level);
		}
		
		//not an else in case theres no DB food
		if(foragedfood == 0) {
			int8 index = 0;
			index = rand()%MAX_COMMON_FOOD_IDS;
			foragedfood = common_food_ids[index];
		}
		
		const Item_Struct* food_item = database.GetItem(foragedfood);

		if(!food_item) {
			LogFile->write(EQEMuLog::Error, "NULL returned from database.GetItem in ClientForageItem");
			return;
		}

		if(foragedfood == 13106) 
			stringid = FORAGE_GRUBS;
		else
			switch(food_item->ItemType) {

				case ItemTypeFood: 
					stringid = FORAGE_FOOD;
					break;
					
				case ItemTypeDrink: 
					if(strstr(food_item->Name, "ater")) 
						stringid = FORAGE_WATER;
					else
						stringid = FORAGE_DRINK;
					break;
				default:
					break;
				}
		
		Message_StringID(MT_Skills, stringid);
		const ItemInst* inst = database.CreateItem(food_item, 1);
		if(inst != NULL) {
			// check to make sure it isn't a foraged lore item
			if(CheckLoreConflict(inst->GetItem())) 
				this->Message_StringID(0,DUP_LORE);
			else {
			PushItemOnCursor(*inst);
			SendItemPacket(SLOT_CURSOR, inst, ItemPacketSummonItem);
			if(RuleB(TaskSystem, EnableTaskSystem))
				UpdateTasksForItem(ActivityForage, foragedfood);
			}
		}
		
	} else {
		Message_StringID(MT_Skills, FORAGE_FAILED);
	}
	
	CheckIncreaseSkill(FORAGE,5);
	
}
