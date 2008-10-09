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
#include "../common/debug.h"

#include "zone.h"
#include "worldserver.h"
#include "masterentity.h"
#include "../common/packet_dump.h"
#include "../common/rulesys.h"
#include "StringIDs.h"

#ifdef EMBPERL
#include "embparser.h"
#endif

extern WorldServer worldserver;
extern Zone* zone;


void Client::Handle_OP_ZoneChange(const EQApplicationPacket *app) {

#ifdef EQBOTS

	Mob *clientmob = CastToMob();
	database.CleanBotLeader(GetID());
	if(clientmob) {
		if(clientmob->IsBotRaiding()) {
			BotRaids* br = entity_list.GetBotRaidByMob(clientmob);
			if(br) {
				br->RemoveRaidBots();
				br = NULL;
			}
		}
		Group *g = entity_list.GetGroupByMob(clientmob);
		if(g) {
			bool hasBots = false;
			for(int i=5; i>=0; i--) {
				if(g->members[i] && g->members[i]->IsBot()) {
					hasBots = true;
					g->members[i]->BotOwner = NULL;
					g->members[i]->Kill();
				}
			}
			if(hasBots) {
				hasBots = false;
				if(g->BotGroupCount() <= 1) {
					g->DisbandGroup();
				}
			}
		}
	}

#endif //EQBOTS

	zoning = true;
	if (app->size != sizeof(ZoneChange_Struct)) {
		LogFile->write(EQEMuLog::Debug, "Wrong size: OP_ZoneChange, size=%d, expected %d", app->size, sizeof(ZoneChange_Struct));
		return;
	}

#if EQDEBUG >= 5
	LogFile->write(EQEMuLog::Debug, "Zone request from %s", GetName());
	DumpPacket(app);
#endif
	ZoneChange_Struct* zc=(ZoneChange_Struct*)app->pBuffer;

	uint16 target_zone_id = 0;
	ZonePoint* zone_point = NULL;

	//figure out where they are going.
	if(zc->zoneID == 0) {
		//client dosent know where they are going...
		//try to figure it out for them.
		
		switch(zone_mode) {
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			//going to safe coords, but client dosent know where?
			//assume it is this zone for now.
			cheat_timer.Start(35000,false);
			target_zone_id = zone->GetZoneID();
			break;
		case GMSummon:
			target_zone_id = zonesummon_id;
			break;
		case GateToBindPoint:
			target_zone_id = m_pp.binds[0].zoneId;
			break;
		case ZoneToBindPoint:
			target_zone_id = m_pp.binds[0].zoneId;
			break;
		case ZoneSolicited:  //we told the client to zone somewhere, so we know where they are going.
			cheat_timer.Start(35000,false);
			target_zone_id = zonesummon_id;
			break;
		case ZoneUnsolicited:   //client came up with this on its own.
			zone_point = zone->GetClosestZonePointWithoutZone(GetX(), GetY(), GetZ(), ZONEPOINT_NOZONE_RANGE);
			if(zone_point) {
				//we found a zone point, which is a reasonable distance away
				//assume that is the one were going with.
				target_zone_id = zone_point->target_zone_id;
			} else {
				//unable to find a zone point... is there anything else
				//that can be a valid un-zolicited zone request?
				
				CheatDetected(MQZone);
				Message(13, "Invalid unsolicited zone request.");
				LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%d'.", GetName(), target_zone_id);
				SendZoneCancel(zc);
				return;
			}
			break;
		};
	}
	else {
		// This is to allow both 6.2 and Titanium clients to perform a proper zoning of the client when evac/succor
		// WildcardX 27 January 2008
		if(zone_mode == EvacToSafeCoords && zonesummon_id > 0)
			target_zone_id = zonesummon_id;
		else
		target_zone_id = zc->zoneID;
		
		//if we are zoning to a specific zone unsolicied,
		//then until otherwise determined, they must be zoning
		//on a zone line.
		if(zone_mode == ZoneUnsolicited) {
			zone_point = zone->GetClosestZonePoint(GetX(), GetY(), GetZ(), target_zone_id, ZONEPOINT_ZONE_RANGE);
			//if we didnt get a zone point, or its to a different zone,
			//then we assume this is invalid.
			if(!zone_point || zone_point->target_zone_id != target_zone_id) {
				Message(13, "Invalid unsolicited zone request.");
				LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%d'.", GetName(), target_zone_id);
				if ((cheat_timer.GetRemainingTime())<1 || (!cheat_timer.Enabled())){ //Lieka:  Disable MQGate Detector if timer is active.
					CheatDetected(MQGate);
				}					
				SendZoneCancel(zc);
				return;
			}
		}
	}

//Checks if the zone can be instanced, character has a instance flag, and 
//that the character is not zoning into an instance that is already loaded
if(database.GetInstType(target_zone_id) == 1){
	int32 characterID = database.GetCharacterID(zc->char_name);
	int32 instFlagNum = database.GetCharInstFlagNum(characterID);
	if(instFlagNum >= database.GetDfltInstZFlag())
	{
			int32 charInstZOrgID = database.GetCharInstZOrgID(characterID);
			// If instance flag matches then...
			if(charInstZOrgID == target_zone_id)
			{
				//If instance zone is not in database then it is copied to database
				if(!database.InstZoneLoaded(instFlagNum)) {
					database.LoadInstZone(target_zone_id, instFlagNum);
				}
				target_zone_id = instFlagNum;
			}
	}
}

	//make sure its a valid zone.
	const char *target_zone_name = database.GetZoneName(target_zone_id);
	if(target_zone_name == NULL) {
		//invalid zone...
		Message(13, "Invalid target zone ID.");
		LogFile->write(EQEMuLog::Error, "Zoning %s: Unable to get zone name for zone id '%d'.", GetName(), target_zone_id);
		SendZoneCancel(zc);
		return;
	}

	//load up the safe coords, restrictions, and verify the zone name
	float safe_x, safe_y, safe_z;
	sint16 minstatus = 0;
	int8 minlevel = 0;
	char flag_needed[128];
	if(!database.GetSafePoints(target_zone_name, &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_needed)) {
		//invalid zone...
		Message(13, "Invalid target zone while getting safe points.");
		LogFile->write(EQEMuLog::Error, "Zoning %s: Unable to get safe coordinates for zone '%s'.", GetName(), target_zone_name);
		SendZoneCancel(zc);
		return;
	}

#ifdef EMBPERL
		char buf[10];
		snprintf(buf, 9, "%d", target_zone_id);
		buf[9] = '\0';
		((PerlembParser*)parse)->Event(EVENT_ZONE, 0, buf, (NPC*)NULL, this);
#endif

	//handle circumvention of zone restrictions
	//we need the value when creating the outgoing packet as well.
	int8 ignorerestrictions = zonesummon_ignorerestrictions;
	zonesummon_ignorerestrictions = 0;

	float dest_x=0, dest_y=0, dest_z=0, dest_h;
	dest_h = GetHeading();
	switch(zone_mode) {
	case EvacToSafeCoords:
	case ZoneToSafeCoords:
		LogFile->write(EQEMuLog::Debug, "Zoning %s to safe coords (%f,%f,%f) in %s (%d)", GetName(), safe_x, safe_y, safe_z, target_zone_name, target_zone_id);
		dest_x = safe_x;
		dest_y = safe_y;
		dest_z = safe_z;
		break;
	case GMSummon:
		dest_x = zonesummon_x;
		dest_y = zonesummon_y;
		dest_z = zonesummon_z;
		ignorerestrictions = 1;
		break;
	case GateToBindPoint:
		dest_x = m_pp.binds[0].x;
		dest_y = m_pp.binds[0].y;
		dest_z = m_pp.binds[0].z;
		break;
	case ZoneToBindPoint:
		dest_x = m_pp.binds[0].x;
		dest_y = m_pp.binds[0].y;
		dest_z = m_pp.binds[0].z;
		ignorerestrictions = 1;	//can always get to our bind point? seems exploitable
		break;
	case ZoneSolicited:  //we told the client to zone somewhere, so we know where they are going.
		//recycle zonesummon variables
		cheat_timer.Start(3500,false);
		dest_x = zonesummon_x;
		dest_y = zonesummon_y;
		dest_z = zonesummon_z;
		break;
	case ZoneUnsolicited:   //client came up with this on its own.
		//client requested a zoning... what are the cases when this could happen?
		
		//Handle zone point case:
		if(zone_point != NULL) {
			//they are zoning using a valid zone point, figure out coords
			
			//999999 is a placeholder for 'same as where they were from'
			if(zone_point->target_x == 999999)
				dest_x = GetX();
			else
				dest_x = zone_point->target_x;
			if(zone_point->target_y == 999999)
				dest_y = GetY();
			else
				dest_y = zone_point->target_y;
			if(zone_point->target_z == 999999)
				dest_z=GetZ();
			else
				dest_z = zone_point->target_z;
			if(zone_point->target_heading == 999)
				dest_h = GetHeading();
			else
				dest_h = zone_point->target_heading;
			
			break;
		}
		
		//for now, there are no other cases...
		
		//could not find a valid reason for them to be zoning, stop it.
		CheatDetected(MQZone);
		Message(13, "Invalid unsolicited zone request.");
		LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%s'. Not near a zone point.", GetName(), target_zone_name);
		SendZoneCancel(zc);
		return;
	};

	//OK, now we should know where were going...

	//Check some rules first.
	sint8 myerror = 1;		//1 is succes

	//not sure when we would use ZONE_ERROR_NOTREADY

	//enforce min status and level
	if (!ignorerestrictions && (Admin() < minstatus || GetLevel() < minlevel)) {
		cheat_timer.Start(3500,false); //Lieka:  Don't set off warp detector for when a player is moved to the safe-spot for trying to access a zone without the appropriate level or status requirements (i.e. zoning into FearPlane at level 30, etc)
		myerror = ZONE_ERROR_NOEXPERIENCE;
		}
	
	if(!ignorerestrictions && flag_needed[0] != '\0') {
		//the flag needed string is not empty, meaning a flag is required.
		if(Admin() < minStatusToIgnoreZoneFlags && !HasZoneFlag(target_zone_id)) {
			Message(13, "You must have the flag %s to enter this zone.");
			myerror = ZONE_ERROR_NOEXPERIENCE;
			cheat_timer.Start(3500,false);
		}
	}
	
	//Enforce ldon doungeon entrance rules
	if(myerror == 1 && database.IsLDoNDungeon(target_zone_id)
	// && !ignorerestrictions
	) {
		//this zone is an ldon dungeon
		int advid = GetAdventureID();
		if(advid > 0){
			//we are in an adventure... make sure its the right one
			AdventureInfo ai = database.GetAdventureInfo(advid);
			if(target_zone_id != ai.zonedungeonid) {
				Message(13, "You are not allowed to enter this dungeon!");
				LogFile->write(EQEMuLog::Error, "Zoning %s: Not allowed to enter dungeon '%s' (%d). Not in the right adventure.", GetName(), target_zone_name, target_zone_id);
				SendZoneCancel(zc);
				return;
			}
		} else {
			Message(13, "You are not allowed to enter this dungeon!");
			LogFile->write(EQEMuLog::Error, "Zoning %s: Not allowed to enter dungeon '%s' (%d). Not in any adventure.", GetName(), target_zone_name, target_zone_id);
			SendZoneCancel(zc);
			return;
		}
	}

	if(myerror == 1) {
		//we have successfully zoned
		DoZoneSuccess(zc, target_zone_id, dest_x, dest_y, dest_z, dest_h, ignorerestrictions);
	} else {
		LogFile->write(EQEMuLog::Error, "Zoning %s: Rules prevent this char from zoning into '%s'", GetName(), target_zone_name);
		SendZoneError(zc, myerror);
	}
}

void Client::SendZoneCancel(ZoneChange_Struct *zc) {
	//effectively zone them right back to where they were
	//unless we find a better way to stop the zoning process.
	EQApplicationPacket *outapp;
	cheat_timer.Start(3500,false);
	outapp = new EQApplicationPacket(OP_ZoneChange, sizeof(ZoneChange_Struct));
	ZoneChange_Struct *zc2 = (ZoneChange_Struct*)outapp->pBuffer;
	strcpy(zc2->char_name, zc->char_name);
	zc2->zoneID = zone->GetZoneID();
	zc2->success = 1;
	outapp->priority = 6;
	FastQueuePacket(&outapp);
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
}

void Client::SendZoneError(ZoneChange_Struct *zc, sint8 err) {
	cheat_timer.Start(3500,false);
	LogFile->write(EQEMuLog::Error, "Zone %i is not available because target wasn't found or character insufficent level", zc->zoneID);
	
	EQApplicationPacket *outapp;
	outapp = new EQApplicationPacket(OP_ZoneChange, sizeof(ZoneChange_Struct));
	ZoneChange_Struct *zc2 = (ZoneChange_Struct*)outapp->pBuffer;
	strcpy(zc2->char_name, zc->char_name);
	zc2->zoneID = zc->zoneID;
	zc2->success = err;
	outapp->priority = 6;
	FastQueuePacket(&outapp);
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
}

void Client::DoZoneSuccess(ZoneChange_Struct *zc, uint16 zone_id, float dest_x, float dest_y, float dest_z, float dest_h, sint8 ignore_r) {
	//this is called once the client is fully allowed to zone here
	//it takes care of all the activities which occur when a client zones out
	
	SendLogoutPackets();
	
	//dont clear aggro until the zone is successful
	entity_list.RemoveFromHateLists(this);
	
	if(this->GetPet())
		entity_list.RemoveFromHateLists(this->GetPet());
	
	LogFile->write(EQEMuLog::Status, "Zoning '%s' to: %s (%i) x=%f, y=%f, z=%f",
		m_pp.name, database.GetZoneName(zone_id), zone_id,
		dest_x, dest_y, dest_z);
	
	
	//set the player's coordinates in the new zone so they have them
	//when they zone into it
	x_pos = dest_x; //these coordinates will now be saved when ~client is called
	y_pos = dest_y;
	z_pos = dest_z;
	heading = dest_h; // Cripp: fix for zone heading
	m_pp.heading = dest_h;
	m_pp.zone_id = zone_id;
	
	//Force a save so its waiting for them when they zone
	Save();
	
	if (zone_id == zone->GetZoneID()) {
		// No need to ask worldserver if we're zoning to ourselves (most
		// likely to a bind point), also fixes a bug since the default response was failure
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZoneChange,sizeof(ZoneChange_Struct));
		ZoneChange_Struct* zc2 = (ZoneChange_Struct*) outapp->pBuffer;
		strcpy(zc2->char_name, GetName());
		zc2->zoneID = zone_id;
		zc2->success = 1;
		outapp->priority = 6;
		FastQueuePacket(&outapp);
		
		zone->StartShutdownTimer(AUTHENTICATION_TIMEOUT * 1000);
	} else {
	// vesuvias - zoneing to another zone so we need to the let the world server
	//handle things with the client for a while
		ServerPacket* pack = new ServerPacket(ServerOP_ZoneToZoneRequest, sizeof(ZoneToZone_Struct));
		ZoneToZone_Struct* ztz = (ZoneToZone_Struct*) pack->pBuffer;
		ztz->response = 0;
		ztz->current_zone_id = zone->GetZoneID();
		ztz->requested_zone_id = zone_id;
		ztz->admin = admin;
		ztz->ignorerestrictions = ignore_r;
		strcpy(ztz->name, GetName());
		ztz->guild_id = GuildID();
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
}

void Client::MovePC(const char* zonename, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(database.GetZoneID(zonename), x, y, z, heading, ignorerestrictions, zm);
}

void Client::MovePC(float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(zone->GetZoneID(), x, y, z, heading, ignorerestrictions, zm);
}

void Client::MovePC(int32 zoneID, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
}

void Client::ProcessMovePC(int32 zoneID, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm)
{
	if(zoneID == 0)
		zoneID = zone->GetZoneID();

	if(zoneID == zone->GetZoneID()) {
		// TODO: Determine if this condition is necessary.
		if(IsAIControlled()) {
			GMMove(x, y, z);
			return;
		}

#ifdef EQBOTS

		if(IsBotRaiding()) {
			BotRaids *brsummon = entity_list.GetBotRaidByMob(CastToMob());
			if(brsummon) {
				brsummon->SummonRaidBots(CastToMob(), true);
			}
		}
		else if(IsGrouped())
        {
			Group *g = GetGroup();
			if(g) {
				for(int i=0; i<MAX_GROUP_MEMBERS; i++)
				{
					if(g->members[i] && g->members[i]->IsBot())
					{
						g->members[i]->WhipeHateList();
						g->members[i]->GMMove(GetX(), GetY(), GetZ());
						if(g->members[i]->HasPet()) {
							g->members[i]->GetPet()->WhipeHateList();
							g->members[i]->GetPet()->GMMove(GetX(), GetY(), GetZ());
						}
					}
				}
			}
		}

#endif //EQBOTS

		if(GetPetID() != 0) {
			//if they have a pet and they are staying in zone, move with them
			Mob *p = GetPet();
			if(p != NULL)
				p->GMMove(x+15, y, z);	//so it dosent have to run across the map.
		}
	}

	switch(zm) {
		case GateToBindPoint:
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case GMSummon:
			Message(15, "You have been summoned by a GM!");
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case ZoneToBindPoint:
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case ZoneSolicited:
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case SummonPC:
			Message(15, "You have been summoned!");
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		case Rewind:
			Message(15, "Rewinding to previous location.");
			ZonePC(zoneID, x, y, z, heading, ignorerestrictions, zm);
			break;
		default:
			LogFile->write(EQEMuLog::Error, "Client::ProcessMovePC received a reguest to perform an unsupported client zone operation.");
			break;
	}
}

void Client::ZonePC(int32 zoneID, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	bool ReadyToZone = true;
	int iZoneNameLength = 0;
	const char*	pShortZoneName = NULL;
	char* pZoneName = NULL;

	pShortZoneName = database.GetZoneName(zoneID);
	database.GetZoneLongName(pShortZoneName, &pZoneName);
	iZoneNameLength = strlen(pZoneName);
		
	switch(zm) {
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			cheat_timer.Start(2500,false);
			x = zone->safe_x();
			y = zone->safe_y();
			z = zone->safe_z();
			heading = heading;
			break;
		case GMSummon:
			cheat_timer.Start(2500,false);
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			heading = heading;
			
			zonesummon_id = zoneID;
			zonesummon_ignorerestrictions = 1;
			break;
		case ZoneSolicited:
			cheat_timer.Start(2500,false);
			zonesummon_x = x;
			zonesummon_y = y;
			zonesummon_z = z;
			heading = heading;
			
			zonesummon_id = zoneID;
			zonesummon_ignorerestrictions = ignorerestrictions;
			break;
		case GateToBindPoint:
			cheat_timer.Start(2500,false);
			x = x_pos = m_pp.binds[0].x;
			y = y_pos = m_pp.binds[0].y;
			z = z_pos = m_pp.binds[0].z;
			heading = m_pp.binds[0].heading;
			break;
		case ZoneToBindPoint:
			cheat_timer.Start(2500,false);
			x = x_pos = m_pp.binds[0].x;
			y = y_pos = m_pp.binds[0].y;
			z = z_pos = m_pp.binds[0].z;
			heading = m_pp.binds[0].heading;
			
			zonesummon_ignorerestrictions = 1;
			LogFile->write(EQEMuLog::Debug, "Player %s has died and will be zoned to bind point in zone: %s at LOC x=%f, y=%f, z=%f, heading=%f", GetName(), pZoneName, m_pp.binds[0].x, m_pp.binds[0].y, m_pp.binds[0].z, m_pp.binds[0].heading);
			break;
		case SummonPC:
			cheat_timer.Start(2500,false);
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			heading = heading;
			break;
		case Rewind:
			LogFile->write(EQEMuLog::Debug, "%s has requested a /rewind from %f, %f, %f, to %f, %f, %f in %s", GetName(), x_pos, y_pos, z_pos, rewind_x, rewind_y, rewind_z, zone->GetShortName());
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			heading = heading;
			break;
		default:
			LogFile->write(EQEMuLog::Error, "Client::ZonePC() received a reguest to perform an unsupported client zone operation.");
			ReadyToZone = false;
			break;
	}

	if(ReadyToZone) {
		zone_mode = zm;
		
		if(zm == ZoneToBindPoint) {
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZonePlayerToBind, sizeof(ZonePlayerToBind_Struct) + iZoneNameLength);
			ZonePlayerToBind_Struct* gmg = (ZonePlayerToBind_Struct*) outapp->pBuffer;
		
			gmg->bind_zone_id = zoneID;
			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			strcpy(gmg->zone_name, pZoneName);

			outapp->priority = 6;
			FastQueuePacket(&outapp);
			safe_delete(outapp);
		}
		else if(zm == ZoneSolicited || zm == ZoneToSafeCoords) {
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
	RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;

			gmg->zone_id = zoneID;
	gmg->x = x;
	gmg->y = y;
	gmg->z = z;
	gmg->heading = heading;
	gmg->type = 0x01;				//an observed value, not sure of meaning

	outapp->priority = 6;
	FastQueuePacket(&outapp);
	safe_delete(outapp);
	}
		else if(zm == EvacToSafeCoords) {
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
			RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;

			// if we are in the same zone we want to evac to, client will not send OP_ZoneChange back to do an actual
			// zoning of the client, so we have to send a viable zoneid that the client *could* zone to to make it believe
			// we are leaving the zone, even though we are not. We have to do this because we are missing the correct op code
			// and struct that should be used for evac/succor.
			// 213 is Plane of War
			// 76 is orignial Plane of Hate
			// WildcardX 27 January 2008. Tested this for 6.2 and Titanium clients.

			if(this->GetZoneID() == 213)
				gmg->zone_id = 76;
			else if(this->GetZoneID() == 76)
				gmg->zone_id = 213;
			else
				gmg->zone_id = 213;

			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			gmg->type = 0x01;				// '0x01' was an observed value for the type field, not sure of meaning
			
			// we hide the real zoneid we want to evac/succor to here
			zonesummon_id = zoneID;

			outapp->priority = 6;
			FastQueuePacket(&outapp);
			safe_delete(outapp);
		}
		else {
			if(zoneID == this->GetZoneID()) {
		//properly handle proximities
		entity_list.ProcessMove(this, x_pos, y_pos, z_pos);
		proximity_x = x_pos;
		proximity_y = y_pos;
		proximity_z = z_pos;
		
		//send out updates to people in zone.
		SendPosition();
		
#ifdef PACKET_UPDATE_MANAGER   
		//flush our position queues because we dont know where we will end up
		update_manager.FlushQueues();
#endif
    }
	
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
		RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;
		
		gmg->zone_id = zoneID;
		gmg->x = x;
		gmg->y = y;
		gmg->z = z;
			gmg->heading = heading;
		gmg->type = 0x01;	//an observed value, not sure of meaning
		
	outapp->priority = 6;
	FastQueuePacket(&outapp);
	safe_delete(outapp);
		}

		LogFile->write(EQEMuLog::Debug, "Player %s has requested a zoning to LOC x=%f, y=%f, z=%f, heading=%f in zoneid=%i", GetName(), x, y, z, heading, zoneID);
	}
}

void Client::GoToSafeCoords(uint16 zone_id) {
	if(zone_id == 0)
		zone_id = zone->GetZoneID();

	MovePC(zone_id, 0.0f, 0.0f, 0.0f, 0.0f, 0, ZoneToSafeCoords);
}


void Mob::Gate() {
	GoToBind();
}

void Client::Gate() {
	Mob::Gate();
}

void NPC::Gate() {
	entity_list.MessageClose_StringID(this, true, 200, MT_Spells, GATES, GetCleanName());

	Mob::Gate();
}

void Client::SetBindPoint(int to_zone, float new_x, float new_y, float new_z) {
	if (to_zone == -1) {
		m_pp.binds[0].zoneId = zone->GetZoneID();
		m_pp.binds[0].x = x_pos;
		m_pp.binds[0].y = y_pos;
		m_pp.binds[0].z = z_pos;
	}
	else {
		m_pp.binds[0].zoneId = to_zone;
		m_pp.binds[0].x = new_x;
		m_pp.binds[0].y = new_y;
		m_pp.binds[0].z = new_z;
	}
}

void Client::GoToBind() {
	// move the client, which will zone them if needed.
	// ignore restrictions on the zone request..?
	MovePC(m_pp.binds[0].zoneId, 0.0f, 0.0f, 0.0f, 0.0f, 1, GateToBindPoint);
}

void Client::GoToDeath() {
	MovePC(m_pp.binds[0].zoneId, 0.0f, 0.0f, 0.0f, 0.0f, 1, ZoneToBindPoint);
}

void Client::SetZoneFlag(uint32 zone_id) {
	if(HasZoneFlag(zone_id))
		return;
	
	zone_flags.insert(zone_id);
	
	//update the DB
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	
    // Retrieve all waypoints for this grid
    if(!database.RunQuery(query,MakeAnyLenString(&query,
    	"INSERT INTO zone_flags (charID,zoneID) VALUES(%d,%d)",
    	CharacterID(),zone_id),errbuf)) {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to set zone flag for %s: %s", GetName(), errbuf);
	}
}

void Client::ClearZoneFlag(uint32 zone_id) {
	if(!HasZoneFlag(zone_id))
		return;
	
	zone_flags.erase(zone_id);
	
	//update the DB
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	
    // Retrieve all waypoints for this grid
    if(!database.RunQuery(query,MakeAnyLenString(&query,
    	"DELETE FROM zone_flags WHERE charID=%d AND zoneID=%d",
    	CharacterID(),zone_id),errbuf)) {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to clear zone flag for %s: %s", GetName(), errbuf);
	}
}

void Client::LoadZoneFlags() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	
    // Retrieve all waypoints for this grid
    if(database.RunQuery(query,MakeAnyLenString(&query,
    	"SELECT zoneID from zone_flags WHERE charID=%d",
    	CharacterID()),errbuf,&result))
    {
		while((row = mysql_fetch_row(result))) {
			zone_flags.insert(atoi(row[0]));
		}
		mysql_free_result(result);
    }
    else	// DB query error!
    {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to load zone flags for %s: %s", GetName(), errbuf);
    }
    safe_delete_array(query);
}

bool Client::HasZoneFlag(uint32 zone_id) const {
	return(zone_flags.find(zone_id) != zone_flags.end());
}

void Client::SendZoneFlagInfo(Client *to) const {
	if(zone_flags.empty()) {
		to->Message(0, "%s has no zone flags.", GetName());
		return;
	}
	
	set<uint32>::const_iterator cur, end;
	cur = zone_flags.begin();
	end = zone_flags.end();
	char empty[1] = { '\0' };
	
	to->Message(0, "Flags for %s:", GetName());
	
	for(; cur != end; cur++) {
		uint32 zoneid = *cur;
		
		const char *short_name = database.GetZoneName(zoneid);
		
		char *long_name = NULL;
		database.GetZoneLongName(short_name, &long_name);
		if(long_name == NULL)
			long_name = empty;
		
		float safe_x, safe_y, safe_z;
		sint16 minstatus = 0;
		int8 minlevel = 0;
		char flag_name[128];
		if(!database.GetSafePoints(short_name, &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_name)) {
			strcpy(flag_name, "(ERROR GETTING NAME)");
		}
		
		to->Message(0, "Has Flag %s for zone %s (%d,%s)", flag_name, long_name, zoneid, short_name);
		if(long_name != empty)
			delete[] long_name;
	}
}

bool Client::CanBeInZone() {
	//check some critial rules to see if this char needs to be booted from the zone
    //only enforce rules here which are serious enough to warrant being kicked from
    //the zone

	if(Admin() >= RuleI(GM, MinStatusToZoneAnywhere))
		return(true);
	
	float safe_x, safe_y, safe_z;
	sint16 minstatus = 0;
	int8 minlevel = 0;
	char flag_needed[128];
	if(!database.GetSafePoints(zone->GetShortName(), &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_needed)) {
		//this should not happen...
		_log(CLIENT__ERROR, "Unable to query zone info for ourself '%s'", zone->GetShortName());
		return(false);
	}
	
	if(GetLevel() < minlevel) {
		_log(CLIENT__ERROR, "Character does not meet min level requirement (%d < %d)!", GetLevel(), minlevel);
		return(false);
	}
	if(Admin() < minstatus) {
		_log(CLIENT__ERROR, "Character does not meet min status requirement (%d < %d)!", Admin(), minstatus);
		return(false);
	}
	
	if(flag_needed[0] != '\0') {
		//the flag needed string is not empty, meaning a flag is required.
		if(Admin() < minStatusToIgnoreZoneFlags && !HasZoneFlag(zone->GetZoneID())) {
			_log(CLIENT__ERROR, "Character does not have the flag to be in this zone (%s)!", flag_needed);
			return(false);
		}
	}

	return(true);
}









