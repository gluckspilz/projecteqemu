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

#ifdef EQBOTS

#include "../common/debug.h"
#include "botRaids.h"
#include "masterentity.h"
#include "groups.h"

BotRaids::BotRaids(Mob *leader)
{
	if(!leader->IsGrouped())
	{
		if(leader->IsClient()) {
			leader->Message(15, "You can't create a raid (not grouped or your group isn't full)");
		}
	}
	else if(leader->IsClient()) {
        memset(BotRaidGroups, 0, sizeof(BotRaidGroups));
		entity_list.AddBotRaid(this);
		botrleader = leader;
		botrleader->SetBotRaiding(true);
		botrleader->SetBotRaidID(GetBotRaidID());
		botmaintank = NULL;
		botsecondtank = NULL;
		botraidmaintarget = NULL;
		botraidsecondtarget = NULL;
		botgroup1target = NULL;
		botgroup2target = NULL;
		botgroup3target = NULL;
		botgroup4target = NULL;
		botgroup5target = NULL;
		botgroup6target = NULL;
		botgroup7target = NULL;
		botgroup8target = NULL;
		botgroup9target = NULL;
		botgroup10target = NULL;
		botgroup11target = NULL;
		botgroup12target = NULL;
		abotAttack = 1;
		leader->Message(15, "Your raid has been created.\n");
	}
}

bool BotRaids::AddBotGroup(Group *gtoadd) {
	int i=0;
	//see if they are allready in the group
	for(i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i] && !strcasecmp(BotRaidGroups[i]->GetLeaderName(), gtoadd->GetLeaderName()))
			return false;
	}
	//put them in the group
	for(i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(!BotRaidGroups[i]) {
			BotRaidGroups[i] = gtoadd;
			break;
		}
	}
	return true;
}

void BotRaids::RemoveBotGroup(Group *delgroup) {
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			if(BotRaidGroups[i] == delgroup) {
				for(int j=5; j>=0; j--) {
					if(delgroup->members[j] && delgroup->members[j]->IsBot()) {
						delgroup->members[j]->BotOwner = NULL;
						delgroup->members[j]->Kill();
					}
				}
				BotRaidGroups[i] = NULL;
				int k = i+1;
				for(; k<MAX_BOT_RAID_GROUPS; k++) {
					if(BotRaidGroups[k]) {
						BotRaidGroups[k-1] = BotRaidGroups[k];
						BotRaidGroups[k] = NULL;
					}
				}
				break;
			}
		}
	}
}

void BotRaids::RemoveRaidBots() {
	for(int i=11; i>=0; i--) {
		if(BotRaidGroups[i]) {
			for(int j=5; j>=0; j--) {
				if(BotRaidGroups[i]) {
					if(BotRaidGroups[i]->members[j]) {
						if(BotRaidGroups[i]->members[j]->IsBot()) {
							BotRaidGroups[i]->members[j]->BotOwner = NULL;
							BotRaidGroups[i]->members[j]->Kill();
						}
						else if(BotRaidGroups[i]->members[j]->IsClient()) {
							BotRaidGroups[i]->members[j]->SetBotRaidID(0);
							BotRaidGroups[i]->members[j]->SetBotRaiding(false);
							if(BotRaidGroups[i]->BotGroupCount() < 2) {
								BotRaidGroups[i]->members[j]->SetGrouped(false);
							}
						}
					}
				}
			}
		}
	}
}

int32 BotRaids::RaidBotGroupsCount() {
	int count = 0;
	for(int i=count; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			count++;
		}
	}
	return count;
}

bool BotRaids::RemoveEmptyBotGroup() {
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		Group *g = BotRaidGroups[i];
		if(g) {
			if(g->BotGroupCount() == 0) {
				BotRaidGroups[i] = 0x00000000;
				int j = i+1;
				for(; j<MAX_BOT_RAID_GROUPS; j++) {
					if(BotRaidGroups[j]) {
						BotRaidGroups[j-1] = BotRaidGroups[j];
						BotRaidGroups[j] = 0x00000000;
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool BotRaids::RemoveClientGroup(Mob *m) {
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		Group *g = BotRaidGroups[i];
		if(g) {
			if(g->GetLeader() == m) {
				if(BotRaidGroups[i]->BotGroupCount() == 1) {
					m->SetBotRaidID(0);
					m->SetBotRaiding(false);
					m->SetGrouped(false);
					entity_list.RemoveGroup(BotRaidGroups[i]->GetID());
				}
				else {
					for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
						if(g->members[j]) {
							g->members[j]->SetBotRaidID(0);
							g->members[j]->SetBotRaiding(false);
						}
					}
				}
				BotRaidGroups[i] = 0x00000000;
				return true;
			}
		}
	}
	return false;
}

bool BotRaids::IsBotGroupInRaid(Group *gtocheck) {
	if(GetBotRaidID() == gtocheck->GetLeader()->GetBotRaidID())
		return true;
	else
		return false;
}

void BotRaids::DisbandBotRaid() {	
	entity_list.RemoveBotRaid(GetBotRaidID());
}

bool BotRaids::IsBotRaidMember(Mob* mob) {
	if(GetBotRaidID() == mob->GetBotRaidID())
		return true;
	else
		return false;
}

bool BotRaids::BotRaidProcess() {
	// TODO
	return true;
}

void BotRaids::AddBotRaidAggro(Mob *m) {
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(BotRaidGroups[i]->members[j]->IsBot()) {
						BotRaidGroups[i]->members[j]->AddToHateList(m, 1);
					}
				}
			}
		}
	}
}

bool BotRaids::GetBotRaidAggro() {
	bool gotAggro = false;
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(BotRaidGroups[i]->members[j]->IsEngaged()) {
						gotAggro = true;
						break;
					}
					if(BotRaidGroups[i]->members[j] && BotRaidGroups[i]->members[j]->GetPetID()) {
						if(BotRaidGroups[i]->members[j]->GetPet()->IsEngaged()) {
							gotAggro = true;
							break;
						}
					}
				}
			}
		}
	}
	return gotAggro;
}

void BotRaids::FollowGuardCmd(Client *c, bool isGuard) {
	int32 followingID = 0;
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(isGuard) {
						if(BotRaidGroups[i]->members[j]->IsBot()) {
							BotRaidGroups[i]->members[j]->SetFollowID(0);
						}
					}
					else {
						if(followingID == 0) {
							if(BotRaidGroups[i]->members[j]->IsBot()) {
								followingID = BotRaidGroups[i]->members[j]->GetID();
								BotRaidGroups[i]->members[j]->SetFollowID(c->GetID());
							}
						}
						else {
							if(BotRaidGroups[i]->members[j]->IsBot()) {
								BotRaidGroups[i]->members[j]->SetFollowID(followingID);
							}
						}
					}
				}
			}
		}
	}
}

void BotRaids::SummonRaidBots(Mob *m, bool wipe_hate) {
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(BotRaidGroups[i]->members[j]->IsBot()) {
						if(wipe_hate) {
							BotRaidGroups[i]->members[j]->WipeHateList();
						}
						BotRaidGroups[i]->members[j]->SetTarget(m);
						BotRaidGroups[i]->members[j]->Warp(m->GetX(), m->GetY(), m->GetZ());
						if(BotRaidGroups[i]->members[j] && BotRaidGroups[i]->members[j]->GetPetID()) {
							if(wipe_hate) {
								BotRaidGroups[i]->members[j]->GetPet()->WipeHateList();
							}
							BotRaidGroups[i]->members[j]->GetPet()->SetTarget(BotRaidGroups[i]->members[j]);
							BotRaidGroups[i]->members[j]->GetPet()->Warp(m->GetX(), m->GetY(), m->GetZ());
						}
					}
				}
			}
		}
	}
}

void BotRaids::SendBotRaidMessage(Mob *sender, const char *message) {
	if(sender == NULL || !sender->IsGrouped() || message == NULL)
		return;

	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(BotRaidGroups[i]->members[j]->IsClient()) {
						BotRaidGroups[i]->members[j]->CastToClient()->ChannelMessageSend(sender->GetName(), BotRaidGroups[i]->members[j]->GetName(), 2, 0, message);
					}
				}
			}
		}
	}
}

void BotRaids::SetBotRaidLeader(Mob *rrleader) {
	botrleader = rrleader;
}

Mob* BotRaids::GetRaidBotLeader() {
	if(botrleader == NULL)
		return NULL;
	else
		return botrleader;
}

void BotRaids::SetBotMainTank(Mob *mtank) {
	if(mtank && IsBotRaidMember(mtank) ) {
		botmaintank = mtank;
	}
	else {
		botmaintank = NULL;
	}
}

void BotRaids::SetBotSecondTank(Mob *stank) {
	if(stank != NULL && IsBotRaidMember(stank)) {
		botsecondtank = stank;
	}
	else {
		botsecondtank=NULL;
	}
}

void BotRaids::SetBotMainTarget(Mob *target) {
	if(target && !IsBotRaidMember(target))
		botraidmaintarget = target;
	else
		botraidmaintarget = NULL;
}

void BotRaids::SetBotSecondTarget(Mob *starget) {
	if(starget && !IsBotRaidMember(starget))
		botraidsecondtarget = starget;
}

void BotRaids::SetBotGroupTarget(Mob *target, Group *group) {
	if(target && !IsBotRaidMember(target) && group) {
		Mob *gleader = group->GetLeader();
		gleader->CastToNPC()->SetTarget(target);
	}
}

void BotRaids::GroupAssignTask(Group *g, int iTask, Mob *m) {
	if(g == NULL || !g->IsGroup() || iTask < 0 || iTask > 5)
		return;

	if((iTask == 2) && (m != NULL) && !m->IsBot()) {
		Mob *gleader = g->GetLeader();
		if(gleader->IsBot()) {
			gleader->SetFollowID(gleader->BotOwner->GetID());
			gleader->WipeHateList();
			gleader->Say("Attacking %s", m->GetCleanName());
			gleader->AddToHateList(m, 2000, 2000, false);
			for(int i=0; i<MAX_GROUP_MEMBERS; i++) {
				if(g->members[i] && (g->members[i] != gleader)) {
					g->members[i]->SetFollowID(gleader->GetID());
					g->members[i]->WipeHateList();
					g->members[i]->Say("Attacking %s", m->GetCleanName());
					g->members[i]->AddToHateList(m, 2000, 2000, false);
				}
			}
		}
		else {
			gleader->Message(15, "Only bot groups can offtank.");
		}
	}
	// Guard
	else if(iTask == 4) {
		for(int i=0; i<MAX_GROUP_MEMBERS; i++) {
			if(g->members[i]) {
				g->members[i]->SetFollowID(0);
                g->members[i]->WipeHateList();
				g->members[i]->Say("Guarding here.");
			}
		}
	}
}

void BotRaids::SplitExp(uint32 exp, Mob* other)
{
	if(other->CastToNPC()->MerchantType != 0) // Ensure NPC isn't a merchant
	{
		return;
	}

	if(other->GetOwner() && other->GetOwner()->IsBot()) // Ensure owner isn't a bot
	{
		return;
	}

	if(other->GetOwner() && other->GetOwner()->IsClient()) // Ensure owner isn't pc
	{
		return;
	}

	uint32 groupexp = exp; 
	int8 membercount = 0; 
	int8 maxlevel = 1;

	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++)
	{
		if(BotRaidGroups[i])
		{
			for(int j=0; j<MAX_GROUP_MEMBERS; j++)
			{
				if(BotRaidGroups[i]->members[j])
				{
					if(BotRaidGroups[i]->members[j]->GetLevel() > maxlevel)
					{
						maxlevel = BotRaidGroups[i]->members[j]->GetLevel();
					}
					membercount++;
				}
			}
		}
	}

	groupexp = (uint32)((float)groupexp * (1.0f-(RuleR(Character, RaidExpMultiplier))));

	int conlevel = Mob::GetLevelCon(maxlevel, other->GetLevel());
	if(conlevel == CON_GREEN)
	{
		return;	//no exp for greenies...
	}

	if(membercount == 0)
	{
		return;
	}

	for(int x=0; x<MAX_GROUP_MEMBERS; x++)
	{
		if(BotRaidGroups[0]->members[x] != NULL) // If Group Member is Client
		{
			if(BotRaidGroups[0]->members[x]->IsClient())
			{
				Client *cmember = BotRaidGroups[0]->members[x]->CastToClient();
				// add exp + exp cap 
				sint16 diff = cmember->GetLevel() - maxlevel;
				sint16 maxdiff = -(cmember->GetLevel()*15/10 - cmember->GetLevel());
				if(maxdiff > -5)
				{
					maxdiff = -5;
				}
				if(diff >= maxdiff) /*Instead of person who killed the mob, the person who has the highest level in the group*/
				{
					uint32 tmp = (cmember->GetLevel() + 3) * (cmember->GetLevel() + 3) * 75 * 35 / 10;
					uint32 tmp2 = (groupexp / membercount) + 1;
					cmember->AddEXP( tmp < tmp2 ? tmp : tmp2, conlevel ); 
				}
			}
		} 
	}
}

void BotRaids::RaidDefendEnraged() {
	for(int j=0; j<MAX_BOT_RAID_GROUPS; j++) {
		if(BotRaidGroups[j]) {
			for(int i=0; i<MAX_GROUP_MEMBERS; i++) {
				if(BotRaidGroups[j]->members[i] && BotRaidGroups[j]->members[i]->IsBot()) {
					BotRaidGroups[j]->members[i]->Say("Enraged... stopping attacks.");
					BotRaidGroups[j]->members[i]->SetFollowID(0);
					BotRaidGroups[j]->members[i]->WipeHateList();
					if(BotRaidGroups[j]->members[i] && BotRaidGroups[j]->members[i]->GetPetID()) {
						BotRaidGroups[j]->members[i]->GetPet()->WipeHateList();
					}
				}
			}
		}
	}
}

// g1 follow g2
// or g1 follow g2 and assist g2
void BotRaids::GroupAssignTask(Group *g, int iTask, Group *g2) {
	if(g == NULL || g2 == NULL || !g->IsGroup() || !g2->IsGroup() )
		return;

	Mob *gleader1 = g->GetLeader();
	Mob *gleader2 = g2->GetLeader();

	if(!gleader1 || !gleader2)
		return;

	if(iTask == 1) {
		gleader1->SetFollowID(gleader2->GetID());
        gleader1->WipeHateList();
		gleader1->Say("Following %s", gleader2->GetName());
		for(int i=0; i<MAX_GROUP_MEMBERS; i++) {
			if(g->members[i] && (g->members[i] != gleader1)) {
				g->members[i]->SetFollowID(gleader1->GetID());
                g->members[i]->WipeHateList();
				g->members[i]->Say("Following %s", gleader2->GetName());
			}
		}
	}
	else if(iTask == 3)
	{
		if(gleader2->GetTarget() == NULL)
		{
			return;
		}
		else
		{
			if(gleader2->BotOwner)
			{
				//break invis when you attack
				if(gleader2->BotOwner->invisible) {
					gleader2->BotOwner->BuffFadeByEffect(SE_Invisibility);
					gleader2->BotOwner->BuffFadeByEffect(SE_Invisibility2);
					gleader2->BotOwner->invisible = false;
				}
				if(gleader2->BotOwner->invisible_undead) {
					gleader2->BotOwner->BuffFadeByEffect(SE_InvisVsUndead);
					gleader2->BotOwner->BuffFadeByEffect(SE_InvisVsUndead2);
					gleader2->BotOwner->invisible_undead = false;
				}
				if(gleader2->BotOwner->invisible_animals){
					gleader2->BotOwner->BuffFadeByEffect(SE_InvisVsAnimals);
					gleader2->BotOwner->invisible_animals = false;
				}
				if(gleader2->BotOwner->hidden || gleader2->BotOwner->improved_hidden){
					gleader2->BotOwner->hidden = false;
					gleader2->BotOwner->improved_hidden = false;
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_SpawnAppearance, sizeof(SpawnAppearance_Struct));
					SpawnAppearance_Struct* sa_out = (SpawnAppearance_Struct*)outapp->pBuffer;
					sa_out->spawn_id = gleader2->BotOwner->GetID();
					sa_out->type = 0x03;
					sa_out->parameter = 0;
					entity_list.QueueClients(gleader2->BotOwner, outapp, true);
					safe_delete(outapp);
				}
			}

			gleader1->CastToNPC()->SetTarget(gleader2->GetTarget());
			this->SetAttackBotRaidRights(1);
			gleader1->Say("Assisting %s", gleader2->GetName());
		}
	}
}

Mob* BotRaids::GetBotMainTank() {
	if(!botmaintank || (botmaintank->AmIaBot && botmaintank->qglobal))
		return NULL;

	if(entity_list.GetMob(botmaintank->GetName()))
		return botmaintank;
	else
		return NULL;
}

Mob* BotRaids::GetBotSecondTank() {
	if(!botsecondtank || (botsecondtank->AmIaBot && botsecondtank->qglobal))
		return NULL;

	if(entity_list.GetMob(botsecondtank->GetName()))
		return botsecondtank;
	else
		return NULL;
}

Mob* BotRaids::GetBotMainTarget() {
	if(botraidmaintarget == NULL)
		return NULL;
	else
		return botraidmaintarget;
}

Mob* BotRaids::GetBotSecondTarget() {
	if(botraidsecondtarget == NULL)
		return NULL;
	else
		return botraidsecondtarget;
}

void BotRaids::SetAttackBotRaidRights(int right) {
	if( right < 0 || right > 1 )
		return;
	else{
		abotAttack = right;
	}
}

int BotRaids::GetBotAttackRights() {
	if( abotAttack == 0 || abotAttack == 1 )
		return abotAttack;
	else{
		return 0;
	}
}

void BotRaids::SaveGroups(Client *c) {
	database.DeleteBotGroups(c->CharacterID());
	for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
		if(BotRaidGroups[i]) {
			for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(BotRaidGroups[i]->members[j]) {
					if(BotRaidGroups[i]->members[j]->IsClient()) {
						continue;
					}
					database.SaveBotGroups(i, c->CharacterID(), BotRaidGroups[i]->members[j]->GetNPCTypeID(), j);
				}
			}
		}
	}
}

void BotRaids::BotRaidInfo(Client *c) {
	if(c->IsClient()) {
		if(c->IsBotRaiding()) {
			bool moredata = false;
			for(int i=0; i<MAX_BOT_RAID_GROUPS; i++) {
				if(BotRaidGroups[i] && BotRaidGroups[i]->members[0]) {
					moredata = true;
					c->Message(15, "Group %i (%i members)", i+1, BotRaidGroups[i]->BotGroupCount());
					c->Message(15, "Group %i Leader: %s", i+1, BotRaidGroups[i]->members[0]->GetName());
					for(int j=0; j<MAX_GROUP_MEMBERS; j++) {
						if(BotRaidGroups[i]->members[j]) {
							int memberClass = BotRaidGroups[i]->members[j]->GetClass();
							if((memberClass == MONK) || (memberClass == WARRIOR) || (memberClass == BERSERKER) || (memberClass == ROGUE))
							{
								c->Message(15, "%s,  hp: %i | %i",
									BotRaidGroups[i]->members[j]->GetName(),
									BotRaidGroups[i]->members[j]->GetHP(),
									BotRaidGroups[i]->members[j]->GetMaxHP());
							}
							else
							{
								c->Message(15, "%s,  hp: %i | %i  mana: %i | %i",
									BotRaidGroups[i]->members[j]->GetName(),
									BotRaidGroups[i]->members[j]->GetHP(),
									BotRaidGroups[i]->members[j]->GetMaxHP(),
									BotRaidGroups[i]->members[j]->GetMana(),
									BotRaidGroups[i]->members[j]->GetMaxMana());
							}
						}
					}
					c->Message(15, "---------");
				}
			}
			if(moredata) {
				c->Message(15, "Raid Leader is %s", botrleader->GetName());
				if(botmaintank) {
					c->Message(15, "Main Tank is %s", botmaintank->GetName());
				}
				else {
					c->Message(15, "A Main Tank is not assigned.");
				}
				if(botsecondtank) {					
					c->Message(15, "Secondary Tank is %s", botsecondtank->GetName());
				}
				else {
					c->Message(15, "A Secondary Tank is not assigned.");
				}
				if(botraidmaintarget) {
					c->Message(15, "Main Raid Target is %s", botraidmaintarget->GetCleanName());
				}
				else {
					c->Message(15, "A Main Raid Target is not assigned.");
				}
				//			if(raidsecondtarget) {
				//				c->Message(15, "Secondary Raid Target is %s", raidsecondtarget->GetCleanName());
				//			}
				//			else {
				//				c->Message(15, "A Secondary Raid Target is not assigned.");
				//			}
			}
		}
	}
}

#endif //EQBOTS
