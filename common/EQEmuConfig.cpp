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
#include "../common/debug.h"
#include "EQEmuConfig.h"
#include "MiscFunctions.h"
#include <iostream>
#include <sstream>

string EQEmuConfig::ConfigFile = "eqemu_config.xml";
EQEmuConfig *EQEmuConfig::_config = NULL;

void EQEmuConfig::do_world(TiXmlElement *ele) {
	const char *text;
	TiXmlElement * sub_ele;;

	text= ParseTextBlock(ele,"shortname");
	if (text)
		ShortName=text;

	text = ParseTextBlock(ele,"longname");
	if (text)
		LongName=text;

	text = ParseTextBlock(ele,"address",true);
	if (text)
		WorldAddress=text;

	text = ParseTextBlock(ele,"localaddress",true);
	if (text)
		LocalAddress=text;

	text = ParseTextBlock(ele,"maxclients",true);
	if (text)
		MaxClients=atoi(text);

	// Get the <key> element
	text = ParseTextBlock(ele,"key",true);
	if (text)
		SharedKey=text;

	// Get the <loginserver> element
	sub_ele = ele->FirstChildElement("loginserver");
	if (sub_ele) {
		text=ParseTextBlock(sub_ele,"host",true);
		if (text)
			LoginHost=text;

		text=ParseTextBlock(sub_ele,"port",true);
		if (text)
			LoginPort=atoi(text);
		
		text=ParseTextBlock(sub_ele,"account",true);
		if (text)
			LoginAccount=text;

		text=ParseTextBlock(sub_ele,"password",true);
		if (text)
			LoginPassword=text;
	} else {
		char	str[32];
		do {
			sprintf(str, "loginserver%i", ++LoginCount);
			sub_ele = ele->FirstChildElement(str);
			if (sub_ele) {
				LoginConfig* loginconfig = new LoginConfig;
				text=ParseTextBlock(sub_ele,"host",true);
				if (text)
					loginconfig->LoginHost=text;

				text=ParseTextBlock(sub_ele,"port",true);
				if (text)
					loginconfig->LoginPort=atoi(text);

				text=ParseTextBlock(sub_ele,"account",true);
				if (text)
					loginconfig->LoginAccount=text;

				text=ParseTextBlock(sub_ele,"password",true);
				if (text)
					loginconfig->LoginPassword=text;
				loginlist.Insert(loginconfig);
			}
		} while(sub_ele);
	}
	
	// Check for locked
	sub_ele = ele->FirstChildElement("locked");
	if (sub_ele != NULL)
		Locked=true;

	// Get the <tcp> element
	sub_ele = ele->FirstChildElement("tcp");
	if(sub_ele != NULL) {

		text = sub_ele->Attribute("ip");
		if (text)
			WorldIP=text;

		text = sub_ele->Attribute("port");
		if (text)
			WorldTCPPort=atoi(text);

		text = sub_ele->Attribute("telnet");
		if (text && !strcasecmp(text,"enabled"))
			TelnetEnabled=true;

	}

	// Get the <http> element
	sub_ele = ele->FirstChildElement("http");
	if(sub_ele != NULL) {

//		text = sub_ele->Attribute("ip");
//		if (text)
//			WorldIP=text;

		text = sub_ele->Attribute("mimefile");
		if (text)
			WorldHTTPMimeFile=text;

		text = sub_ele->Attribute("port");
		if (text)
			WorldHTTPPort=atoi(text);

		text = sub_ele->Attribute("enabled");
		if (text && !strcasecmp(text,"true"))
			WorldHTTPEnabled=true;
		
	}
}

void EQEmuConfig::do_chatserver(TiXmlElement *ele) {
	const char *text;

	text=ParseTextBlock(ele,"host",true);
	if (text)
		ChatHost=text;

	text=ParseTextBlock(ele,"port",true);
	if (text)
		ChatPort=atoi(text);
}
		
void EQEmuConfig::do_mailserver(TiXmlElement *ele) {
	const char *text;

	text=ParseTextBlock(ele,"host",true);
	if (text)
		MailHost=text;

	text=ParseTextBlock(ele,"port",true);
	if (text)
		MailPort=atoi(text);
}
		
void EQEmuConfig::do_database(TiXmlElement *ele) {
	const char *text;

	text=ParseTextBlock(ele,"host",true);
	if (text)
		DatabaseHost=text;

	text=ParseTextBlock(ele,"port",true);
	if (text)
		DatabasePort=atoi(text);

	text=ParseTextBlock(ele,"username",true);
	if (text)
		DatabaseUsername=text;

	text=ParseTextBlock(ele,"password",true);
	if (text)
		DatabasePassword=text;

	text=ParseTextBlock(ele,"db",true);
	if (text)
		DatabaseDB=text;
}
		
void EQEmuConfig::do_zones(TiXmlElement *ele) {
	const char *text;
	TiXmlElement *sub_ele;
//	TiXmlNode *node,*sub_node;

	text=ParseTextBlock(ele,"defaultstatus",true);
	if (text)
		DefaultStatus=atoi(text);

	// Get the <ports> element
	sub_ele = ele->FirstChildElement("ports");
	if(sub_ele != NULL) {

		text = sub_ele->Attribute("low");
		if (text)
			ZonePortLow=atoi(text);;

		text = sub_ele->Attribute("high");
		if (text)
			ZonePortHigh=atoi(text);
	}

/*	node = ele->FirstChild("start");
	if (node) {
		string s_static="static";
		string s_dynamic="dynamic";
		sub_node=NULL;
		while( (sub_node = node->IterateChildren( sub_node )) ) {
			if(sub_node->Type() != TiXmlNode::ELEMENT)
				continue;	//skip crap we dont care about

			TiXmlElement *sub_ele = (TiXmlElement *) sub_node;

			if (s_static == sub_ele->Value()) {
				const char *zone = MakeLowerString(sub_ele->Attribute("zone"));
				const char *port = sub_ele->Attribute("port");
				if (zone) {
					StaticZones[zone] = (port) ? atoi(port) : 0;
				}
			} else if (s_dynamic == sub_ele->Value()) {
				const char *count = sub_ele->Attribute("count");
				if (count) {
					DynamicCount=atoi(count);
				}
			}
		}
	}*/
}

void EQEmuConfig::do_files(TiXmlElement *ele) {
	const char *text;

	text=ParseTextBlock(ele,"spells",true);
	if (text)
		SpellsFile=text;

	text=ParseTextBlock(ele,"opcodes",true);
	if (text)
		OpCodesFile=text;

	text=ParseTextBlock(ele,"logsettings",true);
	if (text)
		LogSettingsFile=text;

	text=ParseTextBlock(ele,"eqtime",true);
	if (text)
		EQTimeFile=text;
}

void EQEmuConfig::do_directories(TiXmlElement *ele) {
	const char *text;

	text=ParseTextBlock(ele,"maps",true);
	if (text)
		MapDir=text;

	text=ParseTextBlock(ele,"quests",true);
	if (text)
		QuestDir=text;

	text=ParseTextBlock(ele,"plugins",true);
	if (text)
		PluginDir=text;

}

void EQEmuConfig::do_launcher(TiXmlElement *ele) {
	const char *text;
	TiXmlElement *sub_ele;

	text=ParseTextBlock(ele,"logprefix",true);
	if (text)
		LogPrefix = text;

	text=ParseTextBlock(ele,"logsuffix",true);
	if (text)
		LogSuffix = text;

	// Get the <exe> element
	text = ParseTextBlock(ele,"exe",true);
	if (text)
		ZoneExe = text;

	// Get the <timers> element
	sub_ele = ele->FirstChildElement("timers");
	if(sub_ele != NULL) {
		text = sub_ele->Attribute("restart");
		if (text)
			RestartWait = atoi(text);

		text = sub_ele->Attribute("reterminate");
		if (text)
			TerminateWait = atoi(text);

		text = sub_ele->Attribute("initial");
		if (text)
			InitialBootWait = atoi(text);

		text = sub_ele->Attribute("interval");
		if (text)
			ZoneBootInterval = atoi(text);
	}
}

string EQEmuConfig::GetByName(const string &var_name) const {
	if(var_name == "ShortName")
		return(ShortName);
	if(var_name == "LongName")
		return(LongName);
	if(var_name == "WorldAddress")
		return(WorldAddress);
	if(var_name == "LoginHost")
		return(LoginHost);
	if(var_name == "LoginAccount")
		return(LoginAccount);
	if(var_name == "LoginPassword")
		return(LoginPassword);
	if(var_name == "LoginPort")
		return(itoa(LoginPort));
	if(var_name == "Locked")
		return(Locked?"true":"false");
	if(var_name == "WorldTCPPort")
		return(itoa(WorldTCPPort));
	if(var_name == "WorldIP")
		return(WorldIP);
	if(var_name == "TelnetEnabled")
		return(TelnetEnabled?"true":"false");
	if(var_name == "WorldHTTPPort")
		return(itoa(WorldHTTPPort));
	if(var_name == "WorldHTTPMimeFile")
		return(WorldHTTPMimeFile);
	if(var_name == "WorldHTTPEnabled")
		return(WorldHTTPEnabled?"true":"false");
	if(var_name == "ChatHost")
		return(ChatHost);
	if(var_name == "ChatPort")
		return(itoa(ChatPort));
	if(var_name == "MailHost")
		return(MailHost);
	if(var_name == "MailPort")
		return(itoa(MailPort));
	if(var_name == "DatabaseHost")
		return(DatabaseHost);
	if(var_name == "DatabaseUsername")
		return(DatabaseUsername);
	if(var_name == "DatabasePassword")
		return(DatabasePassword);
	if(var_name == "DatabaseDB")
		return(DatabaseDB);
	if(var_name == "DatabasePort")
		return(itoa(DatabasePort));
	if(var_name == "SpellsFile")
		return(SpellsFile);
	if(var_name == "OpCodesFile")
		return(OpCodesFile);
	if(var_name == "EQTimeFile")
		return(EQTimeFile);
	if(var_name == "LogSettingsFile")
		return(LogSettingsFile);
	if(var_name == "MapDir")
		return(MapDir);
	if(var_name == "QuestDir")
		return(QuestDir);
	if(var_name == "PluginDir")
		return(PluginDir);
	if(var_name == "LogPrefix")
		return(LogPrefix);
	if(var_name == "LogSuffix")
		return(LogSuffix);
	if(var_name == "ZoneExe")
		return(ZoneExe);
	if(var_name == "ZonePortLow")
		return(itoa(ZonePortLow));
	if(var_name == "ZonePortHigh")
		return(itoa(ZonePortHigh));
	if(var_name == "DefaultStatus")
		return(itoa(DefaultStatus));
//	if(var_name == "DynamicCount")
//		return(itoa(DynamicCount));
	return("");
}

void EQEmuConfig::Dump() const
{
	cout << "ShortName = " << ShortName << endl;
	cout << "LongName = " << LongName << endl;
	cout << "WorldAddress = " << WorldAddress << endl;
	cout << "LoginHost = " << LoginHost << endl;
	cout << "LoginAccount = " << LoginAccount << endl;
	cout << "LoginPassword = " << LoginPassword << endl;
	cout << "LoginPort = " << LoginPort << endl;
	cout << "Locked = " << Locked << endl;
	cout << "WorldTCPPort = " << WorldTCPPort << endl;
	cout << "WorldIP = " << WorldIP << endl;
	cout << "TelnetEnabled = " << TelnetEnabled << endl;
	cout << "WorldHTTPPort = " << WorldHTTPPort << endl;
	cout << "WorldHTTPMimeFile = " << WorldHTTPMimeFile << endl;
	cout << "WorldHTTPEnabled = " << WorldHTTPEnabled << endl;
	cout << "ChatHost = " << ChatHost << endl;
	cout << "ChatPort = " << ChatPort << endl;
	cout << "MailHost = " << MailHost << endl;
	cout << "MailPort = " << MailPort << endl;
	cout << "DatabaseHost = " << DatabaseHost << endl;
	cout << "DatabaseUsername = " << DatabaseUsername << endl;
	cout << "DatabasePassword = " << DatabasePassword << endl;
	cout << "DatabaseDB = " << DatabaseDB << endl;
	cout << "DatabasePort = " << DatabasePort << endl;
	cout << "SpellsFile = " << SpellsFile << endl;
	cout << "OpCodesFile = " << OpCodesFile << endl;
	cout << "EQTimeFile = " << EQTimeFile << endl;
	cout << "LogSettingsFile = " << LogSettingsFile << endl;
	cout << "MapDir = " << MapDir << endl;
	cout << "QuestDir = " << QuestDir << endl;
	cout << "PluginDir = " << PluginDir << endl;
	cout << "ZonePortLow = " << ZonePortLow << endl;
	cout << "ZonePortHigh = " << ZonePortHigh << endl;
	cout << "DefaultStatus = " << (int)DefaultStatus << endl;
//	cout << "DynamicCount = " << DynamicCount << endl;
}

























