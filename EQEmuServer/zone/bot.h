#ifndef BOT_H
#define BOT_H

#ifdef BOTS

#include "botStructs.h"
#include "mob.h"
#include "client.h"
#include "groups.h"
#include "zonedb.h"
#include "StringIDs.h"
#include "../common/MiscFunctions.h"

using namespace std;

typedef enum {	//focus types
	botfocusSpellHaste = 1,
	botfocusSpellDuration,
	botfocusRange,
	botfocusReagentCost,
	botfocusManaCost,
	botfocusImprovedHeal,
	botfocusImprovedDamage,
	botfocusImprovedDOT,		//i dont know about this...
	botfocusImprovedCritical,
	botfocusImprovedUndeadDamage,
	botfocusPetPower,
} botfocusType;	

class Bot : public NPC {
public:
	// Class Constructors
	Bot(NPCType npcTypeData, Client* botOwner);
	Bot(uint32 botID, uint32 botOwnerCharacterID, uint32 botInventoryID, uint32 botSpellsID, NPCType npcTypeData);

	// Class Methods
	bool IsValidRaceClassCombo();
	bool IsValidName();
	bool IsBotNameAvailable(std::string* errorMessage);
	bool DeleteBot(std::string* errorMessage);
	void Spawn(Client* botCharacterOwner, std::string* errorMessage);
	//void SetBotOwnerCharacterID(uint32 botOwnerCharacterID, std::string* errorMessage);
	void Depop(std::string* errorMessage);
	bool MesmerizeTarget(Mob* target);
	virtual void SetLevel(uint8 in_level, bool command = false) override;
	virtual void FillSpawnStruct(NewSpawn_Struct* ns, Mob* ForWho) override;
	virtual bool Process() override;
	//virtual void AI_Process();
	virtual bool Save() override;
	virtual void Depop(bool StartSpawnTimer = true) override;
	void BotRemoveEquipItem(int slot) { equipment[slot] = 0; }
	void BotAddEquipItem(int slot, uint32 id) { equipment[slot] = id; }
	void SendBotArcheryWearChange(int8 material_slot, uint32 material, uint32 color);
	void CalcBotStats(bool showtext = true);
	int16 BotGetSpells(int spellslot) { return AIspells[spellslot].spellid; }
	int16 BotGetSpellType(int spellslot) { return AIspells[spellslot].type; }
    int16 BotGetSpellPriority(int spellslot) { return AIspells[spellslot].priority; }
	bool Bot_Command_Resist(int resisttype, int level);
	bool Bot_Command_DireTarget(int diretype, Mob *target);
	bool Bot_Command_CharmTarget(int charmtype, Mob *target);
	bool Bot_Command_CalmTarget(Mob *target);
	bool Bot_Command_RezzTarget(Mob *target);
	bool Bot_Command_Cure(int curetype, int level);

	// AI Methods
	virtual bool AI_EngagedCastCheck();
	virtual bool Bot_AI_PursueCastCheck();
	virtual bool Bot_AI_IdleCastCheck();
	virtual bool Bot_AICastSpell(Mob* tar, int8 iChance, int16 iSpellTypes);
	
	// Bot Orders
	virtual bool IsBotOrderAttack() { return _botOrderAttack; }
	virtual void SetBotOrderAttack(bool botAttack) { _botOrderAttack = botAttack; }

	// Bot Inventory Class Methods
	uint32 GetBotItemBySlot(uint32 slotID, std::string* errorMessage);
	std::list<BotInventory> GetBotItems(std::string* errorMessage);
	void RemoveBotItemBySlot(uint32 slotID, std::string* errorMessage);
	void SetBotItemInSlot(uint32 slotID, uint32 itemID, std::string* errorMessage);
	uint32 GetBotItemsCount(std::string* errorMessage);
	void BotTradeSwapItem(Client* client, sint16 lootSlot, uint32 id, sint16 maxCharges, uint32 equipableSlots, std::string* errorMessage, bool swap = true);
	void BotTradeAddItem(uint32 id, sint16 maxCharges, uint32 equipableSlots, int16 lootSlot, Client* client, std::string* errorMessage, bool addToDb = true);

	// Static Class Methods
	static Bot* LoadBot(uint32 botID, std::string* errorMessage);
	static std::list<BotsAvailableList> GetBotList(uint32 botOwnerCharacterID, std::string* errorMessage);
	static void ProcessBotCommands(Client *c, const Seperator *sep);
	static std::list<SpawnedBotsList> ListSpawnedBots(uint32 characterID, std::string* errorMessage);
	static void SaveBotGroups(uint32 groupID, uint32 characterID, uint32 botID, uint16 slotID, std::string* errorMessage);
	static void DeleteBotGroups(uint32 characterID, std::string* errorMessage);
	static std::list<BotGroup> LoadBotGroups(uint32 characterID, std::string* errorMessage);
	static uint32 SpawnedBotCount(uint32 botOwnerCharacterID, std::string* errorMessage);
	static uint32 AllowedBotSpawns(uint32 botOwnerCharacterID, std::string* errorMessage);
	static void CleanBotLeader(uint32 botOwnerCharacterID, std::string* errorMessage);
	static uint32 GetBotOwnerCharacterID(uint32 botID, std::string* errorMessage);
	static bool SetBotOwnerCharacterID(uint32 botID, uint32 botOwnerCharacterID, std::string* errorMessage);
	static std::string ClassIdToString(uint16 classId);
	static std::string RaceIdToString(uint16 raceId);
	static uint32 GetCountBotsInGroup(Group* group);
	static bool AddBotToGroup(Bot* bot, Group* group);

	// "GET" Class Methods
	//uint32 GetBotID() { return this->GetNPCTypeID(); }
	uint32 GetBotID() { return _botID; }
	uint32 GetBotOwnerCharacterID() { return _botOwnerCharacterID; }
	uint32 GetBotSpellID() { return _botSpellID; }
	uint32 GetInventoryID() { return _botInventoryID; }
	Mob* GetBotOwner() { return _botOwner; }
	uint32 GetBotArcheryRange() { return _botArcheryRange; }
	virtual bool GetSpawnStatus() { return _spawnStatus; }
	int8 GetPetChooserID() { return _petChooserID; }
	bool IsBotRaiding() { return _botRaiding; }
	bool IsPetChooser() { return _petChooser; }
	bool IsBotArcher() { return _botArcher; }
	bool IsBotCharmer() { return _botCharmer; }
	virtual bool IsBot() const { return true; }

	// "SET" Class Methods
	void SetBotSpellID(uint32 newSpellID);
	virtual void SetSpawnStatus(bool spawnStatus) { _spawnStatus = spawnStatus; }
	void SetBotArcheryRange(uint32 archeryRange) { _botArcheryRange = archeryRange; }
	void SetPetChooserID(int8 id) { _petChooserID = id; }
	void SetBotArcher(bool a) { _botArcher = a; }
	void SetBotCharmer(bool c) { _botCharmer = c; }
	void SetBotRaiding(bool v) { _botRaiding = v; }
	void SetPetChooser(bool p) { _petChooser = p; }
	void SetBotOwner(Mob* botOwner) { _botOwner = botOwner; }

	// Class Deconstructors
	virtual ~Bot();

protected:
	virtual void BotAIProcess();
	virtual void PetAIProcess();
	static NPCType FillNPCTypeStruct(std::string botName, std::string botLastName, uint8 botLevel, uint16 botRace, uint8 botClass, uint8 botBodyType, sint32 hitPoints, uint8 gender, float size, uint32 hitPointsRegenRate, uint32 manaRegenRate, uint32 face, uint32 hairStyle, uint32 hairColor, uint32 eyeColor, uint32 eyeColor2, uint32 beardColor, uint32 beard, uint32 drakkinHeritage, uint32 drakkinTattoo, uint32 drakkinDetails, float runSpeed, sint16 mr, sint16 cr, sint16 dr, sint16 fr, sint16 pr, sint16 ac, uint16 str, uint16 sta, uint16 dex, uint16 agi, uint16 _int, uint16 wis, uint16 cha, uint16 attack);
	static NPCType CreateDefaultNPCTypeStructForBot(std::string botName, std::string botLastName, uint8 botLevel, uint16 botRace, uint8 botClass, uint8 gender);
	virtual void BotMeditate(bool isSitting);
	virtual bool BotRangedAttack(Mob* other);
	virtual bool BotAttackMelee(Mob* other, int Hand = 13, bool = false);
	virtual bool CheckBotDoubleAttack(bool Triple = false);

private:
	// Class Members
	uint32 _botID;
	uint32 _botOwnerCharacterID;
	uint32 _botSpellID;
	uint32 _botInventoryID;
	bool _spawnStatus;
	Mob* _botOwner;
	// uint32 _expPoints;
	// uint32 _aaPoints;
	bool _botOrderAttack;
	bool _botRaiding;
	bool _botArcher;
	bool _botCharmer;
	bool _petChooser;
	int8 _petChooserID;
	uint32 _botArcheryRange;
	bool cast_last_time;


	// Class Methods
	void GenerateBaseStats();
	void GenerateAppearance();
	void GenerateArmorClass();
	void GenerateBaseHitPoints();
	void SetBotLeader(uint32 botID, uint32 botOwnerCharacterID, std::string botName, std::string zoneShortName, std::string* errorMessage);
	uint32 GetBotLeader(uint32 botID, std::string* errorMessage);
	//void CleanBotLeaderEntries(std::string* errorMessage);
	void DoAIProcessing();
	void SetBotID(uint32 botID);
};

#endif // BOTS

#endif // BOT_H