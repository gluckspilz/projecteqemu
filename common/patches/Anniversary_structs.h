#ifndef Anniversary_STRUCTS_H_
#define Anniversary_STRUCTS_H_

namespace Anniversary {
	namespace structs {


static const uint32 BUFF_COUNT = 25;

/*
** Compiler override to ensure
** byte aligned structures
*/
#pragma pack(1)

struct LoginInfo_Struct {
/*000*/	char	login_info[64];
/*064*/	uint8	unknown064[124];
/*188*/	uint8	zoning;			// 01 if zoning, 00 if not
/*189*/	uint8	unknown189[275];
/*488*/
};

struct EnterWorld_Struct {
/*000*/	char	name[64];
/*064*/	uint32	tutorial;		// 01 on "Enter Tutorial", 00 if not
/*068*/	uint32	return_home;		// 01 on "Return Home", 00 if not
};

/* Name Approval Struct */
/* Len: */
/* Opcode: 0x8B20*/
struct NameApproval
{
	char name[64];
	int32 race;
	int32 class_;
	int32 deity;
};

/*
** Entity identification struct
** Size: 4 bytes
** OPCodes: OP_DeleteSpawn, OP_Assist
*/
struct EntityId_Struct
{
/*00*/	uint32	entity_id;
/*04*/
};

struct Duel_Struct
{
	int32 duel_initiator;
	int32 duel_target;
};

struct DuelResponse_Struct
{
	int32 target_id;
	int32 entity_id;
	int32 unknown;
};
/*
	Cofruben:
	Adventure stuff,not a net one,just one for our use 
*/
static const uint32 ADVENTURE_COLLECT		= 0;
static const uint32 ADVENTURE_MASSKILL		= 1;
static const uint32 ADVENTURE_NAMED			= 2;
static const uint32 ADVENTURE_RESCUE		= 3;

struct AdventureInfo {
	int32 QuestID;
	int32 NPCID;
	bool in_use;
	int32 status;
	bool ShowCompass;
	int32 Objetive;// can be item to collect,mobs to kill,boss to kill and someone to rescue.
	int32 ObjetiveValue;// number of items,or number of needed mob kills.
	char text[512];
	int8 type;
	int32 minutes;
	int32 points;
	float x;
	float y;
	int32 zoneid;
	int32 zonedungeonid;
};
///////////////////////////////////////////////////////////////////////////////


/*
** Color_Struct
** Size: 4 bytes
** Used for convenience
** Merth: Gave struct a name so gcc 2.96 would compile
**
*/
struct Color_Struct
{
	union
	{
		struct
		{
			int8	blue;
			int8	green;
			int8	red;
			uint8	use_tint;	// if there's a tint this is FF
		} rgb;
		uint32 color;
	};
};

struct CharSelectEquip {
	//totally guessed;
	uint32 equip0;
	uint32 equip1;
	uint32 itemid;
	Color_Struct color;
};

struct CharacterSelectEntry_Struct {
/*0008*/	uint8 level;		//edi+40h
/*0009*/	uint8 haircolor;	//edi+0F0h
/*0010*/	uint8 gender;		//edi+4Ch
/*0011*/	char name[1];		//variable length, edi+0
uint8 beard;		//edi+0F1h (may be color)
uint8 hairstyle;		//edi+0ECh
uint8 face;		//edi+4Dh
CharSelectEquip	equip[9];
uint32 u13;		//edi+0E4h
uint32 u14;		//edi+0E8h
uint8 u15;//0xff
uint32 deity;		//edi+0F7h
uint32 zone;		//edi+0E0h
uint8 u18;		//edi+0F2h    seen 0x01 and 0x0
uint8 u19;		//edi+0F4h    0xff
uint32 race;		//edi+44h
uint8 gohome;		//edi+0F3h guessed, 0x1
uint8 class_;		//edi+41h
uint8 eyecolor1;		//edi+0EFh
uint8 beardcolor;		//edi+0EDh  0xff, guessed
uint8 eyecolor2;		//edi+0EEh
uint32 secondary;		//edi+0F8h
uint32 primary;		//edi+0FCh
uint32 u29;		//edi+100h
};

/*
** Character Selection Struct
**
*/
struct CharacterSelect_Struct {
/*0000*/	int32	char_count;		//number of chars in this packet
/*0004*/	int32	total_chars;	//total number of chars allowed?
/*0008*/	CharacterSelectEntry_Struct entries[0];
};

/* 
* Visible equiptment. 
* Size: 12 Octets 
*/ 
struct EquipStruct { 
/*00*/ uint32 equip0;
/*04*/ uint32 equip1;
/*08*/ uint32 itemId;
/*12*/ 
};

/*
** Generic Spawn Struct
** Length: 257 Bytes
** Fields from old struct not yet found:
**	float	size;
**	float	walkspeed;	// probably one of the ff 33 33 33 3f
**	float	runspeed;	// probably one of the ff 33 33 33 3f
**	int8	traptype;	// 65 is disarmable trap, 66 and 67 are invis triggers/traps
**	int8	npc_armor_graphic;	// 0xFF=Player, 0=none, 1=leather, 2=chain, 3=steelplate
**	int8	npc_helm_graphic;	// 0xFF=Player, 0=none, 1=leather, 2=chain, 3=steelplate
**
*/

/* 
** Generic Spawn Struct 
** Length: 383 Octets 
** Used in: 
**   spawnZoneStruct
**   dbSpawnStruct
**   petStruct
**   newSpawnStruct
*/
/*
showeq -> eqemu
sed -e 's/_t//g' -e 's/seto_0xFF/set_to_0xFF/g'
*/
struct Spawn_Struct {
/*0000*/ uint8 unknown0000[4];
/*0004*/ float    size;           // Model size
/*0008*/ uint8 unknown0008[4];
/*0012*/ char     lastName[32];   // Player's Lastname
/*0044*/ uint32 race;           // Spawn race
/*0048*/ uint8 unknown0048[7];
/*0055*/ float    runspeed;       // Speed when running
/*0059*/ uint8 unknown0059[9];
/*0068*/ float    walkspeed;      // Speed when walking
/*0072*/ int16  deity;          // Player's Deity
/*0074*/ uint8 unknown0074[2];
/*0076*/ uint8  curHp;          // Current hp
/*0077*/ uint8 unknown0077;
/*0078*/ int8   aaitle;       // 0=none, 1=general, 2=archtype, 3=class
/*0079*/ uint8 unknown0079[26];
/*0105*/ uint8  class_;         // Player's class
/*0106*/ uint8 unknown0106;
/*0107*/ uint8 unknown0107;
/*0108*/ uint8 beardcolor;
/*0109*/ uint8 beard;		//not sure.
/*0110*/ uint8 unknown0110;
/*0111*/ uint8 unknown0111[11];
/*0122*/ char     title[32];      // Title
/*0154*/ uint8 unknown0154;
/*0155*/ union 
         {
             struct
             {
               /*0155*/ EquipStruct equip_helmet; // Equiptment: Helmet visual
               /*0167*/ EquipStruct equip_chest; // Equiptment: Chest visual
               /*0179*/ EquipStruct equip_arms; // Equiptment: Arms visual
               /*0191*/ EquipStruct equip_bracers; // Equiptment: Wrist visual
               /*0203*/ EquipStruct equip_hands; // Equiptment: Hands visual
               /*0215*/ EquipStruct equip_legs; // Equiptment: Legs visual
               /*0227*/ EquipStruct equip_feet; // Equiptment: Boots visual
               /*0239*/ EquipStruct equip_primary; // Equiptment: Main visual
               /*0251*/ EquipStruct equip_secondary; // Equiptment: Off visual
             } equip;
             /*0155*/ EquipStruct equipment[9];
         };
/*0263*/ uint8 unknown0263[14];
/*0277*/ uint32 guildID;        // Current guild
/*0281*/ uint8 unknown0281[5];
/*0286*/ signed   y:19;           // y coord
         signed   deltaZ:13;      // change in z
/*0290*/ signed   z:19;           // z coord
         signed   animation:10;   // animation
         signed   padding0290:3;  // ***Placeholder
/*0294*/ signed   deltaHeading:10;// change in heading
         signed   deltaY:13;      // change in y
         signed   padding0294:9; // ***Placeholder
/*0298*/ signed   x:19;           // x coord
         signed   padding0298:1;  // ***Placeholder
         unsigned heading:12;     // heading
/*0302*/ signed   deltaX:13;      // change in x
         signed   padding0302:19;  // ***Placeholder
/*0306*/ uint8 eyecolor1;	//not sure, may be face
/*0307*/ uint8 unknown0307;
/*0308*/ uint8 gender;         // Gender (0=male, 1=female)
/*0309*/ uint8 unknown0309[132];
/*0441*/ uint8 face;		//not sure, may be swapped with an eye color
/*0442*/ uint8 unknown442;		//saw 0x01
/*0443*/ uint8 unknown443[4];
/*0447*/ uint8 gm;             // 0=no, 1=gm
/*0448*/ uint8 unknown0448[11];
/*0459*/ union 
         {
             struct 
             {
                 /*0459*/ Color_Struct color_helmet;    // Color of helmet item
                 /*0463*/ Color_Struct color_chest;     // Color of chest item
                 /*0467*/ Color_Struct color_arms;      // Color of arms item
                 /*0471*/ Color_Struct color_bracers;   // Color of bracers item
                 /*0475*/ Color_Struct color_hands;     // Color of hands item
                 /*0479*/ Color_Struct color_legs;      // Color of legs item
                 /*0483*/ Color_Struct color_feet;      // Color of feet item
                 /*0487*/ Color_Struct color_primary;   // Color of primary item
                 /*0491*/ Color_Struct color_secondary; // Color of secondary item
             } equipment_colors;
             /*0459*/ Color_Struct colors[9]; // Array elements correspond to struct equipment_colors above
         };
/*0495*/ uint8 eyecolor2;		//not sure, may be face
/*0496*/ uint8 unknown0496[199];
/*0695*/ char     name[64];       // Player's Name
/*0759*/ uint8  level;          // Spawn Level
/*0760*/ uint8 unknown0760;
/*0761*/ uint32 petOwnerId;     // If this is a pet, the spawn id of owner
/*0765*/ uint8  NPC;            // 0=player,1=npc,2=pc corpse,3=npc corpse
/*0766*/ uint8 set_to_0xFF[8];
/*0774*/ uint8 haircolor;	//may be swapped with beardcolor
/*0775*/ uint32 spawnId;        // Spawn Id
/*0779*/ uint8 unknown0779[40];
/*0819*/ uint8  bodytype;       // Bodytype
/*0820*/ uint8 unknown0820[11];
/*0831*/ uint8  light;          // Spawn's lightsource

/*0832*/ uint8 unknown0832[30];
/* wrong, just need them to exist:*/
uint8 invis;
uint8 lfg;
uint8 hairstyle;
uint8 findable;
uint8 equip_chest2;
uint8 helm;

/*0868*/ char     suffix[32];     // Player's suffix (of Veeshan, etc.)
/*0900*/
};

/*
** New Spawn
** Length: 176 Bytes
** OpCode: 4921
*/
struct NewSpawn_Struct
{
	struct Spawn_Struct spawn;	// Spawn Information
};

struct ClientZoneEntry_Struct {
/*0000*/	uint32	unknown00;
/*0004*/	char	char_name[64];			// Character Name
};

/*
** Server Zone Entry Struct
** Length: 452 Bytes
** OPCodes: OP_ServerZoneEntry
**
*/
struct ServerZoneEntry_Struct
{
	struct NewSpawn_Struct player;
};

struct NewZone_Struct {
/*0000*/	char	char_name[64];			// Character Name
/*0064*/	char	zone_short_name[32];	// Zone Short Name
/*0096*/	char    unknown0096[96];
/*0192*/	char	zone_long_name[278];	// Zone Long Name
/*0470*/	uint8	ztype;					// Zone type (usually FF)
/*0471*/	uint8	fog_red[4];				// Zone fog (red)
/*0475*/	uint8	fog_green[4];			// Zone fog (green)
/*0479*/	uint8	fog_blue[4];			// Zone fog (blue)
/*0483*/	uint8	unknown323;
/*0484*/	float	fog_minclip[4];
/*0500*/	float	fog_maxclip[4];
/*0516*/	float	gravity;
/*0520*/	int8	time_type;
/*0521*/	uint8	unknown521[49];
/*0570*/	uint8	sky;					// Sky Type
/*0571*/	uint8	unknown571[13];			// ***Placeholder
/*0584*/	float	zone_exp_multiplier;	// Experience Multiplier
/*0588*/	float	safe_y;					// Zone Safe Y
/*0592*/	float	safe_x;					// Zone Safe X
/*0596*/	float	safe_z;					// Zone Safe Z
/*0600*/	float	max_z;					// Guessed
/*0604*/	float	underworld;				// Underworld, min z (Not Sure?)
/*0608*/	float	minclip;				// Minimum View Distance
/*0612*/	float	maxclip;				// Maximum View DIstance
/*0616*/	int8	unknown_end[84];		// ***Placeholder
/*0700*/	char	zone_short_name2[68];	//zone file name? excludes instance number which can be in previous version.
/*0768*/	char	unknown672[28];
/*0796*/	sint32	unknown796;	//seen -1
/*0800*/	char	unknown800[40];
/*0840*/	sint32	unknown840;	//seen 600
/*0844*/	sint32	unknown844;
/*0848*/	uint16	zone_id;
/*0850*/	uint16	zone_instance;
/*0852*/	char	unknown852[24];
/*0876*/	uint32	unknown876;	//seen 50
/*0880*/	uint32	unknown880;	//seen 10
/*0884*/	uint8	unknown884;	//seen 1
/*0885*/	uint8	unknown885;	//seen 0 (POK) or 1 (rujj)
/*0886*/	uint8	unknown886;	//seen 1
/*0887*/	uint8	unknown887;	//seen 0
/*0888*/	uint32	unknown888;	//seen 0
/*0892*/	uint32	unknown892;	//seen 180
/*0896*/	uint32	unknown896;	//seen 180
/*0900*/	uint32	unknown900;	//seen 180
/*0904*/	uint32	unknown904;	//seen 2
/*0908*/	uint32	unknown908;	//seen 2
/*0912*/
};

/*
** Memorize Spell Struct
** Length: 12 Bytes
**
*/
struct MemorizeSpell_Struct { 
int32 slot;     // Spot in the spell book/memorized slot 
int32 spell_id; // Spell id (200 or c8 is minor healing, etc) 
int32 scribing; // 1 if memorizing a spell, set to 0 if scribing to book, 2 if un-memming
int32 unknown12;
};

/*
** Make Charmed Pet
** Length: 12 Bytes
**
*/
struct Charm_Struct {
/*00*/	int32	owner_id;
/*04*/	int32	pet_id;
/*08*/	int32	command;    // 1: make pet, 0: release pet
/*12*/
};

struct InterruptCast_Struct
{
	int32 spawnid;
	int32 messageid;
	char	message[0];
};

struct DeleteSpell_Struct
{
/*000*/sint16	spell_slot;
/*002*/int8	unknowndss002[2];
/*004*/int8	success;
/*005*/int8	unknowndss006[3];
/*008*/
};

struct ManaChange_Struct
{                  
	int32	new_mana;                  // New Mana AMount
	int32	stamina;
	int32	spell_id;
	int32	unknown12;
	int32	unknown16;
};

struct SwapSpell_Struct 
{ 
	int32 from_slot; 
	int32 to_slot; 


}; 

struct BeginCast_Struct
{
	// len = 8
/*000*/	int16	caster_id;
/*002*/	int16	spell_id;
/*004*/	int32	cast_time;		// in miliseconds
};

struct CastSpell_Struct
{
	int32	slot;
	int32	spell_id;
	int32	inventoryslot;  // slot for clicky item, 0xFFFF = normal cast
	int32	target_id;
	int8    cs_unknown[4];
};

/*
** SpawnAppearance_Struct
** Changes client appearance for all other clients in zone
** Size: 8 bytes
** Used in: OP_SpawnAppearance
**
*/
struct SpawnAppearance_Struct
{
/*0000*/ uint16 spawn_id;          // ID of the spawn
/*0002*/ uint16 type;              // Values associated with the type
/*0004*/ uint32 parameter;         // Type of data sent
/*0008*/
};


// solar: this is used inside profile
struct SpellBuff_Struct
{
/*000*/	int8	slotid;		//badly named... seems to be 2 for a real buff, 0 otherwise
/*001*/ int8	level;
/*002*/	int8	bard_modifier;
/*003*/	int8	effect;			//not real
/*004*/	int32	spellid;
/*008*/ int32	duration;
/*012*/	int32	counters;
/*016*/	int32	player_id;	//'global' ID of the caster, for wearoff messages
};

struct SpellBuffFade_Struct {
/*000*/	uint32 entityid;
/*004*/	int8 slot;
/*005*/	int8 level;
/*006*/	int8 effect;
/*007*/	int8 unknown7;
/*008*/	uint32 spellid;
/*012*/	uint32 duration;
/*016*/	uint32 unknown016;
/*020*/	uint32 unknown020;	//prolly global player ID
/*024*/	uint32 slotid;
/*028*/	uint32 bufffade;
/*032*/
};

struct ItemNamePacket_Struct {
/*000*/	uint32 item_id;
/*004*/	uint32 unkown004;
/*008*/ char name[64];
/*072*/
};

// Length: 10
struct ItemProperties_Struct {

int8	unknown01[2];
int8	charges;
int8	unknown02[13];
};

struct GMTrainee_Struct
{
	/*000*/ uint32 npcid;
	/*004*/ uint32 playerid;
	/*008*/ uint32 skills[73];
	/*300*/ int8 unknown300[148];
	/*448*/
};

struct GMTrainEnd_Struct
{
	/*000*/ uint32 npcid;
	/*004*/ uint32 playerid;
	/*008*/
};

struct GMSkillChange_Struct {
/*000*/	int16		npcid;
/*002*/ int8		unknown1[2];    // something like PC_ID, but not really. stays the same thru the session though
/*002*/ int16       skillbank;      // 0 if normal skills, 1 if languages
/*002*/ int8		unknown2[2];
/*008*/ uint16		skill_id;
/*010*/ int8		unknown3[2];
};
struct ConsentResponse_Struct {
	char grantname[64];
	char ownername[64];
	int8 permission;
	char zonename[32];
};

/*
** Name Generator Struct
** Length: 72 bytes
** OpCode: 0x0290
*/
struct NameGeneration_Struct
{
/*0000*/	int32	race;
/*0004*/	int32	gender;
/*0008*/	char	name[64];
/*0072*/
};

/*
** Character Creation struct
** Length: 140 Bytes
** OpCode: 0x0113
*/
struct CharCreate_Struct
{
/*0000*/	int32	class_;
/*0004*/	int32	haircolor;	// Might be hairstyle
/*0008*/	int32	beardcolor;	// Might be beard
/*0012*/	int32	beard;		// Might be beardcolor
/*0016*/	int32	gender;
/*0020*/	int32	race;
/*0024*/	int32	start_zone;
	// 0 = odus
	// 1 = qeynos
	// 2 = halas
	// 3 = rivervale
	// 4 = freeport
	// 5 = neriak
	// 6 = gukta/grobb
	// 7 = ogguk
	// 8 = kaladim
	// 9 = gfay
	// 10 = felwithe
	// 11 = akanon
	// 12 = cabalis
	// 13 = shar vahl
/*0028*/	int32	hairstyle;	// Might be haircolor
/*0032*/	int32	deity;
/*0036*/	int32	STR;
/*0040*/	int32	STA;
/*0044*/	int32	AGI;
/*0048*/	int32	DEX;
/*0052*/	int32	WIS;
/*0056*/	int32	INT;
/*0060*/	int32	CHA;
/*0064*/	int32	face;		// Could be unknown0076
/*0068*/	int32	eyecolor1;	//its possiable we could have these switched
/*0073*/	int32	eyecolor2;	//since setting one sets the other we really can't check
/*0076*/	int32	unknown0076;
/*0080*/	int32	unknown0080;
/*0084*/	int32	unknown0084;
/*0088*/	int32	unknown0088;
/*0092*/
};

/*
 *Used in PlayerProfile
 */
struct AA_Array
{
	int32 AA;
	int32 value;	
};


static const uint32 MAX_PP_DISCIPLINES = 100;

struct Disciplines_Struct {
	uint32 values[MAX_PP_DISCIPLINES];
};

static const uint32 MAX_PLAYER_TRIBUTES = 5;
static const uint32 MAX_PLAYER_BANDOLIER = 20;
static const uint32 MAX_PLAYER_BANDOLIER_ITEMS = 4;
static const uint32 MAX_POTIONS_IN_BELT = 4;
static const uint32 TRIBUTE_NONE = 0xFFFFFFFF;
struct Tribute_Struct {
	uint32 tribute;
	uint32 tier;
};

//len = 72
struct BandolierItem_Struct {
	uint32 item_id;
	uint32 icon;
	char item_name[64];
};

//len = 320
enum { //bandolier item positions
	bandolierMainHand = 0,
	bandolierOffHand,
	bandolierRange,
	bandolierAmmo
};
struct Bandolier_Struct {
	char name[32];
	BandolierItem_Struct items[MAX_PLAYER_BANDOLIER_ITEMS];
};
struct PotionBelt_Struct {
	BandolierItem_Struct items[MAX_POTIONS_IN_BELT];
};
	
static const uint32 MAX_GROUP_LEADERSHIP_AA_ARRAY = 16;
static const uint32 MAX_RAID_LEADERSHIP_AA_ARRAY = 16;
static const uint32 MAX_LEADERSHIP_AA_ARRAY = (MAX_GROUP_LEADERSHIP_AA_ARRAY+MAX_RAID_LEADERSHIP_AA_ARRAY);
struct LeadershipAA_Struct {
	uint32 ranks[MAX_LEADERSHIP_AA_ARRAY];
};
struct GroupLeadershipAA_Struct {
	uint32 ranks[MAX_GROUP_LEADERSHIP_AA_ARRAY];
};
struct RaidLeadershipAA_Struct {
	uint32 ranks[MAX_RAID_LEADERSHIP_AA_ARRAY];
};

 /** 
* A bind point. 
* Size: 20 Octets 
*/ 
struct BindStruct { 
   /*000*/ uint32 zoneId; 
   /*004*/ float x; 
   /*008*/ float y; 
   /*012*/ float z; 
   /*016*/ float heading; 
   /*020*/ 
}; 


/*
** Player Profile
**
** Length: 4308 bytes
** OpCode: 0x006a
 */
static const uint32 MAX_PP_LANGUAGE		= 28;
static const uint32 MAX_PP_SPELLBOOK	= 400;
static const uint32 MAX_PP_MEMSPELL		= 9;
static const uint32 MAX_PP_SKILL		= 75;
static const uint32 MAX_PP_AA_ARRAY		= 359;
static const uint32 MAX_GROUP_MEMBERS	= 6;
static const uint32 MAX_RECAST_TYPES	= 20;
/*
showeq -> eqemu
sed -e 's/_t//g' -e 's/MAX_AA/MAX_PP_AA_ARRAY/g' \
    -e 's/MAX_SPELL_SLOTS/MAX_PP_MEMSPELL/g' \
    -e 's/MAX_KNOWN_SKILLS/MAX_PP_SKILL/g' \
    -e 's/MAXRIBUTES/MAX_PLAYER_TRIBUTES/g' \
    -e 's/MAX_BUFFS/BUFF_COUNT/g' \
    -e 's/MAX_KNOWN_LANGS/MAX_PP_LANGUAGE/g' \
    -e 's/MAX_RECASTYPES/MAX_RECAST_TYPES/g' \
    -e 's/spellBuff/SpellBuff_Struct/g' \
    -e 's/lastName/last_name/g' \
    -e 's/guildID/guild_id/g' \
    -e 's/itemint/item_tint/g' \
    -e 's/MANA/mana/g' \
    -e 's/curHp/cur_hp/g' \
    -e 's/sSpellBook/spell_book/g' \
    -e 's/sMemSpells/mem_spells/g' \
    -e 's/uint32[ \t]*disciplines\[MAX_DISCIPLINES\]/Disciplines_Struct  disciplines/g' \
    -e 's/aa_unspent/aapoints/g' \
    -e 's/aa_spent/aapoints_spent/g' \
    -e 's/InlineItem[ \t]*potionBelt\[MAX_POTIONS_IN_BELT\]/PotionBelt_Struct  potionbelt/g' \
    -e 's/ldon_guk_points/ldon_points_guk/g' \
    -e 's/ldon_mir_points/ldon_points_mir/g' \
    -e 's/ldon_mmc_points/ldon_points_mmc/g' \
    -e 's/ldon_ruj_points/ldon_points_ruj/g' \
    -e 's/ldonak_points/ldon_points_tak/g' \
    -e 's/ldon_avail_points/ldon_points_available/g' \
    -e 's/tributeTime/tribute_time_remaining/g' \
    -e 's/careerTribute/career_tribute_points/g' \
    -e 's/currentTribute/tribute_points/g' \
    -e 's/tributeActive/tribute_active/g' \
    -e 's/TributeStruct/Tribute_Struct/g' \
    -e 's/expGroupLeadAA/group_leadership_exp/g' \
    -e 's/expRaidLeadAA/raid_leadership_exp/g' \
    -e 's/groupLeadAAUnspent/group_leadership_points/g' \
    -e 's/raidLeadAAUnspent/raid_leadership_points/g' \
    -e 's/uint32[ \t]*leadershipAAs\[MAX_LEAD_AA\]/LeadershipAA_Struct leader_abilities/g' \
    -e 's/BandolierStruct/Bandolier_Struct/g' \
    -e 's/MAX_BANDOLIERS/MAX_PLAYER_BANDOLIER/g' \
    -e 's/birthdayTime/birthday/g' \
    -e 's/lastSaveTime/lastlogin/g' \
    -e 's/zoneId/zone_id/g' \
    -e 's/hunger/hunger_level/g' \
    -e 's/thirst/thirst_level/g' \
    -e 's/guildstatus/guildrank/g' \
    -e 's/airRemaining/air_remaining/g' \
    
    

 */
struct PlayerProfile_Struct
{
/*00000*/ uint32  checksum;           //
//BEGIN SUB-STRUCT used for shrouding stuff...
/*00004*/ uint32  gender;             // Player Gender - 0 Male, 1 Female
/*00008*/ uint32  race;               // Player race
/*00012*/ uint32  class_;             // Player class
/*00016*/ uint8   unknown00016[40];       // ***Placeholder
/*00056*/ uint8   level;              // Level of player
/*00057*/ uint8   level1;             // Level of player (again?)
/*00058*/ uint8   unknown00022[2];    // ***Placeholder
/*00060*/ BindStruct binds[5];          // Bind points (primary is first)
/*00160*/ uint32  deity;              // deity
/*00164*/ uint32  intoxication;       // Alcohol level (in ticks till sober?)
/*00168*/ uint32  spellSlotRefresh[MAX_PP_MEMSPELL]; // Refresh time (millis)
/*00204*/ uint8 unknown0166[4];
/*00208*/ uint8   haircolor;          // Player hair color
/*00209*/ uint8   beardcolor;         // Player beard color
/*00210*/ uint8   eyecolor1;          // Player left eye color
/*00211*/ uint8   eyecolor2;          // Player right eye color
/*00212*/ uint8   hairstyle;          // Player hair style
/*00213*/ uint8   beard;              // Player beard type
/*00214*/ uint8 unknown00178[10];
//NOTE: this prolly changed to the new equip1/2/itemid format like the spawn struct and char select stuff.
/*00224*/ uint32  item_material[9];   // Item texture/material of worn items
/*00260*/ uint8 unknown00224[224];
/*00484*/ Color_Struct item_tint[9];    // RR GG BB 00
/*00520*/ AA_Array  aa_array[MAX_PP_AA_ARRAY];   // AAs
/*03392*/ uint8 unknown02220[16];
/*03408*/ uint32  points;             // Unspent Practice points
/*03412*/ uint32  mana;               // Current mana
/*03416*/ uint32  cur_hp;              // Current HP without +HP equipment
/*03420*/ uint32  STR;                // Strength
/*03424*/ uint32  STA;                // Stamina
/*03428*/ uint32  CHA;                // Charisma
/*03432*/ uint32  DEX;                // Dexterity
/*03436*/ uint32  INT;                // Intelligence
/*03440*/ uint32  AGI;                // Agility
/*03444*/ uint32  WIS;                // Wisdom
/*03448*/ uint8   face;               // Player face
/*03449*/ uint8 unknown02264[147];
/*03596*/ int32   spell_book[MAX_PP_SPELLBOOK];    // List of the Spells in spellbook
/*05196*/ uint8   unknown4184[448];   // all 0xff after last spell    
/*05644*/ int32   mem_spells[MAX_PP_MEMSPELL]; // List of spells memorized
/*05680*/ uint8 unknown04396[32];
/*05712*/ uint32  platinum;           // Platinum Pieces on player
/*05716*/ uint32  gold;               // Gold Pieces on player
/*05720*/ uint32  silver;             // Silver Pieces on player
/*05724*/ uint32  copper;             // Copper Pieces on player
/*05728*/ uint32  platinum_cursor;    // Platinum Pieces on cursor
/*05732*/ uint32  gold_cursor;        // Gold Pieces on cursor
/*05736*/ uint32  silver_cursor;      // Silver Pieces on cursor
/*05740*/ uint32  copper_cursor;      // Copper Pieces on cursor
/*05744*/ uint32  skills[MAX_PP_SKILL]; // List of skills 
/*06044*/ uint8 unknown04760[236];
/*06280*/ uint32  toxicity;           // Potion Toxicity (15=too toxic, each potion adds 3)
/*06284*/ uint32  thirst_level;             // Drink (ticks till next drink)
/*06288*/ uint32  hunger_level;             // Food (ticks till next eat)
/*06292*/ SpellBuff_Struct buffs[BUFF_COUNT];   // Buffs currently on the player
/*06792*/ Disciplines_Struct  disciplines; // Known disciplines
/*07192*/ uint8 unknown05008[160];
/*07352*/ uint32 recastTimers[MAX_RECAST_TYPES]; // Timers (GMT of last use)
/*07432*/ uint32  endurance;          // Current endurance
/*07436*/ uint32  aapoints_spent;           // Number of spent AA points
/*07440*/ uint32  aapoints;         // Unspent AA points
/*07444*/ uint8 unknown06160[4];
/*07448*/ Bandolier_Struct bandoliers[MAX_PLAYER_BANDOLIER]; // bandolier contents
/*13848*/ PotionBelt_Struct  potionbelt; // potion belt
/*14136*/ uint8 unknown12852[8];
/*14144*/ uint32 available_slots;
/*14148*/ uint8 unknown12864[80];
//END SUB-STRUCT used for shrouding.
/*14228*/ char    name[64];           // Name of player
/*14292*/ char    last_name[32];       // Last name of player
/*14324*/ uint8   unknown14324[8];
/*14332*/ int32   guild_id;            // guildid
/*14336*/ uint32  birthday;       // character birthday
/*14340*/ uint32  lastlogin;       // character last save time
/*14344*/ uint32  timePlayedMin;      // time character played
/*14348*/ uint8   pvp;                // 1=pvp, 0=not pvp
/*14349*/ uint8   anon;               // 2=roleplay, 1=anon, 0=not anon     
/*14350*/ uint8   gm;                 // 0=no, 1=yes (guessing!)
/*14351*/ int8    guildrank;        // 0=member, 1=officer, 2=guildleader
/*14352*/ uint8 unknown13054[12];
/*14364*/ uint32  exp;                // Current Experience
/*14368*/ uint8 unknown13072[12];
/*14380*/ uint8   languages[MAX_PP_LANGUAGE]; // List of languages
/*14405*/ uint8   unknown13109[4];    // All 0x00 (language buffer?)
/*14412*/ float     y;                  // Players y position (NOT positive about this switch)
/*14416*/ float     x;                  // Players x position
/*14420*/ float     z;                  // Players z position
/*14424*/ float     heading;            // Players heading   
/*14428*/ uint8   unknown13132[4];    // ***Placeholder
/*14432*/ uint32  platinum_bank;      // Platinum Pieces in Bank
/*14436*/ uint32  gold_bank;          // Gold Pieces in Bank
/*14440*/ uint32  silver_bank;        // Silver Pieces in Bank
/*14444*/ uint32  copper_bank;        // Copper Pieces in Bank
/*14448*/ uint32  platinum_shared;    // Shared platinum pieces
/*14452*/ uint8 unknown13156[84];
/*14536*/ uint32  expansions;         // Bitmask for expansions
/*14540*/ uint8 unknown13244[12];
/*14552*/ uint32  autosplit;          // 0 = off, 1 = on
/*14556*/ uint8 unknown13260[16];
/*14572*/ uint16  zone_id;             // see zones.h
/*14574*/ uint16  zoneInstance;       // Instance id
/*14576*/ char      groupMembers[MAX_GROUP_MEMBERS][64];// all the members in group, including self 
/*14960*/ char      groupLeader[64];    // Leader of the group ?
/*15024*/ uint8 unknown13728[660];
/*15684*/ uint32  leadAAActive;       // 0 = leader AA off, 1 = leader AA on
/*15688*/ uint8 unknown14392[136];
/*15824*/ sint32  ldon_points_guk;    // Earned GUK points
/*15828*/ sint32  ldon_points_mir;    // Earned MIR points
/*15832*/ sint32  ldon_points_mmc;    // Earned MMC points
/*15836*/ sint32  ldon_points_ruj;    // Earned RUJ points
/*15840*/ sint32  ldon_points_tak;    // Earned TAK points
/*15844*/ sint32  ldon_points_available;  // Available LDON points
/*15848*/ uint8 unknown14420[136];
/*15984*/ uint32  tribute_time_remaining;        // Time remaining on tribute (millisecs)
/*15988*/ uint32  career_tribute_points;      // Total favor points for this char
/*15992*/ uint32  unknown7208;        // *** Placeholder
/*15996*/ uint32  tribute_points;     // Current tribute points
/*16000*/ uint32  unknown7216;        // *** Placeholder
/*16004*/ uint32  tribute_active;      // 0 = off, 1=on
/*16008*/ Tribute_Struct tributes[MAX_PLAYER_TRIBUTES]; // Current tribute loadout
/*16048*/ uint8 unknown14616[8];
/*16056*/ uint32  group_leadership_exp;     // Current group lead exp points (format though??)
/*16060*/ uint32 unknown14628;
/*16064*/ uint32  raid_leadership_exp;      // Current raid lead AA exp points (format though??)
/*16068*/ uint32  group_leadership_points; // Unspent group lead AA points
/*16072*/ uint32  raid_leadership_points;  // Unspent raid lead AA points
/*16076*/ LeadershipAA_Struct leader_abilities; // Leader AA ranks
/*16204*/ uint8 unknown14772[128];
/*16332*/ uint32  air_remaining;       // Air supply (seconds)
/*16336*/ uint8 unknown14904[4608];
/*20944*/ uint32 expAA;               // Exp earned in current AA point
/*20948*/ uint8 unknown19516[40];
/*20988*/ uint32 currentRadCrystals;  // Current count of radiant crystals
/*20992*/ uint32 careerRadCrystals;   // Total count of radiant crystals ever
/*20996*/ uint32 currentEbonCrystals; // Current count of ebon crystals
/*21000*/ uint32 careerEbonCrystals;  // Total count of ebon crystals ever
/*21004*/ uint8  groupAutoconsent;    // 0=off, 1=on
/*21005*/ uint8  raidAutoconsent;     // 0=off, 1=on
/*21006*/ uint8  guildAutoconsent;    // 0=off, 1=on
/*21007*/ uint8  unknown19575[5];     // ***Placeholder (6/29/2005)
/*21012*/ uint32 showhelm;            // 0=no, 1=yes
/*21016*/ uint8  unknown19584[4];     // ***Placeholder (10/27/2005)
/*21020*/ uint32 unknown19588;        // *** Placeholder
/*21024*/ uint8	 unknown19592[1024];
/*22048*/
};





/*
** Client Target Struct
** Length: 2 Bytes
** OpCode: 6221
*/
struct ClientTarget_Struct {
/*000*/	int32	new_target;			// Target ID
};

/*
** Target Rejection Struct
** Length: 12 Bytes
** OpCode: OP_TargetReject
*/
struct TargetReject_Struct {
/*00*/	uint8	unknown00[12];
};

struct PetCommand_Struct {
/*000*/ int32	command;
/*004*/ int32	unknown;
};

/*
** Delete Spawn
** Length: 4 Bytes
** OpCode: OP_DeleteSpawn
*/
struct DeleteSpawn_Struct
{
/*00*/ int32 spawn_id;             // Spawn ID to delete
/*04*/
};

/*
** Channel Message received or sent
** Length: 144 Bytes + Variable Length + 1
** OpCode: OP_ChannelMessage
**
*/
struct ChannelMessage_Struct
{
/*000*/	char	targetname[64];		// Tell recipient
/*064*/	char	sender[64];			// The senders name (len might be wrong)
/*128*/	int32	language;			// Language
/*132*/	int32	chan_num;			// Channel
/*136*/	int32	cm_unknown4[2];		// ***Placeholder
/*144*/	int32	skill_in_language;	// The players skill in this language? might be wrong
/*148*/	char	message[0];			// Variable length message
};

/*
** Special Message
** Length: 4 Bytes + Variable Text Length + 1
** OpCode: OP_SpecialMesg
**
*/
/*
	Theres something wrong with this... example live packet:
Server->Client: [ Opcode: OP_SpecialMesg (0x0fab) Size: 244 ]
   0: 01 02 00 0A 00 00 00 09 - 05 00 00 42 61 72 73 74  | ...........Barst
  16: 72 65 20 53 6F 6E 67 77 - 65 61 76 65 72 00 7C F9  | re Songweaver.|.
  32: FF FF 84 FF FF FF 03 00 - 00 00 47 72 65 65 74 69  | ..........Greeti

*/
struct SpecialMesg_Struct
{
/*00*/	char	header[3];				// 04 04 00 <-- for #emote style msg
/*03*/	uint32	msg_type;				// Color of text (see MT_*** below)
/*07*/	uint32	target_spawn_id;		// Who is it being said to?
/*11*/	char	sayer[1];				// Who is the source of the info
/*12*/	uint8	unknown12[12];
/*24*/	char	message[1];				// What is being said?
};

/*
** When somebody changes what they're wearing
**      or give a pet a weapon (model changes)
** Length: 16 Bytes
*/
struct WearChange_Struct{
/*000*/ int16 spawn_id;
/*002*/ int16 material;
/*004*/ Color_Struct color;
/*008*/ int8 wear_slot_id;
/*009*/ uint8 unknown09[10];
};

/*
** Type:   Bind Wound Structure
** Length: 8 Bytes
*/
//Fixed for 7-14-04 patch
struct BindWound_Struct
{
/*002*/	int16	to;			// TargetID
/*004*/	int16	unknown2;		// ***Placeholder
/*006*/	int16	type;
/*008*/	int16	unknown6;
};


/*
** Type:   Zone Change Request (before hand)
** Length: 88 bytes 
** OpCode: a320
*/

struct ZoneChange_Struct {
/*000*/	char	char_name[64];     // Character Name
/*064*/	uint16	zoneID;
/*066*/	uint16	instanceID;
/*068*/	float	y;
/*072*/	float	x;
/*076*/	float	z;
/*080*/	int32	zone_reason;	//0x0A == death, I think
/*084*/	sint32	success;		// =0 client->server, =1 server->client, -X=specific error
/*088*/
};

struct RequestClientZoneChange_Struct {
/*00*/	uint16	zone_id;
/*02*/	uint16	instance_id;
/*04*/	float	y;
/*08*/	float	x;
/*12*/	float	z;
/*16*/	float	heading;
/*20*/	uint32	type;	//unknown... values 
};

struct Animation_Struct {
/*00*/	int16 spawnid;
/*02*/	int8 action;
/*03*/	int8 value;
/*04*/
};

// solar: this is what causes the caster to animate and the target to
// get the particle effects around them when a spell is cast
// also causes a buff icon
struct Action_Struct
{
 /* 00 */	int16 target;	// id of target
 /* 02 */	int16 source;	// id of caster
 /* 04 */	uint16 level; // level of caster
 /* 06 */	uint16 unknown06;	// seems to be fixed to 0x0A
 /* 08 */	uint32 unknown08;
 /* 12 */	uint16 unknown16;
// some kind of sequence that's the same in both actions
// as well as the combat damage, to tie em together?
 /* 14 */	int32 sequence;	
 /* 18 */	uint32 unknown18;
 /* 22 */	int8 type;		// 231 (0xE7) for spells
 /* 23 */	uint32 unknown23;
 /* 27 */	int16 spell;	// spell id being cast
 /* 29 */	int8 unknown29;
// this field seems to be some sort of success flag, if it's 4
 /* 30 */	int8 buff_unknown;	// if this is 4, a buff icon is made
 /* 31 */
};

// solar: this is what prints the You have been struck. and the regular
// melee messages like You try to pierce, etc.  It's basically the melee
// and spell damage message
struct CombatDamage_Struct
{
/* 00 */	int16	target;
/* 02 */	int16	source;
/* 04 */	int8	type; //slashing, etc.  231 (0xE7) for spells
/* 05 */	int16	spellid;
/* 07 */	int32	damage;
/* 11 */	int32 unknown11;
/* 15 */	int32 sequence;	// see above notes in Action_Struct
/* 19 */	int32	unknown19;
/* 23 */
};

/*
** Consider Struct
** Length: 24 Bytes
** OpCode: 3721
*/
struct Consider_Struct{
/*000*/ uint32	playerid;               // PlayerID
/*004*/ uint32	targetid;               // TargetID
/*008*/ int32	faction;                // Faction
/*0012*/ int32	level;                  // Level
/*016*/ sint32	cur_hp;                  // Current Hitpoints
/*020*/ sint32	max_hp;                  // Maximum Hitpoints
/*024*/ int8 pvpcon;     // Pvp con flag 0/1
/*025*/ int8	unknown3[3];
};

/*
** Spawn Death Blow
** Length: 32 Bytes
** OpCode: 0114
*/
struct Death_Struct
{
/*000*/	int32	spawn_id;
/*004*/	int32	killer_id;
/*008*/	int32	corpseid;	// was corpseid
/*012*/	int32	attack_skill;	// was type
/*016*/	int32	spell_id;
/*020*/ int32	bindzoneid;	//bindzoneid?
/*024*/	int32	damage;
/*028*/	int32	unknown028;
};

struct BecomeCorpse_Struct {
	int32	spawn_id;
	float y;
	float x;
	float z;
};

/*
** Spawn position update
**	Struct sent from server->client to update position of
**	another spawn's position update in zone (whether NPC or PC)
**
*/
struct PlayerPositionUpdateServer_Struct
{
/*0000*/ uint16		spawn_id;
/*0002*/ signed   		padding1:12;    // ***Placeholder
         signed   delta_z:13;      // change in z
         signed   		padding2:7;     // ***Placeholder
/*0006*/ signed   y_pos:19;           // y coord
         signed   		padding3:13;    // ***Placeholder
/*0010*/ signed   z_pos:19;           // z coord
         signed   animation:10;   // animation
         signed   		padding4:3;     // ***Placeholder
/*0014*/ signed   delta_heading:10;// change in heading
         signed   delta_y:13;      // change in y
         signed   		padding5:9;     // ***Placeholder
/*0018*/ signed   x_pos:19;           // x coord
         unsigned heading:12;     // heading
         signed   		padding6:1;     // ***Placeholder
/*0022*/ signed   delta_x:13;      // change in x
         signed   		padding7:19;    // ***Placeholder
/*0026*/
};

/*
** Player position update
**	Struct sent from client->server to update
**	player position on server
**
*/
struct PlayerPositionUpdateClient_Struct
{
/*0000*/ uint16 spawn_id;        // Player's spawn id
/*0002*/ uint16	sequence;	//increments one each packet
/*0004*/ unsigned padding1:12;    // ***Placeholder
         unsigned heading:12;     // Directional heading
         unsigned padding2:8;     // ***Placeholder
/*0008*/ float delta_y;            // Change in y
/*0012*/ signed animation:10;     // animation
         unsigned padding3:22;    // ***Placeholder
/*0016*/ float delta_x;            // Change in x
/*0020*/ float y_pos;                 // y coord
/*0024*/ signed delta_heading:10;  // change in heading
         signed padding4:6;       // ***Placeholder (mostly 1)
/*0026*/ uint8 unknown0026[2];  // ***Placeholder
/*0028*/ float delta_z;            // Change in z
/*0032*/ float z_pos;                 // z coord
/*0036*/ float x_pos;                 // x coord
/*0040*/
};

/*
** Spawn HP Update
** Length: 10 Bytes
** OpCode: OP_HPUpdate
*/
struct SpawnHPUpdate_Struct
{
/*00*/ uint32	cur_hp;               // Id of spawn to update
/*04*/ sint32	max_hp;                 // Maximum hp of spawn
/*08*/ sint16	spawn_id;                 // Current hp of spawn
/*10*/
};
struct SpawnHPUpdate_Struct2
{
/*01*/ sint16	spawn_id;
/*00*/ int8	hp;
};
/*
** Stamina
** Length: 8 Bytes
** OpCode: 5721
*/
struct Stamina_Struct {
/*00*/ int32 food;                     // (low more hungry 127-0)
/*02*/ int32 water;                    // (low more thirsty 127-0)
};

/*
** Level Update
** Length: 12 Bytes
*/
struct LevelUpdate_Struct
{
/*00*/ uint32 level;                  // New level
/*04*/ uint32 level_old;              // Old level
/*08*/ uint32 exp;                    // Current Experience
};

/*
** Experience Update
** Length: 14 Bytes
** OpCode: 9921
*/
struct ExpUpdate_Struct
{
/*0000*/ uint32 exp;                    // Current experience ratio from 0 to 330
/*0004*/ uint32 aaxp; // @BP ??
};

/*
** Item Packet Struct - Works on a variety of opcodes
** Packet Types: See ItemPacketType enum
**
*/
enum ItemPacketType
{
	ItemPacketViewLink			= 0x00,
	ItemPacketTradeView			= 0x65,
	ItemPacketLoot				= 0x66,
	ItemPacketTrade				= 0x67,
	ItemPacketCharInventory		= 0x69,
	ItemPacketSummonItem		= 0x6A,
	ItemPacketTributeItem		= 0x6C,
	ItemPacketMerchant			= 0x64,
	ItemPacketWorldContainer	= 0x6B
};
struct ItemPacket_Struct
{
/*00*/	ItemPacketType	PacketType;
/*04*/	char			SerializedItem[1];
/*xx*/
};

struct BulkItemPacket_Struct
{
/*00*/	char			SerializedItem[0];
/*xx*/
};

struct Consume_Struct
{
/*0000*/ int32 slot;
/*0004*/ int32 auto_consumed; // 0xffffffff when auto eating e7030000 when right click
/*0008*/ int8  c_unknown1[4];
/*0012*/ int8  type; // 0x01=Food 0x02=Water
/*0013*/ int8  unknown13[3];
};


struct MoveItem_Struct
{
/*0000*/ uint32 from_slot;
/*0004*/ uint32 to_slot;
/*0008*/ uint32 number_in_stack;
};

//
// from_slot/to_slot
// -1 - destroy
//  0 - cursor
//  1 - inventory
//  2 - bank
//  3 - trade
//  4 - shared bank
//
// cointype
// 0 - copeer
// 1 - silver
// 2 - gold
// 3 - platinum
//
static const uint32 COINTYPE_PP = 3;
static const uint32 COINTYPE_GP = 2;
static const uint32 COINTYPE_SP = 1;
static const uint32 COINTYPE_CP = 0;

struct MoveCoin_Struct
{
		 sint32 from_slot;
		 sint32 to_slot;
		 sint32 cointype1;
		 sint32 cointype2;
		 sint32	amount;
};
struct TradeCoin_Struct{
	int32	trader;
	int8	slot;
	int16	unknown5;
	int8	unknown7;
	int32	amount;
};
struct TradeMoneyUpdate_Struct{
	int32	trader;
	int32	type;
	int32	amount;
};
/*
** Surname struct
** Size: 100 bytes
*/
struct Surname_Struct 
{ 
/*0000*/	char name[64];
/*0064*/	uint32 unknown0064;
/*0068*/	char lastname[32];
/*0100*/
}; 

struct GuildsListEntry_Struct {
	char name[64];
};

static const uint32 MAX_NUMBER_GUILDS = 1500;
struct GuildsList_Struct {
	int8 head[64]; // First on guild list seems to be empty...
	GuildsListEntry_Struct Guilds[MAX_NUMBER_GUILDS];
};

struct GuildUpdate_Struct {
	int32	guildID;
	GuildsListEntry_Struct entry;
};

/*
** Money Loot
** Length: 22 Bytes
** OpCode: 5020
*/
struct moneyOnCorpseStruct {
/*0000*/ uint8	response;		// 0 = someone else is, 1 = OK, 2 = not at this time
/*0001*/ uint8	unknown1;		// = 0x5a
/*0002*/ uint8	unknown2;		// = 0x40
/*0003*/ uint8	unknown3;		// = 0
/*0004*/ uint32	platinum;		// Platinum Pieces
/*0008*/ uint32	gold;			// Gold Pieces

/*0012*/ uint32	silver;			// Silver Pieces
/*0016*/ uint32	copper;			// Copper Pieces
};

//opcode = 0x5220
// size 292


struct LootingItem_Struct {
/*000*/	int32	lootee;
/*002*/	int32	looter;
/*004*/	int16	slot_id;
/*006*/	int8	unknown3[2];
/*008*/	int32	auto_loot;
};

struct GuildManageStatus_Struct{
	int32	guildid;
	int32	oldrank;
	int32	newrank;
	char	name[64];
};
// Guild invite, remove
struct GuildJoin_Struct{
/*000*/	int32	guild_id;
/*004*/	int32	unknown04;
/*008*/	int32	level;
/*012*/	int32	class_;
/*016*/	int32	rank;//0 member, 1 officer, 2 leader
/*020*/	int32	zoneid;
/*024*/	int32	unknown24;
/*028*/	char	name[64];
/*092*/
};
struct GuildInviteAccept_Struct {
	char inviter[64];
	char newmember[64];
	int32 response;
	int32 guildeqid;
};
struct GuildManageRemove_Struct {
	int32 guildeqid;
	char member[64];
};
struct GuildCommand_Struct {
	char othername[64];
	char myname[64];
	int16 guildeqid;
	int8 unknown[2]; // for guildinvite all 0's, for remove 0=0x56, 2=0x02
	int32 officer;
};

// Opcode OP_GMZoneRequest
// Size = 88 bytes
struct GMZoneRequest_Struct {
/*0000*/	char	charname[64];
/*0064*/	int32	zone_id;
/*0068*/	float	x;
/*0072*/	float	y;
/*0076*/	float	z;
/*0080*/	char	unknown0080[4];
/*0084*/	int32	success;		// 0 if command failed, 1 if succeeded?
/*0088*/
//	/*072*/	sint8	success;		// =0 client->server, =1 server->client, -X=specific error
//	/*073*/	int8	unknown0073[3]; // =0 ok, =ffffff error
};

struct GMSummon_Struct {
/*  0*/	char    charname[64];
/* 30*/	char    gmname[64];
/* 60*/ int32	success;
/* 61*/	int32	zoneID;
/*92*/	sint32  y;
/*96*/	sint32  x;
/*100*/ sint32  z;
/*104*/	int32 unknown2; // E0 E0 56 00
};

struct GMGoto_Struct { // x,y is swapped as compared to summon and makes sense as own packet
/*  0*/	char    charname[64];

/* 64*/	char    gmname[64];
/* 128*/ int32	success;
/* 132*/	int32	zoneID;

/*136*/	sint32  y;
/*140*/	sint32  x;
/*144*/ sint32  z;
/*148*/	int32 unknown2; // E0 E0 56 00
};

struct GMLastName_Struct {
	char name[64];
	char gmname[64];
	char lastname[64];
	int16 unknown[4];	// 0x00, 0x00
					    // 0x01, 0x00 = Update the clients
};

//Combat Abilities
struct CombatAbility_Struct {
	int32 m_target;		//the ID of the target mob
	int32 m_atk;
	int32 m_skill;
};

struct DeleteItem_Struct {
/*0000*/ uint32 from_slot;
/*0004*/ uint32 to_slot;
/*0008*/ uint32 number_in_stack;
};

//Instill Doubt
struct Instill_Doubt_Struct {
	int8 i_id;
	int8 ia_unknown;
	int8 ib_unknown;
	int8 ic_unknown;
	int8 i_atk;

	int8 id_unknown;
	int8 ie_unknown;
	int8 if_unknown;
	int8 i_type;
	int8 ig_unknown;
	int8 ih_unknown;
	int8 ii_unknown;
};

struct GiveItem_Struct {
	uint16 to_entity;
	sint16 to_equipSlot;
	uint16 from_entity;
	sint16 from_equipSlot;
};

struct RandomReq_Struct {
	int32 low;
	int32 high;
};

/* solar: 9/23/03 reply to /random command; struct from Zaphod */
struct RandomReply_Struct {
/* 00 */	int32 low;
/* 04 */	int32 high;
/* 08 */	int32 result;
/* 12 */	char name[64];
/* 76 */
};

struct LFG_Struct {
/*
Wrong size on OP_LFG. Got: 80, Expected: 68
   0: 00 00 00 00 01 00 00 00 - 00 00 00 00 64 00 00 00  | ............d...
  16: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  32: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  48: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  64: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
Wrong size on OP_LFG. Got: 80, Expected: 68
   0: 00 00 00 00 01 00 00 00 - 3F 00 00 00 41 00 00 00  | ........?...A...
  16: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  32: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  48: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  64: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
Wrong size on OP_LFG. Got: 80, Expected: 68
   0: 00 00 00 00 01 00 00 00 - 3F 00 00 00 41 00 00 00  | ........?...A...
  16: 46 72 75 62 20 66 72 75 - 62 20 66 72 75 62 00 00  | Frub frub frub..
  32: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  48: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
  64: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  | ................
*/
/*000*/	uint32 unknown000;
/*004*/	uint32 value; // 0x00 = off 0x01 = on
/*008*/	uint32 unknown008;
/*012*/	uint32 unknown012;
/*016*/	char	name[64];
};

/*
** LFG_Appearance_Struct
** Packet sent to clients to notify when someone in zone toggles LFG flag
** Size: 8 bytes
** Used in: OP_LFGAppearance
**
*/
struct LFG_Appearance_Struct
{
/*0000*/ uint32 spawn_id;		// ID of the client
/*0004*/ uint8 lfg;				// 1=LFG, 0=Not LFG
/*0005*/ char unknown0005[3];	//
/*0008*/
};


// EverQuest Time Information:
// 72 minutes per EQ Day
// 3 minutes per EQ Hour
// 6 seconds per EQ Tick (2 minutes EQ Time)
// 3 seconds per EQ Minute

struct TimeOfDay_Struct {
	int8	hour;
	int8	minute;
	int8	day;
	int8	month;
	int32	year;
};

// Darvik: shopkeeper structs
struct Merchant_Click_Struct {
/*000*/ int32	npcid;			// Merchant NPC's entity id
/*004*/ int32	playerid;
/*008*/ int32	command;		//1=open, 0=cancel/close
/*012*/ float	rate;			//cost multiplier, dosent work anymore 
};
/*
Unknowns:
0 is e7 from 01 to // MAYBE SLOT IN PURCHASE
1 is 03
2 is 00
3 is 00
4 is ??
5 is ??
6 is 00 from a0 to
7 is 00 from 3f to */
/*
0 is F6 to 01
1 is CE CE
4A 4A
00 00
00 E0
00 CB
00 90
00 3F
*/



struct Merchant_Sell_Struct {
/*000*/	int32	npcid;			// Merchant NPC's entity id
/*004*/	int32	playerid;		// Player's entity id
/*008*/	int32	itemslot;
		int32	unknown12;
/*016*/	int8	quantity;		// Already sold
/*017*/ int8	Unknown016[3];
/*020*/ int32	price;
};
struct Merchant_Purchase_Struct {
/*000*/	int32	npcid;			// Merchant NPC's entity id
/*004*/	int32	itemslot;		// Player's entity id
/*008*/	int32	quantity;
/*012*/	int32	price;
};
struct Merchant_DelItem_Struct{
/*000*/	int32	npcid;			// Merchant NPC's entity id
/*004*/	int32	playerid;		// Player's entity id
/*008*/	int32	itemslot;
};
struct Adventure_Purchase_Struct {
/*000*/	int32	some_flag;	//set to 1 generally...
/*000*/	int32	npcid;
/*004*/	int32	itemid;
/*008*/	int32	variable;
};

struct AdventurePoints_Update_Struct {
/*000*/	uint32				ldon_available_points;		// Total available points
/*004*/ uint8				unkown_apu004[20];
/*024*/	uint32				ldon_guk_points;		// Earned Deepest Guk points
/*028*/	uint32				ldon_mirugal_points;		// Earned Mirugal' Mebagerie points
/*032*/	uint32				ldon_mistmoore_points;		// Earned Mismoore Catacombs Points
/*036*/	uint32				ldon_rujarkian_points;		// Earned Rujarkian Hills points
/*040*/	uint32				ldon_takish_points;		// Earned Takish points
/*044*/	uint8				unknown_apu042[216];
};


struct AdventureFinish_Struct{
	uint32 win_lose;//Cofruben: 00 is a lose,01 is win.
	uint32 points;
};
//OP_AdventureRequest
struct AdventureRequest_Struct{
	int32 risk;//1 normal,2 hard.
	int32 entity_id;
};
struct AdventureRequestResponse_Struct{
	int32 unknown000;
	char text[2048];
	int32 timetoenter;
	int32 timeleft;
	int32 risk;
	float x;
	float y;
	float z;
	int32 showcompass;
	int32 unknown2080;
};



/*struct Item_Shop_Struct {
	uint16 merchantid;
	int8 itemtype;
	Item_Struct item;
	int8 iss_unknown001[6];
};*/

struct Illusion_Struct {
/*000*/	uint32	spawnid;
		char charname[64];		//fix for 7-14-04 patch
/**/	uint16	race;
/**/	char	unknown006[2];
/**/	uint8	gender;
/**/	uint8	texture;	
/**/	uint8	helmtexture;
/**/	uint8	unknown011;
/**/	uint32	face;
/**/	char	unknown020[176];
/**/
};

struct ZonePoint_Entry {
/*0000*/	int32	iterator;
/*0004*/	float	y;
/*0008*/	float	x;
/*0012*/	float	z;
/*0016*/	float	heading;
/*0020*/	int16	zoneid;
/*0022*/	int16	zoneinstance; // LDoN instance
};

struct ZonePoints {
/*0000*/	int32	count;
/*0004*/	struct	ZonePoint_Entry zpe[0]; // Always add one extra to the end after all zonepoints
};

struct SkillUpdate_Struct {
/*00*/	uint32 skillId;
/*04*/	uint32 value;
/*08*/
};

struct ZoneUnavail_Struct {
	//This actually varies, but...
	char zonename[16];
	sint16 unknown[4];
};

struct GroupGeneric_Struct {
	char name1[64];
	char name2[64];
};

struct GroupCancel_Struct {
	char	name1[64];
	char	name2[64];
	uint8	toggle;
};

struct GroupUpdate_Struct {	
/*0000*/	int32	action;
/*0004*/	char	yourname[64];
/*0068*/	char	membername[5][64];
/*0388*/	char	leadersname[64];
};

struct GroupUpdate2_Struct {	
/*0000*/	int32	action;
/*0004*/	char	yourname[64];
/*0068*/	char	membername[5][64];
/*0388*/	char	leadersname[64];
/*0452*/	GroupLeadershipAA_Struct leader_aas;
/*0580*/	int8	unknown[188];
/*0768*/
};
struct GroupJoin_Struct {	
/*0000*/	int32	action;
/*0004*/	char	yourname[64];
/*0068*/	char	membername[64];
/*0132*/	int8	unknown[84];
};

struct FaceChange_Struct {
/*000*/	int8	haircolor;
/*001*/	int8	beardcolor;
/*002*/	int8	eyecolor1; // the eyecolors always seem to be the same, maybe left and right eye?
/*003*/	int8	eyecolor2;
/*004*/	int8	hairstyle;
/*005*/	int8	beard;	// vesuvias
/*006*/	int8	face; 
//vesuvias:
//there are only 10 faces for barbs changing woad just
//increase the face value by ten so if there were 8 woad 
//designs then there would be 80 barb faces
};

/*
** Trade request from one client to another
** Used to initiate a trade
** Size: 8 bytes
** Used in: OP_TradeRequest
*/
struct TradeRequest_Struct {
/*00*/	uint32 to_mob_id;
/*04*/	uint32 from_mob_id;
/*08*/
};

struct TradeAccept_Struct {
/*00*/	uint32 from_mob_id;
/*04*/	uint32 unknown4;		//seems to be garbage
/*08*/
};

/*
** Cancel Trade struct
** Sent when a player cancels a trade
** Size: 8 bytes
** Used In: OP_CancelTrade
**
*/
struct CancelTrade_Struct {
/*00*/	int32 fromid;
/*04*/	int32 action;
/*08*/
};

struct PetitionUpdate_Struct {
	int32 petnumber;    // Petition Number
	int32 color;		// 0x00 = green, 0x01 = yellow, 0x02 = red
	int32 status;
	time_t senttime;    // 4 has to be 0x1F
	char accountid[32];
	char gmsenttoo[64];
	sint32 quetotal;
	char charname[64];
};

struct Petition_Struct {
	int32 petnumber;
	int32 urgency;
	char accountid[32];
	char lastgm[32];
	int32	zone;
	//char zone[32];
	char charname[64];
	int32 charlevel;
	int32 charclass;
	int32 charrace;
	int32 unknown;
	//time_t senttime; // Time?
	int32 checkouts;
	int32 unavail;
	//int8 unknown5[4];
	time_t senttime;
	int32 unknown2;
	char petitiontext[1024];
	char gmtext[1024];
};


struct Who_All_Struct { // 76 length total
/*000*/	char	whom[64];
/*064*/	int32	wrace;		// FF FF = no race

/*066*/	int32	wclass;		// FF FF = no class
/*068*/	int32	lvllow;		// FF FF = no numbers
/*070*/	int32	lvlhigh;	// FF FF = no numbers
/*072*/	int32	gmlookup;	// FF FF = not doing /who all gm
/*074*/	int32	unknown074;
/*076*/	int8	unknown076[64];
/*140*/
};

struct Stun_Struct { // 4 bytes total 
	int32 duration; // Duration of stun
};

struct AugmentItem_Struct { 
/*00*/	sint16	container_slot;
/*02*/	char	unknown02[2];
/*04*/	sint32	augment_slot;
/*08*/
};

// OP_Emote
struct Emote_Struct {
/*0000*/	int32 unknown01;
/*0004*/	char message[1024];
/*1028*/
};

// Inspect 
struct Inspect_Struct { 
	int16 TargetID; 
	int16 PlayerID; 
}; 
//OP_InspectAnswer
struct InspectResponse_Struct{//Cofruben:need to send two of this for the inspect response.
/*000*/	int32 TargetID;
/*004*/	int32 playerid;
/*008*/	char itemnames[21][64];
/*1352*/char unknown_zero[64];//fill with zero's.
/*1416*/int32 itemicons[21];
/*1500*/int32 unknown_zero2;
/*1504*/char text[288];
};

//OP_SetDataRate
struct SetDataRate_Struct {
	float newdatarate;
};

//OP_SetServerFilter
struct SetServerFilter_Struct {
	uint32 filters[31];		//see enum eqFilterType
};

//Op_SetServerFilterAck
struct SetServerFilterAck_Struct {
	int8 blank[8];
};
struct IncreaseStat_Struct{
	/*0000*/	int8	unknown0;
	/*0001*/	int8	str;
	/*0002*/	int8	sta;
	/*0003*/	int8	agi;
	/*0004*/	int8	dex;
	/*0005*/	int8	int_;
	/*0006*/	int8	wis;
	/*0007*/	int8	cha;
	/*0008*/	int8	fire;
	/*0009*/	int8	cold;
	/*0010*/	int8	magic;
	/*0011*/	int8	poison;
	/*0012*/	int8	disease;
	/*0013*/	char	unknown13[116];
	/*0129*/	int8	str2;
	/*0130*/	int8	sta2;
	/*0131*/	int8	agi2;
	/*0132*/	int8	dex2;
	/*0133*/	int8	int_2;
	/*0134*/	int8	wis2;
	/*0135*/	int8	cha2;
	/*0136*/	int8	fire2;
	/*0137*/	int8	cold2;
	/*0138*/	int8	magic2;
	/*0139*/	int8	poison2;
	/*0140*/	int8	disease2;
};

struct GMName_Struct {
	char oldname[64];
	char gmname[64];
	char newname[64];
	int8 badname;
	int8 unknown[3];
};

struct GMDelCorpse_Struct {
	char corpsename[64];
	char gmname[64];
	int8 unknown;
};

struct GMKick_Struct {
	char name[64];
	char gmname[64];
	int8 unknown;
};


struct GMKill_Struct {
	char name[64];
	char gmname[64];
	int8 unknown;
};


struct GMEmoteZone_Struct {
	char text[512];
};

// This is where the Text is sent to the client.
// Use ` as a newline character in the text.
// Variable length.
struct BookText_Struct {
	uint8 unknown0; //always 0xFF
	uint8 type;             //type: 0=scroll, 1=book.. prolly others.
	char booktext[1]; // Variable Length
};
// This is the request to read a book.
// This is just a "text file" on the server
// or in our case, the 'name' column in our books table.
struct BookRequest_Struct {
	uint8 unknown0; //always 0xFF
	uint8 type;             //type: 0=scroll, 1=book.. prolly others.
	char txtfile[20];
};

/*
** Object/Ground Spawn struct
** Used for Forges, Ovens, ground spawns, items dropped to ground, etc
** Size: 92 bytes
** OpCodes: OP_CreateObject
** Last Updated: Oct-17-2003
**
*/
struct Object_Struct {
/*00*/	uint32	linked_list_addr[2];// <Zaphod> They are, get this, prev and next, ala linked list
/*08*/	uint16	unknown008[2];		//
/*12*/	uint32	drop_id;			// Unique object id for zone
/*16*/	uint16	zone_id;			// Redudant, but: Zone the object appears in
/*18*/	uint16	zone_instance;		//
/*20*/	uint32	unknown020;			//
/*24*/	uint32	unknown024;			//
/*28*/	uint8	unknown028[12];		//
/*40*/	float	heading;			// heading
/*44*/	float	z;					// z coord
/*48*/	float	x;					// x coord
/*52*/	float	y;					// y coord
/*56*/	char	object_name[20];	// Name of object, usually something like IT63_ACTORDEF
//*72*/	float	unknown064;			// seems like coords, not always valid, all 0 on most world objects
//*76*/	float	unknown068;			// seems like coords, not always valid, all 0 on most world objects
//*80*/	float	unknown072;			// seems like coords, not always valid, all 0 on most world objects
/*84*/	uint32	unknown076;			//
/*88*/	uint32	unknown088;			// 
/*92*/	uint32	object_type;		// Type of object, not directly translated to OP_OpenObject
/*96*/	uint32	unknown084;			// 
/*100*/	uint32	spawn_id;			// Spawn Id of client interacting with object
/*104*/
};
//<Zaphod> 01 = generic drop, 02 = armor, 19 = weapon
//[13:40] <Zaphod> and 0xff seems to be indicative of the tradeskill/openable items that end up returning the old style item type in the OP_OpenObject

/*
** Click Object Struct
** Client clicking on zone object (forge, groundspawn, etc)
** Size: 8 bytes
** Last Updated: Oct-17-2003
**
*/
struct ClickObject_Struct {
/*00*/	uint32 drop_id;
/*04*/	uint32 player_id;
/*08*/
};

struct Shielding_Struct {
	uint32 target_id;
};

/*
** Click Object Acknowledgement Struct
** Response to client clicking on a World Container (ie, forge)
**
*/
struct ClickObjectAction_Struct {
/*00*/	uint32	player_id;	// Entity Id of player who clicked object
/*04*/	uint32	drop_id;	// Zone-specified unique object identifier
/*08*/	uint32	open;		// 1=opening, 0=closing
/*12*/	uint32	type;		// See object.h, "Object Types"
/*16*/	uint32	unknown16;	//
/*20*/	uint32	icon;		// Icon to display for tradeskill containers
/*24*/	uint32	unknown24;	//
/*28*/	char	object_name[64]; // Object name to display
/*92*/
};

/*
** This is different now, mostly unknown
**
*/
struct CloseContainer_Struct {
/*00*/	uint32	player_id;	// Entity Id of player who clicked object
/*04*/	uint32	drop_id;	// Zone-specified unique object identifier
/*08*/	uint32	open;		// 1=opening, 0=closing
/*12*/	uint32	unknown12[12];
};

/*
** Generic Door Struct
** Length: 52 Octets
** Used in: 
**    cDoorSpawnsStruct(f721)
**
*/
struct Door_Struct
{
/*0000*/ char    name[32];            // Filename of Door // Was 10char long before... added the 6 in the next unknown to it: Daeken M. BlackBlade
/*0032*/ float   yPos;               // y loc
/*0036*/ float   xPos;               // x loc
/*0040*/ float   zPos;               // z loc
/*0044*/ float	 heading;
/*0048*/ int32   incline;	// rotates the whole door
/*0052*/ int16   size;			// 100 is normal, smaller number = smaller model
/*0054*/ int8    unknown0038[6];
/*0060*/ uint8   doorId;             // door's id #
/*0061*/ uint8   opentype;
/*
 *  Open types:
 * 66 = PORT1414 (Qeynos)
 * 55 = BBBOARD (Qeynos)
 * 100 = QEYLAMP (Qeynos)
 * 56 = CHEST1 (Qeynos)
 * 5 = DOOR1 (Qeynos)
 */
/*0062*/ uint8  state_at_spawn;
/*0063*/ uint8  invert_state;	// if this is 1, the door is normally open
/*0064*/ int32  door_param;
/*0068*/ uint8  unknown0052[24]; // mostly 0s, the last 3 bytes are something tho
/*0092*/
};



struct DoorSpawns_Struct {
	struct Door_Struct doors[0];
};

/*
 OP Code: 	Op_ClickDoor
 Size:		16

 10/10/2003-Doodman	Filled in struct
*/
struct ClickDoor_Struct {
/*000*/	int32	doorid;
/*004*/	int8	picklockskill;
/*005*/	int8	unknown005[3];
/*008*/ int32	item_id;
/*012*/ int16	player_id;
/*014*/ int8	unknown014[2];
};

struct MoveDoor_Struct {
	int8	doorid;
	int8	action;
};


struct BecomeNPC_Struct {
	int32 id;
	sint32 maxlevel;
};

struct Underworld_Struct {
	float speed;
	float y;
	float x;
	float z;
};

struct Resurrect_Struct	{
	int32	unknown00;
	int16	zone_id;
	int16	instance_id;
	float	y;
	float	x;
	float	z;
	char	your_name[64];
	int32	unknown88;
	char	rezzer_name[64];
	int32	spellid;
	char	corpse_name[64];
	int32	action;
/* 228 */
};

struct SetRunMode_Struct {
	int8 mode;
	int8 unknown[3];
};

//EnvDamage is EnvDamage2 without a few bytes at the end.

struct EnvDamage2_Struct {
/*0000*/	int32 id;
/*0004*/	int16 unknown4;
/*0006*/	int32 damage;
/*0010*/	int8 unknown10[12];
/*0022*/	int8 dmgtype; //FA = Lava; FC = Falling
/*0023*/	int8 unknown2[4];
/*0027*/	int16 constant; //Always FFFF
/*0029*/	int16 unknown29;
};

//Bazaar Stuff =D

struct BazaarWindowStart_Struct {
	int8   action;
	int8   unknown1;
	int16  unknown2;
};


struct BazaarWelcome_Struct {
	BazaarWindowStart_Struct beginning;
	int32  traders;
	int32  items;
	int8   unknown1[8];
};

struct BazaarSearch_Struct {
	BazaarWindowStart_Struct beginning;
	int32	traderid;
	int32  class_;
	int32  race;
	int32  stat;
	int32  slot;
	int32  type;
	char   name[64];
	int32	minprice;
	int32	maxprice;
};
struct BazaarInspect_Struct{
	int32 item_id;
	int32 unknown;
	char name[64];
};
struct BazaarReturnDone_Struct{
	int32 type;
	int32 traderid;
	int32 unknown8;
	int32 unknown12;
	int32 unknown16;
};
struct BazaarSearchResults_Struct {
	BazaarWindowStart_Struct beginning;
	int32	numitems;
	int32	item_id;
	int32	seller_nr;
	int32	cost;
	int32	unknown20;
	char	name[64];
};

struct ServerSideFilters_Struct {
int8	clientattackfilters; // 0) No, 1) All (players) but self, 2) All (players) but group
int8	npcattackfilters;	 // 0) No, 1) Ignore NPC misses (all), 2) Ignore NPC Misses + Attacks (all but self), 3) Ignores NPC Misses + Attacks (all but group)
int8	clientcastfilters;	 // 0) No, 1) Ignore PC Casts (all), 2) Ignore PC Casts (not directed towards self)
int8	npccastfilters;		 // 0) No, 1) Ignore NPC Casts (all), 2) Ignore NPC Casts (not directed towards self)
};

/*
** Client requesting item statistics
** Size: 32 bytes
** Used In: OP_ItemLinkClick
** Last Updated: Sept-19-2003
**
*/
struct	ItemViewRequest_Struct {
/*000*/	uint32	item_id;
/*004*/	char	unknown004[20];	//im going to guess these are augments
/*024*/ uint32	link_hash;
/*028*/	char	unknown028[16];
};

/*
 *  Client to server packet
 */
struct PickPocket_Struct {
// Size 18
    uint32 to;
    uint32 from;
    uint16 myskill;
    uint8 type; // -1 you are being picked, 0 failed , 1 = plat, 2 = gold, 3 = silver, 4 = copper, 5 = item
    uint8 unknown1; // 0 for response, unknown for input
    uint32 coin;
    uint8 lastsix[2];
};
/*
 * Server to client packet
 */

struct sPickPocket_Struct {
	// Size 28 = coin/fail
	uint32 to;
	uint32 from;
	uint32 myskill;
	uint32 type;
	uint32 coin;
	char itemname[64];
};

struct LogServer_Struct {
// Op_Code OP_LOGSERVER
/*000*/	uint32	unknown000;
/*004*/	uint32	unknown004;
/*008*/	uint32	unknown008;
/*012*/	uint32	unknown012;	// htonl(1) on live
/*016*/	uint32	unknown016;	// htonl(1) on live
/*020*/	uint8	unknown020[12];
/*032*/	char	worldshortname[32];
/*064*/	uint8	unknown064[32];
/*096*/	char	unknown096[16];	// 'pacman' on live
/*112*/	char	unknown112[16];	// '64.37,148,36' on live
/*126*/	uint8	unknown128[48];
/*176*/	uint32	unknown176;	// htonl(0x00002695)
/*180*/	char	unknown180[80];	// 'eqdataexceptions@mail.station.sony.com' on live
/*260*/	uint8	unknown260;	// 0x01 on live
/*261*/	uint8	unknown261;	// 0x01 on live
/*262*/	uint8	unknown262[2];
/*264*/
};

struct ApproveWorld_Struct {
// Size 544
// Op_Code OP_ApproveWorld
    uint8 unknown544[544];
};

struct ClientError_Struct
{
/*00001*/	char	type;
/*00001*/	char	unknown0001[69];
/*00069*/	char	character_name[64];
/*00134*/	char	unknown134[192];
/*00133*/	char	message[31994];
/*32136*/
};

struct MobHealth
{
	/*0000*/	int8	hp; //health percent
	/*0001*/	int16	id;//mobs id
};

struct Track_Struct {
	int16 entityid;
	int16 y;
	int16 x;
	int16 z;
};

struct Tracking_Struct {
	Track_Struct Entrys[0];
};

/*
** ZoneServerInfo_Struct
** Zone server information
** Size: 130 bytes
** Used In: OP_ZoneServerInfo
**
*/
struct ZoneServerInfo_Struct
{
/*0000*/	char	ip[128];
/*0128*/	uint16	port;
};

struct WhoAllPlayer{
	int32	formatstring;
	int32	pidstring;
	char*	name;
	int32	rankstring;
	char*	guild;
	int32	unknown80[2];
	int32	zonestring;
	int32	zone;
	int32	class_;
	int32	level;
	int32	race;
	char*	account;
	int32	unknown100;
};

struct WhoAllReturnStruct {
	int32	id;
	int32	playerineqstring;
	char	line[27];
	int8	unknown35; //0A
	int32	unknown36;//0s
	int32	playersinzonestring;
	int32	unknown44[2]; //0s
	int32	unknown52;//1
	int32	unknown56;//1
	int32	playercount;//1
	struct WhoAllPlayer player[0];
};

struct Trader_Struct {
	int32	code;
	int32	itemid[160];
	int32	unknown;
	int32	itemcost[80];
};

struct ClickTrader_Struct {
	int32	code;
	int32	unknown[161];//damn soe this is totally pointless :/ but at least your finally using memset! Good job :) -LE
	int32	itemcost[80];
};

struct GetItems_Struct{
	int32	items[80];
};

struct BecomeTrader_Struct{
	int32 id;
	int32 code;
};

struct Trader_ShowItems_Struct{
	int32 code;
	int32 traderid;
	int32 unknown08[3];
};

struct TraderBuy_Struct{
	int32 unknown0;
	int32 traderid;
	int32 itemid;
	int32 unknown8;
	int32 price;
	int32 quantity;
	int32 slot_num;
	char  itemname[60];
};

struct TraderItemUpdate_Struct{
	int32 unknown0;
	int32 traderid;
	int8  fromslot;
	int8  toslot; //7?
	int16 charges;
};

struct MoneyUpdate_Struct{
	sint32 platinum;
	sint32 gold;
	sint32 silver;
	sint32 copper;
};

struct TraderDelItem_Struct{
	int32 slotid;
	int32 quantity;
	int32 unknown;
};

struct TraderClick_Struct{
	int32 traderid;
	int32 unknown4[2];
	int32 approval;
};

struct FormattedMessage_Struct{
	int32	unknown0;
	int32	string_id;
	int32	type;
	char	message[0];
};
struct SimpleMessage_Struct{
	int32	string_id;
	int32	color;
	int32	unknown8;
};

struct GuildMemberEntry_Struct {
	char	name[1];					//variable length
	int32	level;						//network byte order
	int32	banker;						//1=yes, 0=no, network byte order
	int32	class_;						//network byte order
	int32	rank;						//network byte order
	int32	time_last_on;				//network byte order
	int32	tribute_enable;				//network byte order
	int32	total_tribute;				//total guild tribute donated, network byte order
	int32	last_tribute;				//unix timestamp
	int32	unknown_one;		//unknown, set to one. (network byte order)
	char	public_note[1];				//variable length.
	int16	zoneinstance;				//network byte order
	int16	zone_id;					//network byte order
/* 42 + strings */
};

struct GuildMembers_Struct {	//just for display purposes, this is not actually used in the message encoding.
	char	player_name[1];		//variable length.
	int32	count;				//network byte order
	GuildMemberEntry_Struct member[0];
};

struct GuildMOTD_Struct{
/*0000*/	int32	unknown0;
/*0004*/	char	name[64];
/*0068*/	char	setby_name[64];
/*0132*/	int32	unknown132;
/*0136*/	char	motd[512];
};

struct GuildURL_Struct{
/*0000*/	int32	unknown0;	//index? seen server send 0 w/ the Guild URL, followed by 1 with nothing.
/*0004*/	int32	unknown4;
/*0008*/	int32	unknown8;	//seen 7
/*0068*/	char	setby_name[64];
/*0132*/	int32	unknown132;	//seen 0x167
/*0136*/	char	url[4080];
};

struct GuildMemberUpdate_Struct {
/*00*/	uint32 guild_id;
/*04*/	char	member_name[64];
/*68*/	uint16	zone_id;
/*70*/	uint16	instance_id;
/*72*/	uint32	some_timestamp;
};

struct GuildMemberLevelUpdate_Struct {
/*00*/	uint32 guild_id;
/*04*/	char	member_name[64];
/*68*/	uint32	level;	//not sure
};



struct GuildUpdate_PublicNote{
	int32	unknown0;
	char	name[64];
	char	target[64];
	char	note[100]; //we are cutting this off at 100, actually around 252
};
struct GuildDemoteStruct{
	char	name[64];
	char	target[64];
};
struct GuildRemoveStruct{
	char	target[64];
	char	name[64];
	int32	unknown128;
	int32	leaderstatus; //?
};
struct GuildMakeLeader{
	char	name[64];
	char	target[64];
};



struct BugStruct{
/*0000*/	char	chartype[64];
/*0064*/	char	name[96];
/*0160*/	char	ui[128];
/*0288*/	float	x;
/*0292*/	float	y;
/*0296*/	float	z;
/*0300*/	float	heading;
/*0304*/	int32	unknown304;
/*0308*/	int32	type;
/*0312*/	char	unknown312[2144];
/*2456*/	char	bug[1024];
/*3480*/	char	placeholder[2];
/*3482*/	char	system_info[4098];
};
struct Make_Pet_Struct { //Simple struct for getting pet info
	int8 level;
	int8 class_;
	int16 race;
	int8 texture;
	int8 pettype;
	float size;
	int8 type;
	int32 min_dmg;
	int32 max_dmg;
};
struct Ground_Spawn{
	float max_x;
	float max_y;
	float min_x;
	float min_y;
	float max_z;
	float heading;
	char name[16];
	int32 item;
	int32 max_allowed;
	int32 respawntimer;
};
struct Ground_Spawns {
	struct Ground_Spawn spawn[50]; //Assigned max number to allow
};
struct PetitionBug_Struct{
	int32	petition_number;
	int32	unknown4;
	char	accountname[64];
	int32	zoneid;
	char	name[64];
	int32	level;
	int32	class_;
	int32	race;
	int32	unknown152[3];
	int32	time;
	int32	unknown168;
	char	text[1028];
};

struct DyeStruct
{
	union
	{
		struct
		{
			struct Color_Struct head;
			struct Color_Struct chest;
			struct Color_Struct arms;
			struct Color_Struct wrists;
			struct Color_Struct hands;
			struct Color_Struct legs;
			struct Color_Struct feet;
			struct Color_Struct primary;	// you can't actually dye this
			struct Color_Struct secondary;	// or this
		}
		dyes;
		struct Color_Struct dye[9];
	};
};

struct ApproveZone_Struct {
	char	name[64];
	int32	zoneid;
	int32	approve;
};
struct ZoneInSendName_Struct {
	int32	unknown0;
	char	name[64];
	char	name2[64];
	int32	unknown132;
};
struct ZoneInSendName_Struct2 {
	int32	unknown0;
	char	name[64];
	int32	unknown68[145];
};

static const uint32 MAX_TRIBUTE_TIERS = 10;

struct StartTribute_Struct {
   int32	client_id;
   int32	tribute_master_id;
   int32	response;
};

struct TributeLevel_Struct {
   uint32	level;	//backwards byte order!
   int32	tribute_item_id;	//backwards byte order!
   int32	cost;	//backwards byte order!
};

struct TributeAbility_Struct {
	int32	tribute_id;	//backwards byte order!
	int32	tier_count;	//backwards byte order!
	TributeLevel_Struct tiers[MAX_TRIBUTE_TIERS];
	char	name[0];
};

struct GuildTributeAbility_Struct {
	uint32	guild_id;
	TributeAbility_Struct ability;
};

struct SelectTributeReq_Struct {
   int32	client_id;	//? maybe action ID?
   uint32	tribute_id;
   int32	unknown8;	//seen E3 00 00 00
};

struct SelectTributeReply_Struct {
   int32	client_id;	//echoed from request.
   uint32	tribute_id;
   char	desc[0];
};

struct TributeInfo_Struct {
	int32	active;		//0 == inactive, 1 == active
	uint32	tributes[MAX_PLAYER_TRIBUTES];	//-1 == NONE
	int32	tiers[MAX_PLAYER_TRIBUTES];		//all 00's
	int32	tribute_master_id;
};

struct TributeItem_Struct {
	int32   slot;
	int32   quantity;
	int32   tribute_master_id;
	sint32  tribute_points;
};

struct TributePoint_Struct {
	sint32   tribute_points;
	int32   unknown04;
	sint32   career_tribute_points;
	int32   unknown12;
};

struct TributeMoney_Struct {
	int32   platinum;
	int32   tribute_master_id;
	sint32   tribute_points;
};


struct Split_Struct
{
	uint32	platinum;
	uint32	gold;
	uint32	silver;
	uint32	copper;
};


/*
** New Combine Struct
** Client requesting to perform a tradeskill combine
** Size: 4 bytes
** Used In: OP_TradeSkillCombine
** Last Updated: Oct-15-2003
**
*/
struct NewCombine_Struct { 
/*00*/	sint16	container_slot;
/*02*/	char	unknown02[2];
/*04*/
};


//client requesting favorite recipies
struct TradeskillFavorites_Struct {
	uint32 object_type;
	uint32 some_id;
	uint32 favorite_recipes[500];
};

//search request
struct RecipesSearch_Struct {
	uint32 object_type;	//same as in favorites
	uint32 some_id;			//same as in favorites
	uint32 mintrivial;
	uint32 maxtrivial;
	char query[56];
	uint32 unknown4;	//is set to 00 03 00 00
	uint32 unknown5; //is set to 4C DD 12 00
/*80*/
};

//one sent for each item, from server in reply to favorites or search
struct RecipeReply_Struct {
	uint32 object_type;
	uint32 some_id;	 //same as in favorites
	uint32 component_count;
	uint32 recipe_id;
	uint32 trivial;
	char recipe_name[64];
/*84*/
};

//received and sent back as an ACK with different reply_code
struct RecipeAutoCombine_Struct {
	uint32 object_type;
	uint32 some_id;
	uint32 unknown1;		//echoed in reply
	uint32 recipe_id;
	uint32 reply_code;		// 93 64 e1 00 (junk) in request
								// 00 00 00 00 in successful reply
								// f5 ff ff ff in 'you dont have all the stuff' reply
};

struct LevelAppearance_Struct { //Sends a little graphic on level up
	int32	spawn_id;
	int32	parm1;
	int32	value1a;
	int32	value1b;
	int32	parm2;
	int32	value2a;
	int32	value2b;
	int32	parm3;
	int32	value3a;
	int32	value3b;
	int32	parm4;
	int32	value4a;
	int32	value4b;
	int32	parm5;
	int32	value5a;
	int32	value5b;
/*64*/
};
struct MerchantList{
	uint32	id;
	uint32	slot;
	uint32	item;
};
struct TempMerchantList{
	uint32	npcid;
	uint32	slot;
	uint32	item;
	uint32	charges; //charges/quantity
	uint32	origslot;
};


struct FindPerson_Point {
	float y;
	float x;
	float z;
};

struct FindPersonRequest_Struct {
	uint32	npc_id;
	FindPerson_Point client_pos;
};

//variable length packet of points
struct FindPersonResult_Struct {
	FindPerson_Point dest;
	FindPerson_Point path[0];	//last element must be the same as dest
};

struct MobRename_Struct {
/*000*/	char	old_name[64];
/*064*/	char	old_name_again[64];	//not sure what the difference is
/*128*/	char	new_name[64];
/*192*/	uint32	unknown192;		//set to 0
/*196*/	uint32	unknown196;		//set to 1
/*200*/
};

struct PlayMP3_Struct {
	char filename[128];
};

//this is for custom title display in the skill window
struct TitleEntry_Struct {
	uint32	skill_id;
	uint32	skill_value;
	char	title[1];
};

struct Titles_Struct {
	uint32	title_count;
	TitleEntry_Struct titles[0];
};

//this is for title selection by the client
struct TitleListEntry_Struct {
	uint32	unknown0;	//title ID
	char prefix[1];		//variable length, null terminated
	char postfix[1];		//variable length, null terminated
};

struct TitleList_Struct {
	uint32 title_count;
	TitleListEntry_Struct titles[0];	//list of title structs
	//uint32 unknown_ending; seen 0x7265, 0
};

struct SetTitle_Struct {
	uint32	is_suffix;	//guessed: 0 = prefix, 1 = suffix
	uint32	title_id;
};

struct SetTitleReply_Struct {
	uint32	is_suffix;	//guessed: 0 = prefix, 1 = suffix
	char	title[32];
	uint32	entity_id;
};

struct TaskDescription_Struct {
/*000*/	uint32	activity_count;		//not right.
/*004*/	uint32	taskid;
/*008*/	uint8	unk;
/*009*/	uint32	id3;
/*013*/	uint32	unknown13;
/*017*/	char	name[1];	//variable length, 0 terminated
/*018*/	uint32	unknown18;
/*022*/	uint32	unknown22;
/*026*/	char	desc[1];	//variable length, 0 terminated
/*027*/	uint32	reward_count;   //not sure
/*031*/	uint32	unknown31;
/*035*/	uint32	unknown35;
/*039*/	uint16	unknown39;
/*041*/	char	reward_link[1];	//variable length, 0 terminated
/*042*/	uint32	unknown43;  //maybe crystal count?
};

struct TaskMemberList_Struct {
/*00*/  uint32  gopher_id;
/*04*/  uint32  unknown04;
/*08*/  uint32  member_count;   //1 less than the number of members
/*12*/  char	list_pointer[0];
/*	list is of the form:
	char member_name[1]	//null terminated string
	uint8   task_leader	//boolean flag
*/
};


struct TaskActivity_Struct {
/*000*/	uint32	activity_count;		//not right
/*004*/	uint32	id3;
/*008*/	uint32	taskid;
/*012*/	uint32	activity_id;
/*016*/	uint32	unknown016;
/*020*/	uint32	activity_type;
/*024*/	uint32	unknown024;
/*028*/	uint32	unknown28;
/*032*/	char mob_name[1];	//variable length, 0 terminated
/*033*/	char item_name[1];	//variable length, 0 terminated
/*034*/	uint32	goal_count;
/*038*/	uint32	unknown38;	//0xFFFFFFFF
/*042*/	uint32	unknown42;	//0xFFFFFFFF
/*046*/	uint32	unknown46;  //saw 0x151,0x156
/*050*/	uint32	unknown50;  //saw 0x404,0
/*054*/	char activity_name[1];  //variable length, 0 terminated... commonly empty
/*055*/	uint32	done_count;
/*059*/	uint32	unknown59;	//=1 except on unknown and terminal activities?
/*063*/
};

struct TaskHistoryEntry_Struct {
	uint32	task_id;
	char	name[1];
	uint32	completed_time;
};
struct TaskHistory_Struct {
	uint32 completed_count;
	TaskHistoryEntry_Struct entries[0];
};

struct AcceptNewTask_Struct {
	uint32	task_id;		//set to 0 for 'decline'
	uint32	task_master_id;	//entity ID
};

//was all 0's from client, server replied with same op, all 0's
struct CancelTask_Struct {
	uint32 unknown0;
	uint32 unknown4;
};

struct AvaliableTask_Struct {
	uint32	task_index;		//no idea, seen 0x1
	uint32	task_master_id;	//entity ID
	uint32	task_id;
	uint32	unknown012;
	uint32	activity_count;	//not sure, seen 2
	char desc[1];	//variable length, 0 terminated
	uint32	reward_platinum;//not sure on these
	uint32	reward_gold;
	uint32	reward_silver;
	uint32	reward_copper;
	char some_name[1];	//variable length, 0 terminated
	uint8	unknown1;
	uint32	unknown2;	//0xFFFFFFFF
	uint32	unknown3;	//0xFFFFFFFF
	uint32	unknown4;	//seen 0x16
	uint8	unknown5;
};


struct BankerChange_Struct {
	uint32	platinum;
	uint32	gold;
	uint32	silver;
	uint32	copper;
	uint32	platinum_bank;
	uint32	gold_bank;
	uint32	silver_bank;
	uint32	copper_bank;
};

struct LeadershipExpUpdate_Struct {
/*00*/	uint32	group_leadership_exp;
/*04*/	uint32	group_leadership_points;
/*08*/	uint32	raid_leadership_exp;
/*12*/	uint32	raid_leadership_points;
};

struct UpdateLeadershipAA_Struct {
/*00*/	uint32	ability_id;
/*04*/	uint32	new_rank;
/*08*/	uint32	unknown08;
/*12*/
};

/** 
* Leadership AA update 
* Length: 32 Octets 
* OpCode: LeadExpUpdate 
*/ 
struct leadExpUpdateStruct { 
   /*0000*/ uint32 unknown0000;          // All zeroes? 
   /*0004*/ uint32 group_leadership_exp;         // Group leadership exp value 
   /*0008*/ uint32 group_leadership_points;   // Unspent group points 
   /*0012*/ uint32 unknown0012;          // Type? 
   /*0016*/ uint32 unknown0016;          // All zeroes? 
   /*0020*/ uint32 raid_leadership_exp;          // Raid leadership exp value 
   /*0024*/ uint32 raid_leadership_points;    // Unspent raid points 
   /*0028*/ uint32 unknown0028; 
}; 



struct RaidGeneral_Struct {
/*00*/	uint32		action;	//=10
/*04*/	char		player_name[64];	//should both be the player's name
uint32 unknown68;
/*04*/	char		leader_name[64];
/*132*/	uint32		parameter;
};

struct RaidAdd_Struct {
/*000*/	uint32		action;	//=0
/*004*/	char		player_name[64];	//should both be the player's name
/*068*/	char		leader_name[64];
/*132*/	uint8		_class;
/*133*/	uint8		level;
/*134*/	uint8		has_group;
/*135*/	uint8		unknown135;	//seems to be 0x42 or 0
};

struct RaidCreate_Struct {
/*00*/	uint32		action;	//=8
/*04*/	char		leader_name[64];
/*68*/	uint32		leader_id;
};

struct RaidMemberInfo_Struct {
/*00*/	uint8		group_number;
/*01*/	char		member_name[1];		//dyanmic length, null terminated '\0'
/*00*/	uint8		unknown00;
/*01*/	uint8		_class;
/*02*/	uint8		level;
/*03*/	uint8		is_raid_leader;
/*04*/	uint8		is_group_leader;
/*05*/	uint8		main_tank;		//not sure
/*06*/	uint8		unknown06[5];	//prolly more flags
};

struct RaidDetails_Struct {
/*000*/	uint32		action;	//=6,20
/*004*/	char		leader_name[64];
/*068*/	uint32		unknown68[4];
/*084*/	LeadershipAA_Struct abilities;	//ranks in backwards byte order		
/*128*/	uint8		unknown128[142];
/*354*/	uint32		leader_id;
};

struct RaidMembers_Struct {
/*000*/	RaidDetails_Struct		details;
/*358*/	uint32					member_count;		//including leader
/*362*/	RaidMemberInfo_Struct	members[1];
/*...*/	RaidMemberInfo_Struct	empty;	//seem to have an extra member with a 0 length name on the end
};

struct DynamicWall_Struct {
/*00*/  char	name[32];
/*32*/  float   y;
/*36*/  float   x;
/*40*/  float   z;
/*44*/  uint32  something;
/*48*/  uint32  unknown48;  //0
/*52*/  uint32  one_hundred;	//0x64
/*56*/  uint32  unknown56;  //0
/*60*/  uint32  something2;
/*64*/  sint32  unknown64;  //-1
/*68*/  uint32  unknown68;  //0
/*72*/  uint32  unknown72;  //0
/*76*/  uint32  unknown76;  //0x100
/*80*/
};

enum {	//bandolier actions
	BandolierCreate = 0,
	BandolierRemove = 1,
	BandolierSet = 2
};

struct BandolierCreate_Struct {
/*00*/	uint32	action;	//0 for create
/*04*/	uint8	number;
/*05*/	char	name[32];
/*37*/	uint16	unknown37;	//seen 0x93FD
/*39*/	uint8	unknown39;	//0
};

struct BandolierDelete_Struct {
/*00*/	uint32	action;
/*04*/	uint8	number;
/*05*/	int8	unknown05[35];
};

struct BandolierSet_Struct {
/*00*/	uint32	action;
/*04*/	uint8	number;
/*05*/	int8	unknown05[35];
};

struct Arrow_Struct {
/*000*/	uint32	type;		//unsure on name, seems to be 0x1, dosent matter
/*005*/	int8	unknown004[12];
/*016*/	float	src_y;
/*020*/	float	src_x;
/*024*/	float	src_z;
/*028*/	int8	unknown028[12];
/*040*/	float	velocity;		//4 is normal, 20 is quite fast
/*044*/	float	launch_angle;	//0-450ish, not sure the units, 140ish is straight
/*048*/	float	tilt;		//on the order of 125
/*052*/	int8	unknown052[8];
/*060*/	float	arc;
/*064*/	int8	unknown064[12];
/*076*/	uint32	source_id;
/*080*/ uint32	target_id;	//entity ID
/*084*/	uint32	item_id;	//1 to about 150ish
/*088*/	uint32	unknown088;	//seen 125, dosent seem to change anything..
/*092*/ uint32	unknown092;	//seen 16, dosent seem to change anything
/*096*/	uint8	unknown096[5];
/*101*/	char	model_name[16];
/*117*/	int8	unknown117[19];
};

//made a bunch of trivial structs for stuff for opcode finder to use
struct Consent_Struct {
	char name[1];	//always at least a null
};

struct AdventureMerchant_Struct {
	uint32	unknown_flag;		//seems to be 1
	uint32	entity_id;
};

struct Save_Struct {
	int8	unknown00[192];
};

struct GMToggle_Struct {
	int8 unknown0[64];
	uint32 toggle;
};

struct GroupInvite_Struct {
	char invitee_name[64];
	char inviter_name[64];
//	int8	unknown128[65];
};

struct BuffFadeMsg_Struct {
	uint32 color;
	char msg[1];
};

struct UseAA_Struct {
	int32 begin;
	int32 ability;
	int32 end;
};

struct AA_Ability {
/*00*/	int32 skill_id;
/*04*/	int32 base1;
/*08*/	int32 base2;
/*12*/	int32 slot;
};

struct SendAA_Struct {
/*0000*/	int32 id;
/*0004*/	int32 unknown004;		//set to 1.
/*0008*/	int32 hotkey_sid;
/*0012*/	int32 hotkey_sid2;
/*0016*/	int32 title_sid;
/*0020*/	int32 desc_sid;
/*0024*/	int32 class_type;
/*0028*/	int32 cost;
/*0032*/	int32 seq;
/*0036*/	int32 current_level; //1s, MQ2 calls this AARankRequired
/*0040*/	int32 prereq_skill;		//is < 0, abs() is category #
/*0044*/	int32 prereq_minpoints; //min points in the prereq
/*0048*/	int32 type;
/*0052*/	int32 spellid;
/*0056*/	int32 spell_type;
/*0060*/	int32 spell_refresh;
/*0064*/	int16 classes;
/*0066*/	int16 berserker; //seems to be 1 if its a berserker ability
/*0068*/	int32 max_level;
/*0072*/	int32 last_id;
/*0076*/	int32 next_id;
/*0080*/	int32 cost2;
/*0084*/	int32 unknown80[2]; //0s
/*0088*/	int32 total_abilities;
/*0092*/	AA_Ability abilities[0];
};

struct AA_List {
	SendAA_Struct* aa[0];
};

struct AA_Action {
/*00*/	int32	action;
/*04*/	int32	ability;
/*08*/	int32	unknown08;
/*12*/	int32	exp_value;
};

struct AA_Skills {		//this should be removed and changed to AA_Array
/*00*/	int32	aa_skill; 
/*04*/	int32	aa_value;
};

struct AAExpUpdate_Struct {
/*00*/	int32 unknown00;	//seems to be a value from AA_Action.ability
/*04*/	int32 aapoints_unspent;
/*08*/	int8 aaxp_percent;	//% of exp that goes to AAs
/*09*/	int8 unknown09[3];	//live dosent always zero these, so they arnt part of aaxp_percent
};


struct AltAdvStats_Struct {
/*000*/  uint32 experience;
/*004*/  uint16 unspent;
/*006*/  uint16	unknown006;
/*008*/  int8	percentage;
/*009*/  int8	unknown009[3];
};

struct PlayerAA_Struct {
	AA_Skills aa_list[MAX_PP_AA_ARRAY];
};

struct AATable_Struct {
	AA_Skills aa_list[MAX_PP_AA_ARRAY];
};

struct Weather_Struct {
	uint32	val1;	//generall 0x000000FF
	uint32	type;	//0x31=rain, 0x02=snow(i think), 0 = normal
	uint32	mode;	
};

struct ZoneInUnknown_Struct {
	uint32	val1;
	uint32	val2;
	uint32	val3;	
};

struct MobHealth_Struct {
	uint16 entity_id;
	uint8 hp;
};

struct AnnoyingZoneUnknown_Struct {
	uint32	entity_id;
	uint32	value;		//always 4
};




	};	//end namespace structs
};	//end namespace Anniversary




#endif /*Anniversary_STRUCTS_H_*/










