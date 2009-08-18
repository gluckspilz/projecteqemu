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

#ifndef HATELIST_H
#define HATELIST_H

class tHateEntry
{
public:
    Mob *ent;
    sint32 damage, hate;
    bool bFrenzy;
};

class HateList
{
public:
    HateList();
    ~HateList();

    // adds a mob to the hatelist
    void Add(Mob *ent, sint32 in_hate=0, sint32 in_dam=0, bool bFrenzy = false, bool iAddIfNotExist = true);
    // sets existing hate
    void Set(Mob *other, int32 in_hate, int32 in_dam);
    // removes mobs from hatelist
    bool RemoveEnt(Mob *ent);
    // Remove all
    void Wipe();
    // ???
    void DoFactionHits(sint32 nfl_id);
    // Gets Hate amount for mob
    sint32 GetEntHate(Mob *ent, bool damage = false);
    // gets top hated mob
    Mob *GetTop(Mob *center);
    // gets any on the list
    Mob *GetRandom();
    // get closest mob or NULL if list empty
    Mob *GetClosest(Mob *hater);
    // gets top mob or NULL if hate list empty
    Mob *GetDamageTop(Mob *hater);
    // used to check if mob is on hatelist
    bool IsOnHateList(Mob *);
    // used to remove or add frenzy hate
    void CheckFrenzyHate();
	//Gets the target with the most hate regardless of things like frenzy etc.
	Mob* GetMostHate();

	int AreaRampage(Mob *caster, Mob *target);

    bool IsEmpty();
	void PrintToClient(Client *c);

	//For accessing the hate list via perl; don't use for anything else
	std::list<tHateEntry*> GetHateList();

protected:
    tHateEntry *Find(Mob *ent);
private:
    LinkedList<tHateEntry*> list;
};

#endif
