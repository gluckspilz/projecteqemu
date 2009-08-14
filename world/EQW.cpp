/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2006  EQEMu Development Team (http://eqemulator.net)

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

#ifdef EMBPERL

#include "../common/debug.h"
#include "EQW.h"
#include "EQWParser.h"
#include "WorldConfig.h"
#include "../common/races.h"
#include "../common/classes.h"
#include "../common/misc.h"
#include "../common/MiscFunctions.h"
#include "zoneserver.h"
#include "zonelist.h"
#include "clientlist.h"
#include "cliententry.h"
#include "LoginServer.h"
#include "worlddb.h"
#include "client.h"
#include "LauncherList.h"
#include "LauncherLink.h"
#include "wguild_mgr.h"

#include <algorithm>

using namespace std;

extern ZSList	zoneserver_list;
extern ClientList	client_list;
extern uint32	numzones;
extern LoginServer loginserver;
extern LauncherList launcher_list;
extern volatile bool	RunLoops;




EQW EQW::s_EQW;

//IO Capture routine
XS(XS_EQWIO_PRINT); /* prototype to pass -Wmissing-prototypes */ 
XS(XS_EQWIO_PRINT) 
{
    dXSARGS; 
    if (items < 2)
    	return;

	int r;
	for(r = 1; r < items; r++) {
		char *str = SvPV_nolen(ST(r));
		EQW::Singleton()->AppendOutput(str);
 	}
 	
    XSRETURN_EMPTY; 
}

EQW::EQW() {
}

void EQW::AppendOutput(const char *str) {
	m_outputBuffer += str;
//	_log(WORLD__EQW, "Append %d chars, yeilding result of length %d", strlen(str), m_outputBuffer.length());
}

const std::string &EQW::GetOutput() const {
//	_log(WORLD__EQW, "Getting, length %d", m_outputBuffer.length());
	return(m_outputBuffer);
}

void EQW::LockWorld() {
	WorldConfig::LockWorld();
	if (loginserver.Connected()) {
		loginserver.SendStatus();
	}
}

void EQW::UnlockWorld() {
	WorldConfig::UnlockWorld();
	if (loginserver.Connected()) {
		loginserver.SendStatus();
	}
}

Const_char *EQW::GetConfig(Const_char *var_name) {
	m_returnBuffer = WorldConfig::get()->GetByName(var_name);
	return(m_returnBuffer.c_str());
}

bool EQW::LSConnected() {
	return(loginserver.Connected());
}

int EQW::CountZones() {
	return(zoneserver_list.GetZoneCount());
}

//returns an array of zone_refs (opaque)
vector<string> EQW::ListBootedZones() {
	vector<string> res;
	
	vector<int32> zones;
	zoneserver_list.GetZoneIDList(zones);
	
	vector<int32>::iterator cur, end;
	cur = zones.begin();
	end = zones.end();
	for(; cur != end; cur++) {
		res.push_back(itoa(*cur));
	}
	
	return(res);
}

map<string,string> EQW::GetZoneDetails(Const_char *zone_ref) {
	map<string,string> res;
	
	ZoneServer *zs = zoneserver_list.FindByID(atoi(zone_ref));
	if(zs == NULL) {
		res["error"] = "Invalid zone.";
		return(res);
	}
	
	res["type"] = zs->IsStaticZone()?"static":"dynamic";
	res["zone_id"] = itoa(zs->GetZoneID());
	res["launch_name"] = zs->GetLaunchName();
	res["launched_name"] = zs->GetLaunchedName();
	res["short_name"] = zs->GetZoneName();
	res["long_name"] = zs->GetZoneLongName();
	res["port"] = itoa(zs->GetCPort());
	res["player_count"] = itoa(zs->NumPlayers());
	
	//this isnt gunna work for dynamic zones...
	res["launcher"] = "";
	if(zs->GetZoneID() != 0) {
		LauncherLink *ll = launcher_list.FindByZone(zs->GetLaunchName());
		if(ll != NULL)
			res["launcher"] = ll->GetName();
	}
	
	return(res);
}

int EQW::CountPlayers() {
	return(client_list.GetClientCount());
}

//returns an array of character names in the zone (empty=all zones)
vector<string> EQW::ListPlayers(Const_char *zone_name) {
	vector<string> res;
	
	vector<ClientListEntry *> list;
	client_list.GetClients(zone_name, list);
	
	vector<ClientListEntry *>::iterator cur, end;
	cur = list.begin();
	end = list.end();
	for(; cur != end; cur++) {
		res.push_back((*cur)->name());
	}
	return(res);
}

map<string,string> EQW::GetPlayerDetails(Const_char *char_name) {
	map<string,string> res;
	
	ClientListEntry *cle = client_list.FindCharacter(char_name);
	if(cle == NULL) {
		res["error"] = "1";
		return(res);
	}
	
	res["character"] = cle->name();
	res["account"] = cle->AccountName();
	res["account_id"] = itoa(cle->AccountID());
	res["location_short"] = cle->zone()?database.GetZoneName(cle->zone()):"No Zone";
	res["location_long"] = res["location_short"];
	res["location_id"] = itoa(cle->zone());
	res["ip"] = long2ip(cle->GetIP());
	res["level"] = itoa(cle->level());
	res["race"] = GetRaceName(cle->race());
	res["race_id"] = itoa(cle->race());
	res["class"] = GetEQClassName(cle->class_());
	res["class_id"] = itoa(cle->class_());
	res["guild_id"] = itoa(cle->GuildID());
	res["guild"] = guild_mgr.GetGuildName(cle->GuildID());
	res["status"] = itoa(cle->Admin());
//	res["patch"] = cle->DescribePatch();
	
	return(res);
}

int EQW::CountLaunchers(bool active_only) {
	if(active_only)
		return(launcher_list.GetLauncherCount());
	
	vector<string> it(EQW::ListLaunchers());
	return(it.size());
}
/*
vector<string> EQW::ListActiveLaunchers() {
	vector<string> launchers;
	launcher_list.GetLauncherNameList(launchers);
	return(launchers);
}*/

vector<string> EQW::ListLaunchers() {
//	vector<string> list;
//	database.GetLauncherList(list);
	vector<string> launchers;
	launcher_list.GetLauncherNameList(launchers);
	return(launchers);
	
/*	if(list.empty()) {
		return(launchers);
	} else if(launchers.empty()) {
		return(list);
	}
	
	//union the two lists.
	vector<string>::iterator curo, endo, curi, endi;
	curo = list.begin();
	endo = list.end();
	for(; curo != endo; curo++) {
		bool found = false;
		curi = launchers.begin();
		endi = launchers.end();
		for(; curi != endi; curi++) {
			if(*curo == *curi) {
				found = true;
				break;
			}
		}
		if(found)
			break;
		launchers.push_back(*curo);
	}
	return(launchers);*/
}

EQLConfig * EQW::GetLauncher(Const_char *launcher_name) {
	return(launcher_list.GetConfig(launcher_name));
}

void EQW::CreateLauncher(Const_char *launcher_name, int dynamic_count) {
	launcher_list.CreateLauncher(launcher_name, dynamic_count);
}

void EQW::LSReconnect() {
	#ifdef WIN32
		_beginthread(AutoInitLoginServer, 0, NULL);
	#else
		pthread_t thread;
		pthread_create(&thread, NULL, &AutoInitLoginServer, NULL);
	#endif
	RunLoops = true;
	_log(WORLD__CONSOLE,"Login Server Reconnect manually restarted by Web Tool");
}

/*EQLConfig * EQW::FindLauncher(Const_char *zone_ref) {
	return(NULL);
}*/

/*
map<string,string> EQW::GetLaunchersDetails(Const_char *launcher_name) {
	map<string,string> res;
	
	LauncherLink *ll = launcher_list.Get(launcher_name);
	if(ll == NULL) {
		res["name"] = launcher_name;
		res["ip"] = "Not Connected";
		res["id"] = "0";
		res["zone_count"] = "0";
		res["connected"] = "no";
		return(res);
	} else {
		res["name"] = ll->GetName();
		res["ip"] = long2ip(ll->GetIP());
		res["id"] = itoa(ll->GetID());
		res["zone_count"] = itoa(ll->CountZones());
		res["connected"] = "yes";
	}
	
	return(res);
}

vector<string> EQW::ListLauncherZones(Const_char *launcher_name) {
	vector<string> list;
	LauncherLink *ll = launcher_list.Get(launcher_name);
	if(ll != NULL) {
		ll->GetZoneList(list);
	}
	return(list);
}

map<string,string> EQW::GetLauncherZoneDetails(Const_char *launcher_name, Const_char *zone_ref) {
	map<string,string> res;
	LauncherLink *ll = launcher_list.Get(launcher_name);
	if(ll != NULL) {
		ll->GetZoneDetails(zone_ref, res);
	} else {
		res["error"] = "Launcher Not Found";
	}
	return(res);
}

void EQW::CreateLauncher(Const_char *launcher_name, int dynamic_count) {
}

bool EQW::BootStaticZone(Const_char *launcher_name, Const_char *short_name) {
	return(false);
}

bool EQW::DeleteStaticZone(Const_char *launcher_name, Const_char *short_name) {
	return(false);
}

bool EQW::SetDynamicCount(Const_char *launcher_name, int count) {
	return(false);
}

int  EQW::GetDynamicCount(Const_char *launcher_name) {
	return(0);
}
*/

int32 EQW::CreateGuild(const char* name, int32 leader_char_id) {
	int32 id = guild_mgr.CreateGuild(name, leader_char_id);
	if(id != GUILD_NONE)
		client_list.UpdateClientGuild(leader_char_id, id);
	return(id);
}

bool EQW::DeleteGuild(int32 guild_id) {
	return(guild_mgr.DeleteGuild(guild_id));
}

bool EQW::RenameGuild(int32 guild_id, const char* name) {
	return(guild_mgr.RenameGuild(guild_id, name));
}

bool EQW::SetGuildMOTD(int32 guild_id, const char* motd, const char *setter) {
	return(guild_mgr.SetGuildMOTD(guild_id, motd, setter));
}

bool EQW::SetGuildLeader(int32 guild_id, int32 leader_char_id) {
	return(guild_mgr.SetGuildLeader(guild_id, leader_char_id));
}

bool EQW::SetGuild(int32 charid, int32 guild_id, int8 rank) {
	client_list.UpdateClientGuild(charid, guild_id);
	return(guild_mgr.SetGuild(charid, guild_id, rank));
}

bool EQW::SetGuildRank(int32 charid, int8 rank) {
	return(guild_mgr.SetGuildRank(charid, rank));
}

bool EQW::SetBankerFlag(int32 charid, bool is_banker) {
	return(guild_mgr.SetBankerFlag(charid, is_banker));
}

bool EQW::SetTributeFlag(int32 charid, bool enabled) {
	return(guild_mgr.SetTributeFlag(charid, enabled));
}

bool EQW::SetPublicNote(int32 charid, const char *note) {
	return(guild_mgr.SetPublicNote(charid, note));
}



#endif //EMBPERL
















