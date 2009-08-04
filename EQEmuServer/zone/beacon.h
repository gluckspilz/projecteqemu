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

#ifndef BEACON_H
#define BEACON_H

#include "entity.h"
#include "mob.h"
#include "../common/types.h"
#include "../common/timer.h"

class Beacon : public Mob
{
public:
	Beacon(Mob *at_mob, int lifetime);
	~Beacon();
	
	//abstract virtual function implementations requird by base abstract class
	virtual void FinishTrade(Mob* tradingWith) { return; }
	virtual void Death(Mob* killerMob, sint32 damage, int16 spell_id, SkillType attack_skill) { return; }
	virtual void Damage(Mob* from, sint32 damage, int16 spell_id, SkillType attack_skill, bool avoidable = true, sint8 buffslot = -1, bool iBuffTic = false) { return; }
	virtual bool Attack(Mob* other, int Hand = 13, bool FromRiposte = false) { return false; }
	virtual void AddToHateList(Mob* other, sint32 hate = 0, sint32 damage = 0, bool iYellForHelp = true, bool bFrenzy = false, bool iBuffTic = false) { return; }

	bool	IsBeacon()			const { return true; }
	bool	Process();
	virtual void	Depop(bool not_used = true)	{ remove_me = true; }
	void AELocationSpell(Mob *caster, int16 cast_spell_id);

protected:
	Timer remove_timer;
	bool remove_me;

	int16 spell_id;
	int spell_iterations;
	Timer spell_timer;

	int16 caster_id;
private:
};

#endif
