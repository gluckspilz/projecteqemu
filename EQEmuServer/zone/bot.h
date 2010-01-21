#ifndef BOT_H
#define BOT_H

#ifdef BOTS

#include "botStructs.h"
#include "mob.h"
#include "client.h"
#include "pets.h"
#include "groups.h"
#include "PlayerCorpse.h"
#include "zonedb.h"
#include "StringIDs.h"
#include "../common/MiscFunctions.h"
#include "../common/debug.h"
#include "guild_mgr.h"
#include "worldserver.h"

#include <sstream>

using namespace std;

extern bool spells_loaded;
extern WorldServer worldserver;

const int BotAISpellRange = 100; // TODO: Write a method that calcs what the bot's spell range is based on spell, equipment, AA, whatever and replace this
const int SpellType_Slow = 8192;

class Bot : public NPC {
public:
	// Class enums
	typedef enum BotfocusType {	//focus types
		BotfocusSpellHaste = 1,
		BotfocusSpellDuration,
		BotfocusRange,
		BotfocusReagentCost,
		BotfocusManaCost,
		BotfocusImprovedHeal,
		BotfocusImprovedDamage,
		BotfocusImprovedDOT,		//i dont know about this...
		BotfocusImprovedCritical,
		BotfocusImprovedUndeadDamage,
		BotfocusPetPower,
		BotfocusResistRate,
		BotfocusHateReduction
	};

	typedef std::map<uint32, uint32> BotInventory;
	typedef std::pair<uint32, uint32> BotInventoryItem;

	typedef enum BotTradeType {	// types of trades a bot can do
		BotTradeClientNormal,
		BotTradeClientNoDropNoTrade
	};

	typedef enum BotRoleType {
		BotRoleMainAssist,
		BotRoleGroupHealer,
		BotRoleRaidHealer
	};

	// Class Constructors
	Bot(NPCType npcTypeData, Client* botOwner);
	Bot(uint32 botID, uint32 botOwnerCharacterID, uint32 botSpellsID, double totalPlayTime, int32 lastZoneId, NPCType npcTypeData);

	//abstract virtual function implementations requird by base abstract class
	virtual void Death(Mob* killerMob, sint32 damage, int16 spell_id, SkillType attack_skill);
	virtual void Damage(Mob* from, sint32 damage, int16 spell_id, SkillType attack_skill, bool avoidable = true, sint8 buffslot = -1, bool iBuffTic = false);
	virtual bool Attack(Mob* other, int Hand = 13, bool FromRiposte = false);
	virtual bool HasRaid() { return (GetRaid() ? true : false);  }
	virtual bool HasGroup() { return (GetGroup() ? true : false); }
	virtual Raid* GetRaid() { return entity_list.GetRaidByMob(this); }
	virtual Group* GetGroup() { return entity_list.GetGroupByMob(this); }
	
	// Common, but informal "interfaces" with Client object
	int32 CharacterID() { return GetBotID(); } // Just returns the Bot Id
	inline bool IsInAGuild() const { return (_guildId != GUILD_NONE && _guildId != 0); }
	inline bool IsInGuild(uint32 in_gid) const { return (in_gid == _guildId && IsInAGuild()); }
	inline int32 GuildID() const { return _guildId; }
	inline int8	GuildRank()	const { return _guildRank; }

	// Class Methods
	bool IsValidRaceClassCombo();
	bool IsValidName();
	bool IsBotNameAvailable(std::string* errorMessage);
	bool DeleteBot(std::string* errorMessage);
	void Spawn(Client* botCharacterOwner, std::string* errorMessage);
	virtual void SetLevel(uint8 in_level, bool command = false);
	virtual void FillSpawnStruct(NewSpawn_Struct* ns, Mob* ForWho);
	virtual bool Process();
	void FinishTrade(Client* client, BotTradeType tradeType);
	virtual bool Save();
	virtual void Depop();
	void CalcBotStats(bool showtext = true);
	int16 BotGetSpells(int spellslot) { return AIspells[spellslot].spellid; }
	int16 BotGetSpellType(int spellslot) { return AIspells[spellslot].type; }
    int16 BotGetSpellPriority(int spellslot) { return AIspells[spellslot].priority; }
	virtual float GetHitChance(Mob* attacker, SkillType skillinuse, int Hand);
	virtual float GetProcChances(float &ProcBonus, float &ProcChance, int16 weapon_speed);
	virtual bool AvoidDamage(Mob* other, sint32 &damage);
	virtual int GetMonkHandToHandDamage(void);
	virtual void TryCriticalHit(Mob *defender, int16 skill, sint32 &damage);
	virtual bool TryFinishingBlow(Mob *defender, SkillType skillinuse);
	virtual void DoRiposte(Mob* defender);
	inline virtual sint16 GetATK() const { return ATK + itembonuses.ATK + spellbonuses.ATK + ((GetSTR() + GetSkill(OFFENSE)) * 9 / 10); }
	virtual void MeleeMitigation(Mob *attacker, sint32 &damage, sint32 minhit);
	virtual void DoSpecialAttackDamage(Mob *who, SkillType skill, sint32 max_damage, sint32 min_damage = 1, sint32 hate_override = -1);
	virtual void TryBackstab(Mob *other);
	virtual void RogueBackstab(Mob* other, bool min_damage = false);
	virtual void RogueAssassinate(Mob* other);
	virtual void DoClassAttacks(Mob *target);
	virtual bool TryHeadShot(Mob* defender, SkillType skillInUse);
	virtual sint32 CheckAggroAmount(int16 spellid);
	virtual void CalcBonuses();
	virtual void MakePet(int16 spell_id, const char* pettype, const char *petname = NULL);
	virtual FACTION_VALUE GetReverseFactionCon(Mob* iOther);
	inline virtual bool IsPet() { return false; }
	virtual bool IsNPC() const { return false; }
	virtual Mob* GetOwner();
	virtual Mob* GetOwnerOrSelf();
	inline virtual bool HasOwner() { return (GetBotOwner() ? true : false); }
	virtual sint32 CheckHealAggroAmount(int16 spellid, int32 heal_possible = 0);
	virtual sint32 CalcMaxMana();
	virtual void SetAttackTimer();
	virtual sint32 CalcMaxHP();
	bool DoFinishedSpellAETarget(int16 spell_id, Mob* spellTarget, int16 slot, bool &stopLogic);
	bool DoFinishedSpellSingleTarget(int16 spell_id, Mob* spellTarget, int16 slot, bool &stopLogic);
	bool DoFinishedSpellGroupTarget(int16 spell_id, Mob* spellTarget, int16 slot, bool &stopLogic);
	void SendBotArcheryWearChange(int8 material_slot, uint32 material, uint32 color);
	void Camp(bool databaseSave = true);
	virtual void AddToHateList(Mob* other, sint32 hate = 0, sint32 damage = 0, bool iYellForHelp = true, bool bFrenzy = false, bool iBuffTic = false);
	virtual void SetTarget(Mob* mob);
	virtual void Zone();
	std::vector<AISpells_Struct> GetBotSpells() { return AIspells; }
	bool IsArcheryRange(Mob* target);
	void ChangeBotArcherWeapons(bool isArcher);
	void Sit();
	void Stand();
	bool IsSitting();
	bool IsStanding();
	bool IsBotCasterCombatRange(Mob *target);
	bool CalculateNewPosition2(float x, float y, float z, float speed, bool checkZ = true) ;

	// AI Methods
	virtual bool AICastSpell(Mob* tar, int8 iChance, int16 iSpellTypes);
	virtual bool AI_EngagedCastCheck();
	virtual bool AI_PursueCastCheck();
	virtual bool AI_IdleCastCheck();

	// Mob AI Virtual Override Methods
	virtual void AI_Process();
	virtual void AI_Stop();

	// Mob Spell Virtual Override Methods
	virtual sint32 GetActSpellDamage(int16 spell_id, sint32 value);
	virtual sint32 GetActSpellHealing(int16 spell_id, sint32 value);
	virtual sint32 GetActSpellCasttime(int16 spell_id, sint32 casttime);
	virtual sint32 GetActSpellCost(int16 spell_id, sint32 cost);
	virtual float GetActSpellRange(int16 spell_id, float range);
	virtual sint32 GetActSpellDuration(int16 spell_id, sint32 duration);
	virtual float GetAOERange(uint16 spell_id);
	virtual bool SpellEffect(Mob* caster, int16 spell_id, float partial = 100);
	virtual void DoBuffTic(int16 spell_id, int32 ticsremaining, int8 caster_level, Mob* caster = 0);
	virtual bool CastSpell(int16 spell_id, int16 target_id, int16 slot = 10, sint32 casttime = -1, sint32 mana_cost = -1, int32* oSpellWillFinish = 0, int32 item_slot = 0xFFFFFFFF);
	virtual bool SpellOnTarget(int16 spell_id, Mob* spelltar);
	virtual bool IsImmuneToSpell(int16 spell_id, Mob *caster);
	virtual bool DetermineSpellTargets(uint16 spell_id, Mob *&spell_target, Mob *&ae_center, CastAction_type &CastAction);
	virtual bool DoCastSpell(int16 spell_id, int16 target_id, int16 slot = 10, sint32 casttime = -1, sint32 mana_cost = -1, int32* oSpellWillFinish = 0, int32 item_slot = 0xFFFFFFFF);
	virtual int CheckStackConflict(int16 spellid1, int caster_level1, int16 spellid2, int caster_level2, Mob* caster1 = NULL, Mob* caster2 = NULL);

	// Bot Action Command Methods
	bool MesmerizeTarget(Mob* target);
	bool Bot_Command_Resist(int resisttype, int level);
	bool Bot_Command_DireTarget(int diretype, Mob *target);
	bool Bot_Command_CharmTarget(int charmtype, Mob *target);
	bool Bot_Command_CalmTarget(Mob *target);
	bool Bot_Command_RezzTarget(Mob *target);
	bool Bot_Command_Cure(int curetype, int level);

	// Bot Equipment & Inventory Class Methods
	void BotTradeSwapItem(Client* client, sint16 lootSlot, uint32 id, sint16 maxCharges, uint32 equipableSlots, std::string* errorMessage, bool swap = true);
	void BotTradeAddItem(uint32 id, sint16 maxCharges, uint32 equipableSlots, int16 lootSlot, std::string* errorMessage, bool addToDb = true);
	void EquipBot(std::string* errorMessage);

	// Static Class Methods
	static void SaveBotGroup(Group* botGroup, std::string botGroupName, std::string* errorMessage);
	static void DeleteBotGroup(std::string botGroupName, std::string* errorMessage);
	static std::list<BotGroup> LoadBotGroup(std::string botGroupName, std::string* errorMessage);
	static uint32 CanLoadBotGroup(uint32 botOwnerCharacterId, std::string botGroupName, std::string* errorMessage);
	static uint32 GetBotGroupIdByBotGroupName(std::string botGroupName, std::string* errorMessage);
	static uint32 GetBotGroupLeaderIdByBotGroupName(std::string botGroupName);
	static std::list<BotGroupList> GetBotGroupListByBotOwnerCharacterId(uint32 botOwnerCharacterId, std::string* errorMessage);
	static bool DoesBotGroupNameExist(std::string botGroupName);
	//static void DestroyBotRaidObjects(Client* client);	// Can be removed after bot raids are dumped
	static uint32 GetBotIDByBotName(std::string botName);
	static Bot* LoadBot(uint32 botID, std::string* errorMessage);
	static std::list<BotsAvailableList> GetBotList(uint32 botOwnerCharacterID, std::string* errorMessage);
	static void ProcessBotCommands(Client *c, const Seperator *sep);
	static std::list<SpawnedBotsList> ListSpawnedBots(uint32 characterID, std::string* errorMessage);
	static uint32 SpawnedBotCount(uint32 botOwnerCharacterID, std::string* errorMessage);
	static uint32 AllowedBotSpawns(uint32 botOwnerCharacterID, std::string* errorMessage);
	static uint32 GetBotOwnerCharacterID(uint32 botID, std::string* errorMessage);
	//static bool SetBotOwnerCharacterID(uint32 botID, uint32 botOwnerCharacterID, std::string* errorMessage);
	static std::string ClassIdToString(uint16 classId);
	static std::string RaceIdToString(uint16 raceId);
	static bool IsBotAttackAllowed(Mob* attacker, Mob* target, bool& hasRuleDefined);
	static void BotGroupOrderFollow(Group* group, Client* client);
	static void BotGroupOrderGuard(Group* group, Client* client);
	static void BotGroupOrderAttack(Group* group, Mob* target, Client* client);
	static void BotGroupSummon(Group* group, Client* client);
	static Bot* GetBotByBotClientOwnerAndBotName(Client* c, std::string botName);
	static void ProcessBotGroupInvite(Client* c, std::string botName);
	static void ProcessBotGroupDisband(Client* c, std::string botName);
	static void BotOrderCampAll(Client* c);
	static void ProcessBotInspectionRequest(Bot* inspectedBot, Client* client);
	static std::list<uint32> GetGroupedBotsByGroupId(uint32 groupId, std::string* errorMessage);
	static void LoadAndSpawnAllZonedBots(Client* botOwner);
	static bool GroupHasBot(Group* group);
	static Bot* GetFirstBotInGroup(Group* group);
	static void ProcessClientZoneChange(Client* botOwner);
	static void ProcessBotOwnerRefDelete(Mob* botOwner);	// Removes a Client* reference when the Client object is destroyed
	static void ProcessGuildInvite(Client* guildOfficer, Bot* botToGuild);	// Processes a client's request to guild a bot
	static bool ProcessGuildRemoval(Client* guildOfficer, std::string botName);	// Processes a client's request to deguild a bot
	static std::list<BotSpell> GetBotSpellsForSpellEffect(Bot* botCaster, int spellEffect);
	static std::list<BotSpell> GetBotSpellsForSpellEffectAndTargetType(Bot* botCaster, int spellEffect, SpellTargetType targetType);
	static std::list<BotSpell> GetBotSpellsBySpellType(Bot* botCaster, int16 spellType);
	static BotSpell GetFirstBotSpellBySpellType(Bot* botCaster, int16 spellType);
	static BotSpell GetBestBotSpellForFastHeal(Bot* botCaster);
	static BotSpell GetBestBotSpellForHealOverTime(Bot* botCaster);
	static BotSpell GetBestBotSpellForPercentageHeal(Bot* botCaster);
	static BotSpell GetBestBotSpellForRegularSingleTargetHeal(Bot* botCaster);
	static BotSpell GetBestBotSpellForMagicBasedSlow(Bot* botCaster);
	static BotSpell GetBestBotSpellForDiseaseBasedSlow(Bot* botCaster);
	static Mob* GetFirstIncomingMobToMez(Bot* botCaster, BotSpell botSpell);
	static BotSpell GetBestBotSpellForMez(Bot* botCaster);
	static BotSpell GetBestBotMagicianPetSpell(Bot* botCaster);
	static std::string GetBotMagicianPetType(Bot* botCaster);
	static BotSpell GetBestBotSpellForNukeByTargetType(Bot* botCaster, SpellTargetType targetType);
	static BotSpell GetBestBotWizardNukeSpellByTargetResists(Bot* botCaster, Mob* target);

	// Static Bot Group Methods
	static bool AddBotToGroup(Bot* bot, Group* group);
	static bool RemoveBotFromGroup(Bot* bot, Group* group);
	static bool BotGroupCreate(std::string botGroupLeaderName);
	static bool BotGroupCreate(Bot* botGroupLeader);
	static bool	GroupHasClass(Group* group, uint8 classId);
	static bool GroupHasClericClass(Group* group) { return GroupHasClass(group, CLERIC); }
	static bool GroupHasDruidClass(Group* group) { return GroupHasClass(group, DRUID); }
	static bool GroupHasShamanClass(Group* group) { return GroupHasClass(group, SHAMAN); }
	static bool GroupHasPriestClass(Group* group) { return GroupHasClass(group, CLERIC | DRUID | SHAMAN); }

	// "GET" Class Methods
	uint32 GetBotID() { return _botID; }
	uint32 GetBotOwnerCharacterID() { return _botOwnerCharacterID; }
	uint32 GetBotSpellID() { return npc_spells_id; }
	Mob* GetBotOwner() { return this->_botOwner; }
	uint32 GetBotArcheryRange();
	virtual bool GetSpawnStatus() { return _spawnStatus; }
	int8 GetPetChooserID() { return _petChooserID; }
	bool IsPetChooser() { return _petChooser; }
	bool IsBotArcher() { return _botArcher; }
	bool IsBotCharmer() { return _botCharmer; }
	virtual bool IsBot() const { return true; }
	bool GetRangerAutoWeaponSelect() { return _rangerAutoWeaponSelect; }
	BotRoleType GetBotRole() { return _botRole; }
	bool IsBotCaster() { return (GetClass() == CLERIC || GetClass() == DRUID || GetClass() == SHAMAN || GetClass() == NECROMANCER || GetClass() == WIZARD || GetClass() == MAGICIAN || GetClass() == ENCHANTER); }

	// "SET" Class Methods
	void SetBotSpellID(uint32 newSpellID);
	virtual void SetSpawnStatus(bool spawnStatus) { _spawnStatus = spawnStatus; }
	void SetPetChooserID(int8 id) { _petChooserID = id; }
	void SetBotArcher(bool a) { _botArcher = a; }
	void SetBotCharmer(bool c) { _botCharmer = c; }
	void SetPetChooser(bool p) { _petChooser = p; }
	void SetBotOwner(Mob* botOwner) { this->_botOwner = botOwner; }
	// void SetBotOwnerCharacterID(uint32 botOwnerCharacterID) { _botOwnerCharacterID = botOwnerCharacterID; }
	void SetRangerAutoWeaponSelect(bool enable) { GetClass() == RANGER ? _rangerAutoWeaponSelect = enable : _rangerAutoWeaponSelect = false; }
	void SetBotRole(BotRoleType botRole) { _botRole = botRole; }

	// Class Destructors
	virtual ~Bot();

protected:
	virtual void PetAIProcess();
	static NPCType FillNPCTypeStruct(uint32 botSpellsID, std::string botName, std::string botLastName, uint8 botLevel, uint16 botRace, uint8 botClass, uint8 gender, float size, uint32 face, uint32 hairStyle, uint32 hairColor, uint32 eyeColor, uint32 eyeColor2, uint32 beardColor, uint32 beard, uint32 drakkinHeritage, uint32 drakkinTattoo, uint32 drakkinDetails, sint16 mr, sint16 cr, sint16 dr, sint16 fr, sint16 pr, sint16 ac, uint16 str, uint16 sta, uint16 dex, uint16 agi, uint16 _int, uint16 wis, uint16 cha, uint16 attack);
	static NPCType CreateDefaultNPCTypeStructForBot(std::string botName, std::string botLastName, uint8 botLevel, uint16 botRace, uint8 botClass, uint8 gender);
	virtual void BotMeditate(bool isSitting);
	virtual bool BotRangedAttack(Mob* other);
	virtual bool CheckBotDoubleAttack(bool Triple = false);
	virtual sint16 GetBotFocusEffect(BotfocusType bottype, int16 spell_id);
	virtual sint16 CalcBotFocusEffect(BotfocusType bottype, int16 focus_id, int16 spell_id);
	virtual void PerformTradeWithClient(sint16 beginSlotID, sint16 endSlotID, Client* client);
	virtual bool AIDoSpellCast(int8 i, Mob* tar, sint32 mana_cost, int32* oDontDoAgainBefore = 0);
	virtual float GetMaxMeleeRangeToTarget(Mob* target);

	static void SetBotGuildMembership(int32 botId, int32 guildid, int8 rank);

private:
	// Class Members
	uint32 _botID;
	uint32 _botOwnerCharacterID;
	//uint32 _botSpellID;
	bool _spawnStatus;
	Mob* _botOwner;
	bool _botOrderAttack;
	bool _botArcher;
	bool _botCharmer;
	bool _petChooser;
	int8 _petChooserID;
	BotInventory _botInventory;
	double _lastTotalPlayTime;
	time_t _startTotalPlayTime;
	Mob* _previousTarget;
	uint32 _guildId;
	int8 _guildRank;
	std::string _guildName;
	int32 _lastZoneId;
	bool _rangerAutoWeaponSelect;
	BotRoleType _botRole;

	// Private "base stats" Members
	sint16 _baseMR;
	sint16 _baseCR;
	sint16 _baseDR;
	sint16 _baseFR;
	sint16 _basePR;
	int _baseAC;
	sint16 _baseSTR;
	sint16 _baseSTA;
	sint16 _baseDEX;
	sint16 _baseAGI;
	sint16 _baseINT;
	sint16 _baseWIS;
	sint16 _baseCHA;
	sint16 _baseATK;
	int16 _baseRace;	// Necessary to preserve the race otherwise bots get their race updated in the db when they get an illusion.
	int8 _baseGender;	// Bots gender. Necessary to preserve the original value otherwise it can be changed by illusions.

	// Class Methods
	void GenerateBaseStats();
	void GenerateAppearance();
	void GenerateArmorClass();
	void GenerateBaseHitPoints();
	void GenerateAABonuses();
	void GenerateBaseManaPoints();
	void GenerateSpecialAttacks();
	void SetBotID(uint32 botID);
	bool CalcBotHitChance(Mob* target, SkillType skillinuse, int Hand);

	// Private "Inventory" Methods
	BotInventory GetBotItems(std::string* errorMessage);
	uint32 GetBotItem(uint32 slotID);
	void BotRemoveEquipItem(int slot);
	void BotAddEquipItem(int slot, uint32 id);
	uint32 GetBotItemBySlot(uint32 slotID);
	void RemoveBotItemBySlot(uint32 slotID, std::string* errorMessage);
	void SetBotItemInSlot(uint32 slotID, uint32 itemID, std::string* errorMessage);
	uint32 GetBotItemsCount(std::string* errorMessage);
	uint32 GetTotalPlayTime();
	void SaveBuffs();	// Saves existing buffs to the database to persist zoning and camping
	void LoadBuffs();	// Retrieves saved buffs from the database on spawning
	void LoadPetBuffs(SpellBuff_Struct* petBuffs, uint32 botPetSaveId);
	void SavePetBuffs(SpellBuff_Struct* petBuffs, uint32 botPetSaveId);
	void LoadPetItems(int32* petItems, uint32 botPetSaveId);
	void SavePetItems(int32* petItems, uint32 botPetSaveId);
	void LoadPetStats(std::string* petName, int16* petMana, int16* petHitPoints, uint32* botPetId, uint32 botPetSaveId);
	uint32 SavePetStats(std::string petName, int16 petMana, int16 petHitPoints, uint32 botPetId);
	void LoadPet();	// Load and spawn bot pet if there is one
	void SavePet();	// Save and depop bot pet if there is one
	uint32 GetPetSaveId();
	void DeletePetBuffs(uint32 botPetSaveId);
	void DeletePetItems(uint32 botPetSaveId);
	void DeletePetStats(uint32 botPetSaveId);
	void LoadGuildMembership(int32* guildId, int8* guildRank, std::string* guildName);
};

#endif // BOTS

#endif // BOT_H
