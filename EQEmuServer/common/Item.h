/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2003  EQEMu Development Team (http://eqemulator.net)

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

// @merth notes:
// These classes could be optimized with database reads/writes by storing
// a status flag indicating how object needs to interact with database

#ifndef __ITEM_H
#define __ITEM_H

class ItemInst;				// Item belonging to a client (contains info on item, dye, augments, charges, etc)
class ItemInstQueue;		// Queue of ItemInst objects (i.e., cursor)
class Inventory;			// Character inventory
class ItemParse;			// Parses item packets
class EvoItemInst;			// Extended item inst, for use with scaling/evolving items
class EvolveInfo;			// Stores information about an evolving item family

#include <string>
#include <vector>
#include <map>
#include <list>
using namespace std;
#include "../common/eq_packet_structs.h"
#include "../common/eq_constants.h"
#include "../common/item_struct.h"

// Helper typedefs
typedef list<ItemInst*>::const_iterator					iter_queue;
typedef map<sint16, ItemInst*>::const_iterator			iter_inst;
typedef map<uint8, ItemInst*>::const_iterator			iter_contents;

namespace ItemField {
	enum {
		source=0,
#define F(x) x,
#include "item_fieldlist.h"
#undef F
		updated
	};
};

// Indexing positions to the beginning slot_id's for a bucket of slots
#define IDX_EQUIP		0
#define IDX_CURSOR_BAG	331
#define IDX_INV			22
#define IDX_INV_BAG		251
#define IDX_TRIBUTE		400
#define IDX_BANK		2000
#define IDX_BANK_BAG	2031
#define IDX_SHBANK		2500
#define IDX_SHBANK_BAG	2531
#define IDX_TRADE		3000
#define IDX_TRADE_BAG	3031
#define IDX_TRADESKILL	4000
#define MAX_ITEMS_PER_BAG 10

// Specifies usage type for item inside ItemInst
enum ItemUseType
{
	ItemUseNormal,
	ItemUseWorldContainer
};

typedef enum {
	byFlagIgnore,	//do not consider this flag
	byFlagSet,		//apply action if the flag is set
	byFlagNotSet	//apply action if the flag is NOT set
} byFlagSetting;


//FatherNitwit: location bits for searching specific
//places with HasItem() and HasItemByUse()
enum {
	invWhereWorn 		= 0x01,
	invWherePersonal	= 0x02,	//in the character's inventory
	invWhereBank		= 0x04,
	invWhereSharedBank	= 0x08,
	invWhereTrading		= 0x10,
	invWhereCursor		= 0x20
};


// ########################################
// Class: Queue
//	Queue that allows a read-only iterator
class ItemInstQueue
{
public:
	~ItemInstQueue();
	/////////////////////////
	// Public Methods
	/////////////////////////
	
	inline iter_queue begin()	{ return m_list.begin(); }
	inline iter_queue end()		{ return m_list.end(); }
	
	void push(ItemInst* inst);
	void push_front(ItemInst* inst);
	ItemInst* pop();
	ItemInst* peek_front() const;
	inline int size() 		{ return m_list.size(); }
	
protected:
	/////////////////////////
	// Protected Members
	/////////////////////////
	
	list<ItemInst*> m_list;
	
};

// ########################################
// Class: Inventory
//	Character inventory
class Inventory
{
	friend class ItemInst;
public:
	///////////////////////////////
	// Public Methods
	///////////////////////////////
	
	virtual ~Inventory();
	
	// Retrieve a writeable item at specified slot
	ItemInst* GetItem(sint16 slot_id) const;
	ItemInst* GetItem(sint16 slot_id, uint8 bagidx) const;

	inline iter_queue cursor_begin()	{ return m_cursor.begin(); }
	inline iter_queue cursor_end()		{ return m_cursor.end(); }
	
	// Retrieve a read-only item from inventory
	inline const ItemInst* operator[](sint16 slot_id) const { return GetItem(slot_id); }
	
	// Add item to inventory
	sint16 PutItem(sint16 slot_id, const ItemInst& inst);

	// Add item to cursor queue
	sint16 PushCursor(const ItemInst& inst);
	
	// Swap items in inventory
	void SwapItem(sint16 slot_a, sint16 slot_b);

	// Remove item from inventory
	bool DeleteItem(sint16 slot_id, uint8 quantity=0);

	// Checks All items in a bag for No Drop
	bool CheckNoDrop(sint16 slot_id);
	
	// Remove item from inventory (and take control of memory)
	ItemInst* PopItem(sint16 slot_id);
	
	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	sint16 HasItem(uint32 item_id, uint8 quantity=0, uint8 where=0xFF);

	// Check whether there is space for the specified number of the specified item.
	bool HasSpaceForItem(const Item_Struct *ItemToTry, uint8 Quantity);
	
	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	sint16 HasItemByUse(uint8 use, uint8 quantity=0, uint8 where=0xFF);

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	sint16 HasItemByLoreGroup(uint32 loregroup, uint8 where=0xFF);
	
	// Locate an available inventory slot
	sint16 FindFreeSlot(bool for_bag, bool try_cursor, int8 min_size = 0, bool is_arrow = false);
	
	// Calculate slot_id for an item within a bag
	static sint16 CalcSlotId(sint16 slot_id); // Calc parent bag's slot_id
	static sint16 CalcSlotId(sint16 bagslot_id, uint8 bagidx); // Calc slot_id for item inside bag
	static uint8 CalcBagIdx(sint16 slot_id); // Calc bagidx for slot_id
	static sint16 CalcSlotFromMaterial(int8 material);
	static int8 CalcMaterialFromSlot(sint16 equipslot);

	static bool CanItemFitInContainer(const Item_Struct *ItemToTry, const Item_Struct *Container);

	// Test whether a given slot can support a container item
	static bool SupportsContainers(sint16 slot_id);
	
	void dumpInventory();
	
protected:
	///////////////////////////////
	// Protected Methods
	///////////////////////////////
	
	// Retrieves item within an inventory bucket
	ItemInst* _GetItem(const map<sint16, ItemInst*>& bucket, sint16 slot_id) const;
	
	// Private "put" item into bucket, without regard for what is currently in bucket
	sint16 _PutItem(sint16 slot_id, ItemInst* inst);
	
	// Checks an inventory bucket for a particular item
	sint16 _HasItem(map<sint16, ItemInst*>& bucket, uint32 item_id, uint8 quantity);
	sint16 _HasItem(ItemInstQueue& iqueue, uint32 item_id, uint8 quantity);
	sint16 _HasItemByUse(map<sint16, ItemInst*>& bucket, uint8 use, uint8 quantity);
	sint16 _HasItemByUse(ItemInstQueue& iqueue, uint8 use, uint8 quantity);
	sint16 _HasItemByLoreGroup(map<sint16, ItemInst*>& bucket, uint32 loregroup);
	sint16 _HasItemByLoreGroup(ItemInstQueue& iqueue, uint32 loregroup);
	
	
	// Player inventory
	map<sint16, ItemInst*>	m_worn;		// Items worn by character
	map<sint16, ItemInst*>	m_inv;		// Items in character personal inventory
	map<sint16, ItemInst*>	m_bank;		// Items in character bank
	map<sint16, ItemInst*>	m_shbank;	// Items in character shared bank
	map<sint16, ItemInst*>	m_trade;	// Items in a trade session
	ItemInstQueue			m_cursor;	// Items on cursor: FIFO
};

class SharedDatabase;

// ########################################
// Class: ItemInst
//	Base class for an instance of an item
//	An item instance encapsulates item data + data specific
//	to an item instance (includes dye, augments, charges, etc)
class ItemInst
{
public:
	/////////////////////////
	// Methods
	/////////////////////////
	
	// Constructors/Destructor
	ItemInst(const Item_Struct* item = NULL, sint16 charges = 0);
	
	ItemInst(SharedDatabase *db, uint32 item_id, sint16 charges = 0);
	
	ItemInst(ItemUseType use_type) {
		m_use_type = use_type;
		m_item = NULL;
		m_charges = 0;
		m_price = 0;
		m_instnodrop = false;
		m_merchantslot = 0;
		m_color = 0;
	}

	ItemInst(const ItemInst& copy);
	
	virtual ~ItemInst();
	
	// Query item type
	virtual bool IsType(ItemClass item_class) const;
	
	// Can item be stacked?
	virtual bool IsStackable() const;

	// Can item be equipped by/at?
	virtual bool IsEquipable(int16 race, int16 class_) const;
	virtual bool IsEquipable(sint16 slot_id) const;
	
	//
	// Augements
	//
	inline bool IsAugmentable() const { return m_item->AugSlotType[0]!=0; }
	sint8 AvailableAugmentSlot(sint32 augtype) const;
	inline sint32 GetAugmentType() const { return m_item->AugType; }

	inline bool IsExpendable() const { return ((m_item->Click.Type == ET_Expendable ) || (m_item->ItemType == ItemTypePotion)); }

	//
	// Contents
	//
	ItemInst* GetItem(uint8 slot) const;
	virtual uint32 GetItemID(uint8 slot) const;
	inline const ItemInst* operator[](uint8 slot) const { return GetItem(slot); }
	void PutItem(uint8 slot, const ItemInst& inst);
	void PutItem(SharedDatabase *db, uint8 slot, uint32 item_id);
	void DeleteItem(uint8 slot);
	ItemInst* PopItem(uint8 index);
	void Clear();
	void ClearByFlags(byFlagSetting is_nodrop, byFlagSetting is_norent);
	uint8 FirstOpenSlot() const;

	//
	// Augments
	//
	ItemInst* GetAugment(uint8 slot) const;
	virtual uint32 GetAugmentItemID(uint8 slot) const;
	void PutAugment(uint8 slot, const ItemInst& inst);
	void PutAugment(SharedDatabase *db, uint8 slot, uint32 item_id);
	void DeleteAugment(uint8 slot);
	ItemInst* RemoveAugment(uint8 index);

	// Has attack/delay?
	virtual bool IsWeapon() const;
	virtual bool IsAmmo() const;

	// Accessors
	const uint32 GetID() const { return m_item->ID; }
	virtual const Item_Struct* GetItem() const		{ return m_item; }
	void SetItem(const Item_Struct* item)	{ m_item = item; }
	
	sint16 GetCharges() const				{ return m_charges; }
	void SetCharges(sint16 charges)			{ m_charges = charges; }
	
	uint32 GetPrice() const					{ return m_price; }
	void SetPrice(uint32 price)				{ m_price = price; }
	
	void SetColor(uint32 color)				{ m_color = color; }
	uint32 GetColor() const					{ return m_color; }

	uint32 GetMerchantSlot() const				{ return m_merchantslot; }
	void SetMerchantSlot(uint32 slot)		{ m_merchantslot = slot; }

	sint32 GetMerchantCount() const				{ return m_merchantcount; }
	void SetMerchantCount(sint32 count)		{ m_merchantcount = count; }

	sint16 GetCurrentSlot() const			{ return m_currentslot; }
	void SetCurrentSlot(sint16 curr_slot)   { m_currentslot = curr_slot; }



	// Is this item already attuned?
	bool IsInstNoDrop() const { return m_instnodrop; }
	void SetInstNoDrop(bool flag) { m_instnodrop=flag; }

	// Allows treatment of this object as though it were a pointer to m_item
	operator bool() const { return (m_item != NULL); }
	
	// Compare inner Item_Struct of two ItemInst objects
	bool operator==(const ItemInst& right) const { return (this->m_item == right.m_item); }
	bool operator!=(const ItemInst& right) const { return (this->m_item != right.m_item); }
	
	// Clone current item
	virtual ItemInst* Clone() const;
	
	bool IsSlotAllowed(sint16 slot_id) const;

	virtual bool IsScaling() const		{ return false; }
	virtual bool IsEvolving() const		{ return false; }

	string Serialize(sint16 slot_id) const { InternalSerializedItem_Struct s; s.slot_id=slot_id; s.inst=(const void *)this; string ser; ser.assign((char *)&s,sizeof(InternalSerializedItem_Struct)); return ser; }
	inline sint32 GetSerialNumber() const { return m_SerialNumber; }
	inline void SetSerialNumber(sint32 id) { m_SerialNumber = id; }

protected:
	//////////////////////////
	// Protected Members
	//////////////////////////
	iter_contents _begin()               { return m_contents.begin(); }
	iter_contents _end()                 { return m_contents.end(); }

	friend class Inventory;
	
	
	void _PutItem(uint8 index, ItemInst* inst) { m_contents[index] = inst; }
	
	ItemUseType			m_use_type;	// Usage type for item
	const Item_Struct*	m_item;		// Ptr to item data
	sint16				m_charges;	// # of charges for chargeable items
	uint32				m_price;	// Bazaar /trader price
	uint32				m_color;
	uint32				m_merchantslot;
	sint16				m_currentslot;
	bool 				m_instnodrop;
	sint32				m_merchantcount;		//number avaliable on the merchant, -1=unlimited
	sint32				m_SerialNumber;	// Unique identifier for this instance of an item. Needed for Bazaar.
	//
	// Items inside of this item (augs or contents);
	map<uint8, ItemInst*> m_contents; // Zero-based index: min=0, max=9
	
};

class EvoItemInst: public ItemInst {
public:
	// constructor and destructor
	EvoItemInst(const EvoItemInst& copy);
	EvoItemInst(const ItemInst& copy);
	EvoItemInst(const Item_Struct* item = NULL, sint16 charges = 0);
	~EvoItemInst();
	
	// accessors... a lot of these are for evolving items (not complete yet)
	bool IsScaling() const				{ return (m_evolveLvl == -1); }
	bool IsEvolving() const				{ return (m_evolveLvl >= 1); }
	uint32 GetExp() const				{ return m_exp; }
	void SetExp(uint32 exp)				{ m_exp = exp; }
	void AddExp(uint32 exp)				{ m_exp += exp; }
	bool IsActivated()					{ return m_activated; }
	void SetActivated(bool activated)	{ m_activated = activated; }
	sint8 GetEvolveLvl() const			{ return m_evolveLvl; }
		
	EvoItemInst* Clone() const;
	const Item_Struct* GetItem() const;
	const Item_Struct* GetUnscaledItem() const;
	void Initialize(SharedDatabase *db = NULL);
	void ScaleItem();
	bool EvolveOnAllKills() const;	
	sint8 GetMaxEvolveLvl() const;
	uint32 GetKillsNeeded(uint8 currentlevel);
		

private:
	uint32				m_exp;
	sint8				m_evolveLvl;
	bool				m_activated;
	Item_Struct*		m_scaledItem;
	const EvolveInfo*	m_evolveInfo;
};

class EvolveInfo {
public:
	friend class EvoItemInst;
	//temporary
	uint16				LvlKills[9];
	uint32				FirstItem;
	uint8				MaxLvl;
	bool				AllKills;
		
	EvolveInfo();
	EvolveInfo(uint32 first, uint8 max, bool allkills, uint32 L2, uint32 L3, uint32 L4, uint32 L5, uint32 L6, uint32 L7, uint32 L8, uint32 L9, uint32 L10);
	~EvolveInfo();
};

#endif // #define __ITEM_H
