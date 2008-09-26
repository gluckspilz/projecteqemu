/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2004  EQEMu Development Team (http://eqemulator.net)

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
#include "../common/types.h"
#include "entity.h"
#include "masterentity.h"
#include "../common/MiscFunctions.h"

/*

Schema:
CREATE TABLE traps (
  id int(11) NOT NULL auto_increment,
  zone varchar(16) NOT NULL default '',
  x int(11) NOT NULL default '0',
  y int(11) NOT NULL default '0',
  z int(11) NOT NULL default '0',
  chance tinyint NOT NULL default '0',
  maxzdiff float NOT NULL default '0',
  radius float NOT NULL default '0',
  effect int(11) NOT NULL default '0',
  effectvalue int(11) NOT NULL default '0',
  effectvalue2 int(11) NOT NULL default '0',
  message varcahr(200) NOT NULL;
  skill int(11) NOT NULL default '0',
  spawnchance int(11) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;


*/

Trap::Trap() :
	Entity(),
	respawn_timer(600000),
	chkarea_timer(500)
{
	trap_id = 0;
	x = 0;
	y = 0;
	z = 0;
	maxzdiff = 0;
	radius = 0;
	effect = 0;
	effectvalue = 0;
	effectvalue2 = 0;
	skill = 0;
	respawn_timer.Disable();
	detected = false;
	disarmed = true;
	spawnchance = 0;
}

Trap::~Trap()
{
}

bool Trap::Process()
{
	if (chkarea_timer.Enabled() && chkarea_timer.Check() 
		/*&& zone->GetClientCount() > 0*/ )
	{
		Mob* trigger = entity_list.GetTrapTrigger(this);
		if (trigger && !(trigger->IsClient() && trigger->CastToClient()->GetGM()))
		{
			Trigger(trigger);
		}
	}
	if (respawn_timer.Enabled() && respawn_timer.Check())
	{
		detected = false;
		disarmed = false;
		chkarea_timer.Enable();
		respawn_timer.Disable();
	}
	return true;
}

void Trap::Trigger(Mob* trigger)
{
	int i = 0;
	const NPCType* tmp = 0;
	switch (effect)
	{
		case 0:
			if(!message[0])
			{
				entity_list.MessageClose(trigger,false,100,13,"%s triggers a trap!",trigger->GetName());
			}
			else
			{
				entity_list.MessageClose(trigger,false,100,13,"%s",message.c_str());
			}

			if ((tmp = database.GetNPCType(effectvalue)))			{
				NPCType tmp2(*tmp);
				tmp2.level = trigger->GetLevel();
				NPC* new_npc = new NPC(&tmp2, 0, x, y, z, 0);
				entity_list.AddNPC(new_npc);
				new_npc->SpellFinished(effectvalue2,trigger,10,0);
				new_npc->Depop();
			}
			break;
		case 1:
			if (!message[0])
			{
				entity_list.MessageClose(trigger,false,effectvalue,13,"A loud alarm rings out through the air...");
			}
			else
			{
				entity_list.MessageClose(trigger,false,effectvalue,13,"%s",message.c_str());
			}

			entity_list.SendAlarm(this,trigger,effectvalue2);			break;
		case 2:
			if (!message[0])
			{
				entity_list.MessageClose(trigger,false,100,13,"The air shimmers...");
			}
			else
			{
				entity_list.MessageClose(trigger,false,100,13,"%s",message.c_str());
			}

			for (i = 0; i < effectvalue2; i++)
			{
				if ((tmp = database.GetNPCType(effectvalue)))
				{
					NPC* new_npc = new NPC(tmp, 0, x-5+rand()%10, y-5+rand()%10, z-5+rand()%10, rand()%250);
					new_npc->AddLootTable();
					entity_list.AddNPC(new_npc);
					new_npc->AddToHateList(trigger,1);
				}
			}
			break;
		case 3:
			if (!message[0])
			{
				entity_list.MessageClose(trigger,false,100,13,"A bandit leaps out from behind a tree!");
			}
			else
			{
				entity_list.MessageClose(trigger,false,100,13,"%s",message.c_str());
			}

			for (i = 0; i < effectvalue2; i++)
			{
				if ((tmp = database.GetNPCType(effectvalue)))
				{
					NPC* new_npc = new NPC(tmp, 0, x-2+rand()%10, y-2+rand()%4, z-2+rand()%4, rand()%250);
					new_npc->AddLootTable();
					entity_list.AddNPC(new_npc);
					new_npc->AddToHateList(trigger,1);
				}
			}
			break;
		case 4:
			if (!message[0])
			{
				entity_list.MessageClose(trigger,false,100,13,"%s triggers a trap!",trigger->GetName());
			}
			else
			{
				entity_list.MessageClose(trigger,false,100,13,"%s",message.c_str());
			}
			trigger->Message(13,"A trap hits you for %i points of damage.",effectvalue);
			trigger->SetHP(trigger->GetHP() - effectvalue);	
	}
	respawn_timer.Start(600000);
	chkarea_timer.Disable();
	disarmed = true;
}

Trap* EntityList::FindNearbyTrap(Mob* searcher, float max_dist) {
	LinkedListIterator<Trap*> iterator(trap_list);
	iterator.Reset();
	float dist = 999999;
	Trap* current_trap = NULL;
	
	float max_dist2 = max_dist*max_dist;
	Trap *cur;
	while(iterator.MoreElements())
	{
		cur = iterator.GetData();
		if(!cur->disarmed) {
			float curdist = 0;
			float tmp = searcher->GetX() - cur->x;
			curdist += tmp*tmp;
			tmp = searcher->GetY() - cur->y;
			curdist += tmp*tmp;
			tmp = searcher->GetZ() - cur->z;
			curdist += tmp*tmp;
			
			if (curdist < max_dist2 && curdist < dist)
			{
				dist = curdist;
				current_trap = cur;
			}
		}
		iterator.Advance();
	}
	return current_trap;
}

Mob* EntityList::GetTrapTrigger(Trap* trap) {
	LinkedListIterator<Client*> iterator(client_list);

	Mob* savemob = 0;
	iterator.Reset();
	
	float xdiff, ydiff, zdiff;
	
	float maxdist = trap->radius * trap->radius;
	
	while(iterator.MoreElements()) {
		Client* cur = iterator.GetData();
		zdiff = cur->GetZ() - trap->z;
		if(zdiff < 0)
			zdiff = 0 - zdiff;
		
		xdiff = cur->GetX() - trap->x;
		ydiff = cur->GetY() - trap->y;
		if ((xdiff*xdiff + ydiff*ydiff) <= maxdist
			&& zdiff < trap->maxzdiff)
		{
			if (MakeRandomInt(0,100) < trap->chance)
				return(cur);
			else
				savemob = cur;
		}
		iterator.Advance();
	}
	return savemob;
}

//todo: rewrite this to not need direct access to trap members.
bool ZoneDatabase::LoadTraps(const char* zonename) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	//	int char_num = 0;
	unsigned long* lengths;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id,x,y,z,effect,effectvalue,effectvalue2,skill,spawnchance,maxzdiff,radius,chance,message FROM traps WHERE zone='%s'", zonename), errbuf, &result)) {
		safe_delete_array(query);
		while ((row = mysql_fetch_row(result)))
		{
			if (MakeRandomInt(0,100) < atoi(row[8]))
			{
				lengths = mysql_fetch_lengths(result);
				Trap* trap = new Trap();
				trap->trap_id = atoi(row[0]);
				trap->x = atof(row[1]);
				trap->y = atof(row[2]);
				trap->z = atof(row[3]);
				trap->effect = atoi(row[4]);
				trap->effectvalue = atoi(row[5]);
				trap->effectvalue2 = atoi(row[6]);
				trap->skill = atoi(row[7]);
				trap->spawnchance = atoi(row[8]);
				trap->maxzdiff = atof(row[9]);
				trap->radius = atof(row[10]);
				trap->chance = atoi(row[11]);
				trap->message = row[12];
				entity_list.AddTrap(trap);
			}
		}
		mysql_free_result(result);
	}
	else {
		LogFile->write(EQEMuLog::Error, "Error in LoadTraps query '%s': %s", query, errbuf);
		safe_delete_array(query);
		return false;
	}

	return true;
}
