#ifndef Live_STRUCTS_H_
#define Live_STRUCTS_H_

namespace Live {
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

//New For SoF
struct WorldObjectsSent_Struct {
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
/*0000*/	uint8 level;				//
/*0000*/	uint8 hairstyle;			//
/*0002*/	uint8 gender;				//
/*0003*/	char name[1];				//variable length, edi+0
/*0000*/	uint8 beard;				//
/*0001*/	uint8 haircolor;			//
/*0000*/	uint8 face;					//
/*0000*/	CharSelectEquip	equip[9];
/*0000*/	uint32 primary;				//
/*0000*/	uint32 secondary;			//
/*0000*/	uint8 u15;					// 0xff
/*0000*/	uint32 deity;				//
/*0000*/	uint16 zone;				//
/*0000*/	uint16 instance;
/*0000*/	uint8 gohome;				//
/*0000*/	uint8 u19;					// 0xff
/*0000*/	uint32 race;				//
/*0000*/	uint8 tutorial;				//
/*0000*/	uint8 class_;				//
/*0000*/	uint8 eyecolor1;			//
/*0000*/	uint8 beardcolor;			//
/*0000*/	uint8 eyecolor2;			//
/*0000*/	uint32 drakkin_heritage;	// Drakkin Heritage
/*0000*/	uint32 drakkin_tattoo;		// Drakkin Tattoo
/*0000*/	uint32 drakkin_details;		// Drakkin Details (Facial Spikes)
/*0000*/	uint8 unknown;				// New field to Live

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
** Length: 897 Octets
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

struct Spawn_Struct_Bitfields
{

	unsigned   ispet:1;		// Could be 'is summoned pet' rather than just is pet.
	unsigned   afk:1;		// 0=no, 1=afk
	unsigned   sneak:1;
	unsigned   lfg:1;
	unsigned   padding5:1;
	unsigned   invis:1;		// 0 = visible, 1 = invis/sneaking
	unsigned   padding7:11;
	unsigned   gm:1;
	unsigned   anon:2;		// 0=normal, 1=anon, 2=roleplay
	unsigned   gender:2;		// Gender (0=male, 1=female, 2=monster)
	unsigned   linkdead:1;		// Toggles LD on or off after name
	unsigned   betabuffed:1;
	unsigned   showhelm:1;
	unsigned   padding26:1;
	unsigned   targetable:1;	// 1 = Targetable 0 = Not Targetable (is_npc?)
	unsigned   targetable_with_hotkey:1;	// is_npc?
	unsigned   showname:1;
	unsigned   statue:1;
	unsigned   trader:1;
	unsigned   buyer:1;
};

struct Spawn_Struct_Position
{
/*0000*/ signed   padding0000:12; // ***Placeholder
         signed   deltaX:13;      // change in x
         signed   padding0005:7;  // ***Placeholder
/*0000*/ signed   deltaHeading:10;// change in heading
         signed   deltaY:13;      // change in y
         signed   padding0006:9;  // ***Placeholder
/*0000*/ signed   y:19;           // y coord
         signed   animation:13;   // animation
/*0000*/ unsigned heading:12;     // heading
         signed   x:19;           // x coord
         signed   padding0014:1;  // ***Placeholder
/*0000*/ signed   z:19;           // z coord
         signed   deltaZ:13;      // change in z
};

struct Spawn_Struct
{
// Note this struct is not used as such, it is here for reference. As the struct is variable sized, the packets
// are constructed in Live.cpp
//
/*0000*/ char     name[1];	//name[64];
/*0000*/ //int8     nullterm1; // hack to null terminate name
/*0064*/ uint32 spawnId;
/*0068*/ uint8  level;
/*0069*/ float    unknown1;
/*0073*/ uint8  NPC;           // 0=player,1=npc,2=pc corpse,3=npc corpse
	 Spawn_Struct_Bitfields	Bitfields;
/*0000*/ uint8  otherData; // & 4 - has title, & 8 - has suffix, & 1 - it's a chest or untargetable
/*0000*/ float unknown3;	// seen -1
/*0000*/ float unknown4;
/*0000*/ float size;
/*0000*/ uint8  face;
/*0000*/ float    walkspeed;
/*0000*/ float    runspeed;
/*0000*/ uint32 race;
/*0000*/ uint8  showname; // for body types - was charProperties
/*0000*/ uint32 bodytype;
/*0000*/ //uint32 bodytype2;      // this is only present if charProperties==2
                                  // are there more than two possible properties?
/*0000*/ uint8  curHp;
/*0000*/ uint8  haircolor;
/*0000*/ uint8  beardcolor;
/*0000*/ uint8  eyecolor1;
/*0000*/ uint8  eyecolor2;
/*0000*/ uint8  hairstyle;
/*0000*/ uint8  beard;
/*0000*/ uint32 drakkin_heritage;
/*0000*/ uint32 drakkin_tattoo;
/*0000*/ uint32 drakkin_details;
/*0000*/ uint8  statue;				// was holding
/*0000*/ uint32 deity;
/*0000*/ uint32 guildID;
/*0000*/ uint32 guildrank;			// 0=member, 1=officer, 2=leader, -1=not guilded
/*0000*/ uint8  class_;
/*0000*/ uint8  pvp;					// 0 = normal name color, 2 = PVP name color
/*0000*/ uint8  StandState;			// stand state - 0x64 for normal animation
/*0000*/ uint8  light;
/*0000*/ uint8  flymode;
/*0000*/ uint8  equip_chest2;
/*0000*/ uint8  unknown9;
/*0000*/ uint8  unknown10;
/*0000*/ uint8  helm;
/*0000*/ char     lastName[1];
/*0000*/ //uint8     lastNameNull; //hack!
/*0000*/ uint32 aatitle;		// 0=none, 1=general, 2=archtype, 3=class was AARank
/*0000*/ uint8  unknown12;
/*0000*/ uint32 petOwnerId;
/*0000*/ uint8  unknown13;
/*0000*/ uint32 unknown14;		// Stance 64 = normal 4 = aggressive 40 = stun/mezzed
/*0000*/ uint32 unknown15;
/*0000*/ uint32 unknown16;
/*0000*/ uint32 unknown17;
/*0000*/ uint32 unknown18;
/*0000*/ uint32 unknown19;
	 Spawn_Struct_Position Position;
/*0000*/ union
         {
           struct
           {
               /*0000*/ Color_Struct color_helmet;    // Color of helmet item
               /*0000*/ Color_Struct color_chest;     // Color of chest item
               /*0000*/ Color_Struct color_arms;      // Color of arms item
               /*0000*/ Color_Struct color_bracers;   // Color of bracers item
               /*0000*/ Color_Struct color_hands;     // Color of hands item
               /*0000*/ Color_Struct color_legs;      // Color of legs item
               /*0000*/ Color_Struct color_feet;      // Color of feet item
               /*0000*/ Color_Struct color_primary;   // Color of primary item
               /*0000*/ Color_Struct color_secondary; // Color of secondary item
           } equipment_colors;
            /*0000*/ Color_Struct colors[9]; // Array elements correspond to struct equipment_colors above
         };

// skip these bytes if not a valid player race
/*0000*/ union
         {
           struct
           {
               /*0000*/ EquipStruct equip_helmet;     // Equiptment: Helmet visual
               /*0000*/ EquipStruct equip_chest;      // Equiptment: Chest visual
               /*0000*/ EquipStruct equip_arms;       // Equiptment: Arms visual
               /*0000*/ EquipStruct equip_bracers;    // Equiptment: Wrist visual
               /*0000*/ EquipStruct equip_hands;      // Equiptment: Hands visual
               /*0000*/ EquipStruct equip_legs;       // Equiptment: Legs visual
               /*0000*/ EquipStruct equip_feet;       // Equiptment: Boots visual
               /*0000*/ EquipStruct equip_primary;    // Equiptment: Main visual
               /*0000*/ EquipStruct equip_secondary;  // Equiptment: Off visual
           } equip;
           /*0000*/ EquipStruct equipment[9];
         };

/*0000*/ //char title[0];  // only read if(hasTitleOrSuffix & 4)
/*0000*/ //char suffix[0]; // only read if(hasTitleOrSuffix & 8)
	 char unknown20[8];
	 uint8 IsMercenary;	// If NPC == 1 and this == 1, then the NPC name is Orange.
/*0000*/ char unknown21[28]; 
};


/*
** Generic Spawn Struct
** Fields from old struct not yet found:
**	int8	traptype;	// 65 is disarmable trap, 66 and 67 are invis triggers/traps
**	uint8	is_pet;		// 0=no, 1=yes
**	uint8	afk;		// 0=no, 1=afk
**	uint8	is_npc;		// 0=no, 1=yes
**	uint8	max_hp;		// (name prolly wrong)takes on the value 100 for players, 100 or 110 for NPCs and 120 for PC corpses...
**	int8	guildrank;	// 0=normal, 1=officer, 2=leader
**	uint8	eyecolor2;	//not sure, may be face
**	int8	aaitle;		// 0=none, 1=general, 2=archtype, 3=class
*/

/*
** New Spawn
** Length: 176 Bytes
** OpCode: 4921
*/
struct NewSpawn_Struct
{
	struct Spawn_Struct spawn;	// Spawn Information
};


/*
** Client Zone Entry struct
** Length: 68 Octets
** OpCode: ZoneEntryCode (when direction == client)
*/
struct ClientZoneEntry_Struct {
/*0000*/ uint32 unknown0000;            // ***Placeholder
/*0004*/ char     char_name[64];               // Player firstname [32]
//*0036*/ uint8  unknown0036[28];        // ***Placeholder
//*0064*/ uint32 unknown0064;            // unknown
};


/*
** Server Zone Entry Struct
** Length: 452 Bytes
** OPCodes: OP_ServerZoneEntry
**
*/
struct ServerZoneEntry_Struct //Adjusted from SEQ Everquest.h Struct
{
	struct NewSpawn_Struct player;
};


//New Zone Struct - Size: 932
struct NewZone_Struct {
/*0000*/	char	char_name[64];			// Character Name
/*0064*/	char	zone_short_name[32];	// Zone Short Name
/*0096*/	char    unknown0096[96];
/*0192*/	char	zone_long_name[278];	// Zone Long Name
/*0470*/	uint8	ztype;					// Zone type (usually FF)
/*0471*/	uint8	fog_red[4];				// Zone fog (red)
/*0475*/	uint8	fog_green[4];				// Zone fog (green)
/*0479*/	uint8	fog_blue[4];				// Zone fog (blue)
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
/*0600*/	float	min_z;					// Guessed - NEW - Seen 0
/*0604*/	float	max_z;					// Guessed
/*0608*/	float	underworld;				// Underworld, min z (Not Sure?)
/*0612*/	float	minclip;				// Minimum View Distance
/*0616*/	float	maxclip;				// Maximum View DIstance
/*0620*/	int8	unknown620[84];		// ***Placeholder
/*0704*/	char	zone_short_name2[96];	//zone file name? excludes instance number which can be in previous version.
/*0800*/	sint32	unknown800;	//seen -1
/*0804*/	char	unknown804[40]; //
/*0844*/	sint32	unknown844;	//seen 600
/*0848*/	sint32	unknown848;
/*0852*/	uint16	zone_id;
/*0854*/	uint16	zone_instance;
/*0856*/	char	unknown856[20];
/*0876*/	uint32	SuspendBuffs;
/*0880*/	uint32	unknown880;	//seen 50
/*0884*/	uint32	unknown884;	//seen 10
/*0888*/	uint8	unknown888;	//seen 1
/*0889*/	uint8	unknown889;	//seen 0 (POK) or 1 (rujj)
/*0890*/	uint8	unknown890;	//seen 1
/*0891*/	uint8	unknown891;	//seen 0
/*0892*/	uint8	unknown892;	//seen 0
/*0893*/	uint8	unknown893;	//seen 0 - 00
/*0894*/	uint8	fall_damage;	// 0 = Fall Damage on, 1 = Fall Damage off
/*0895*/	uint8	unknown895;	//seen 0 - 00
/*0896*/	uint32	unknown896;	//seen 180
/*0900*/	uint32	unknown900;	//seen 180
/*0904*/	uint32	unknown904;	//seen 180
/*0908*/	uint32	unknown908;	//seen 2
/*0912*/	uint32	unknown912;	//seen 2
/*0916*/	float	FogDensity;	//Of about 10 or so zones tested, all but one have this set to 0.33 Blightfire had 0.16
/*0920*/	uint32	unknown920;	//seen 0
/*0924*/	uint32	unknown924;	//seen 0
/*0928*/	uint32	unknown928;	//seen 0
/*0932*/	uint8  unknown932[12];
};


/*
** Memorize Spell Struct
** Length: 16 Bytes
**
*/
struct MemorizeSpell_Struct {
int32 slot;     // Spot in the spell book/memorized slot
int32 spell_id; // Spell id (200 or c8 is minor healing, etc)
int32 scribing; // 1 if memorizing a spell, set to 0 if scribing to book, 2 if un-memming
//int32 unknown12;
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
	//char	message[0];
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
/*004*/	int16	caster_id;
/*006*/	int16	spell_id;
/*016*/	int32	cast_time;		// in miliseconds
};

struct CastSpell_Struct
{
	int32	slot;
	int32	spell_id;
	int32	inventoryslot;  // slot for clicky item, 0xFFFF = normal cast
	int32	target_id;
	int32    cs_unknown[5];
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

// Size 76 (was 24)
struct SpellBuff_Struct
{
/*000*/	int8 slotid;				// badly named... seems to be 2 for a real buff, 0 otherwise
/*001*/ int8 level;
/*002*/	int8 bard_modifier;
/*003*/	int8 effect;				// not real
/*004*/	uint32 unknown004;			// Seen 1 for no buff
/*008*/ int32 spellid;
/*012*/	int32 duration;
/*016*/ int16 dmg_shield_remaining;	//
/*018*/ int8 persistant_buff;		// prolly not real
/*019*/ int8 reserved;				// proll not real
/*020*/	int32 player_id;			// 'global' ID of the caster, for wearoff messages
/*024*/ uint8 unknown0028[52];
};

// Not functional yet, but this is what the packet looks like on Live
struct SpellBuffFade_Struct_Live {
/*000*/	uint32 entityid;	// Player id who cast the buff
/*004*/	int8 slot;
/*005*/	int8 level;
/*006*/	int8 effect;
/*007*/	int8 unknown7;
/*008*/	float unknown008;
/*012*/	uint32 spellid;
/*016*/	uint32 duration;
/*020*/	uint32 unknown016;
/*024*/ uint32 playerId;	// Global player ID?		
/*028*/	uint32 unknown020;
/*032*/ uint8 unknown0028[48];
/*080*/	uint32 slotid;
/*084*/	uint32 bufffade;
/*088*/
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
/*020*/	uint32 unknown020;		// Global player ID?
/*024*/ uint32 playerId;		// Player id who cast the buff
/*028*/	uint32 slotid;
/*032*/	uint32 bufffade;
/*036*/
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
	char zonename[64];
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
** OpCode: 0x009b
*/
struct CharCreate_Struct
{
/*0000*/	int32	class_;
/*0004*/	int32	haircolor;
/*0008*/	int32	beard;	
/*0012*/	int32	beardcolor;
/*0016*/	int32	gender;
/*0020*/	int32	race;
/*0024*/	int32	start_zone;
/*0028*/	int32	hairstyle;
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
/*0076*/	int32	tutorial;
/*0080*/	int32	drakkin_heritage;
/*0084*/	int32	drakkin_tattoo;
/*0088*/	int32	drakkin_details;
/*0092*/
};

/*
** Character Creation struct
** Length: 0 Bytes
** OpCode: 0x
*/
struct CharCreate_Struct_Temp //Size is now 0
{
};

/*
 *Used in PlayerProfile
 */
struct AA_Array
{
	int32 AA;
	int32 value;
	int32 unknown08;	// Looks like AA_Array is now 12 bytes in Live
};


static const uint32 MAX_PP_DISCIPLINES = 100;

struct Disciplines_Struct {
	uint32 values[MAX_PP_DISCIPLINES];
};

static const uint32 MAX_PLAYER_TRIBUTES = 5;
static const uint32 MAX_PLAYER_BANDOLIER = 20;
static const uint32 MAX_PLAYER_BANDOLIER_ITEMS = 4;
static const uint32 MAX_POTIONS_IN_BELT = 5;
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
static const uint32 MAX_PP_LANGUAGE		= 25; //
static const uint32 MAX_PP_SPELLBOOK	= 480; // Confirmed 60 pages on Live now
static const uint32 MAX_PP_MEMSPELL		= 10; //was 9 now 10 on Live
static const uint32 MAX_PP_SKILL		= 75;
static const uint32 MAX_PP_AA_ARRAY		= 300; //was 299
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

// Live May 5 2010 - Size 25312 + 1320
// Live May 12 2010 - Size 26632
struct PlayerProfile_Struct
{
/*00000*/ uint32  checksum;				//
//BEGIN SUB-STRUCT used for shrouding stuff...
/*00004*/ uint32  gender;				// Player Gender - 0 Male, 1 Female
/*00008*/ uint32  race;					// Player race
/*00012*/ uint32  class_;				// Player class
/*00016*/ uint8  unknown00016[40];		// #### uint32  unknown00016;   in Titanium ####uint8[40]
/*00056*/ uint8   level;				// Level of player
/*00057*/ uint8   level1;				// Level of player (again?)
/*00058*/ uint8   unknown00022[2];		// ***Placeholder
/*00060*/ BindStruct binds[5];			// Bind points (primary is first)
/*00160*/ uint32  deity;				// deity
/*00164*/ uint32  intoxication;			// Alcohol level (in ticks till sober?)
/*00168*/ uint32  spellSlotRefresh[MAX_PP_MEMSPELL]; // Refresh time (millis) - 4 Octets Each
/*00208*/ uint8   unknown00218[14];		// Seen 00 00 00 00 00 00 00 00 00 00 00 00 02 01
/*00222*/ uint32  abilitySlotRefresh;
/*00226*/ uint8   haircolor;			// Player hair color
/*00227*/ uint8   beardcolor;			// Player beard color
/*00228*/ uint8   eyecolor1;			// Player left eye color
/*00229*/ uint8   eyecolor2;			// Player right eye color
/*00230*/ uint8   hairstyle;			// Player hair style
/*00231*/ uint8   beard;				// Player beard type
/*00232*/ uint8	  unknown00236[4];		// was 14
/*00236*/ union
	 {
		struct
		{
		/*00236*/ EquipStruct equip_helmet; // Equiptment: Helmet visual
		/*00248*/ EquipStruct equip_chest; // Equiptment: Chest visual
		/*00260*/ EquipStruct equip_arms; // Equiptment: Arms visual
		/*00272*/ EquipStruct equip_bracers; // Equiptment: Wrist visual
		/*00284*/ EquipStruct equip_hands; // Equiptment: Hands visual
		/*00296*/ EquipStruct equip_legs; // Equiptment: Legs visual
		/*00308*/ EquipStruct equip_feet; // Equiptment: Boots visual
		/*00320*/ EquipStruct equip_primary; // Equiptment: Main visual
		/*00332*/ EquipStruct equip_secondary; // Equiptment: Off visual
		} equip;
		/*00236*/ EquipStruct equipment[9]; //Live Shows [108] for this part
	 };
/*00344*/ uint8 unknown00224[168];		// Live Shows [160]
/*00512*/ Color_Struct item_tint[9];	// RR GG BB 00
/*00548*/ AA_Array  aa_array[MAX_PP_AA_ARRAY];	// [3600] AAs 12 bytes each
/*04148*/ uint32  points;				// Unspent Practice points - RELOCATED???
/*04152*/ uint32  mana;					// Current mana
/*04156*/ uint32  cur_hp;				// Current HP without +HP equipment
/*04160*/ uint32  STR;					// Strength - 6e 00 00 00 - 110
/*04164*/ uint32  STA;					// Stamina - 73 00 00 00 - 115
/*04168*/ uint32  CHA;					// Charisma - 37 00 00 00 - 55
/*04172*/ uint32  DEX;					// Dexterity - 50 00 00 00 - 80
/*04176*/ uint32  INT;					// Intelligence - 3c 00 00 00 - 60
/*04180*/ uint32  AGI;					// Agility - 5f 00 00 00 - 95
/*04184*/ uint32  WIS;					// Wisdom - 46 00 00 00 - 70
/*04188*/ uint8   unknown04172[28];		// 
/*04216*/ uint8   face;					// Player face - Actually int32?
/*04217*/ uint8   unknown02264[147];		// was [175]
/*04364*/ int32   spell_book[MAX_PP_SPELLBOOK];	// List of the Spells in spellbook 720 = 90 pages [2880] was [1920]
/*06284*/ uint8   unknown06284[960];			// Spacer for the end of the book for now (pages 60 to 90)
/*07244*/ int32   mem_spells[MAX_PP_MEMSPELL]; // List of spells memorized
/*07284*/ uint8   unknown04396[28];		//#### uint8 unknown04396[32]; in Titanium ####[28]
/*07312*/ uint32  platinum;				// Platinum Pieces on player
/*07316*/ uint32  gold;					// Gold Pieces on player
/*07320*/ uint32  silver;				// Silver Pieces on player
/*07324*/ uint32  copper;				// Copper Pieces on player
/*07328*/ uint32  platinum_cursor;		// Platinum Pieces on cursor
/*07332*/ uint32  gold_cursor;			// Gold Pieces on cursor
/*07336*/ uint32  silver_cursor;		// Silver Pieces on cursor
/*07340*/ uint32  copper_cursor;		// Copper Pieces on cursor
/*07344*/ uint32  skills[MAX_PP_SKILL];	// [300] List of skills
/*07644*/ uint8   unknown04760[236];
/*07880*/ uint32  toxicity;				// Potion Toxicity (15=too toxic, each potion adds 3)
/*07884*/ uint32  thirst_level;			// Drink (ticks till next drink)
/*07888*/ uint32  hunger_level;			// Food (ticks till next eat)
/*07892*/ SpellBuff_Struct buffs[BUFF_COUNT];	// [1900] Buffs currently on the player (30 Max) - (Each Size 76)
/*09792*/ uint8   unknown08492[380];		// End of Buffs - was [360]
/*10172*/ Disciplines_Struct  disciplines;	// [400] Known disciplines
/*10572*/ uint8   unknown09252[400];		// Discs?
/*10972*/ uint32  recastTimers[MAX_RECAST_TYPES]; // Timers (UNIX Time of last use)
/*11052*/ uint8   unknown08124[160];		// Some type of Timers
/*11212*/ uint32  endurance;			// Current endurance
/*11216*/ uint8   unknown09896[20];		// ?
/*11236*/ uint32  aapoints_spent;		// Number of spent AA points
/*11240*/ uint32  aapoints;				// Unspent AA points
/*11244*/ uint8 unknown06160[4];
/*11248*/ Bandolier_Struct bandoliers[MAX_PLAYER_BANDOLIER]; // [6400] bandolier contents
/*17648*/ PotionBelt_Struct  potionbelt;	// [360] potion belt 72 extra octets by adding 1 more belt slot
/*18008*/ uint8 unknown12852[8];
/*18016*/ uint32 available_slots;
/*18020*/ uint8 unknown12864[80];		//
//END SUB-STRUCT used for shrouding. 
/*18100*/ char    name[64];				// Name of player
/*18164*/ char    last_name[32];		// Last name of player
/*18196*/ uint8   unknown19588[8];  //#### Not In Titanium #### new to SoF[12]
/*16884*/ int32   guild_id;            // guildid
/*16888*/ uint32  birthday;       // character birthday
/*16892*/ uint32  lastlogin;       // character last save time
/*16896*/ uint32  account_startdate;       // Date the Account was started - New Field for Live***
/*16900*/ uint32  timePlayedMin;      // time character played
/*16904*/ uint8   pvp;                // 1=pvp, 0=not pvp
/*16905*/ uint8   anon;               // 2=roleplay, 1=anon, 0=not anon
/*16906*/ uint8   gm;                 // 0=no, 1=yes (guessing!)
/*16907*/ int8    guildrank;        // 0=member, 1=officer, 2=guildleader -1=no guild
/*16908*/ uint32  guildbanker;
/*16912*/ uint8 unknown13054[4];  //was [8]
/*16916*/ uint32  exp;                // Current Experience
/*16920*/ uint8 unknown13072[8];
/*16928*/ uint32  timeentitledonaccount;
/*16932*/ uint8   languages[MAX_PP_LANGUAGE]; // List of languages
/*16957*/ uint8   unknown13109[7];    //#### uint8   unknown13109[4]; in Titanium ####[7]
/*16964*/ float     y;                  // Players y position (NOT positive about this switch)
/*16968*/ float     x;                  // Players x position
/*16972*/ float     z;                  // Players z position
/*16976*/ float     heading;            // Players heading
/*16980*/ uint8   unknown13132[4];    // ***Placeholder
/*16984*/ uint32  platinum_bank;      // Platinum Pieces in Bank
/*16988*/ uint32  gold_bank;          // Gold Pieces in Bank
/*16992*/ uint32  silver_bank;        // Silver Pieces in Bank
/*16996*/ uint32  copper_bank;        // Copper Pieces in Bank
/*17000*/ uint32  platinum_shared;    // Shared platinum pieces
/*17004*/ uint8 unknown13156[1036];    // was [716]
/*18040*/ uint32  expansions;         // Bitmask for expansions ff 7f 00 00 - SoD
/*18044*/ uint8 unknown13244[12];
/*18056*/ uint32  autosplit;          // 0 = off, 1 = on
/*18060*/ uint8 unknown13260[16];
/*18076*/ uint16  zone_id;             // see zones.h
/*18074*/ uint16  zoneInstance;       // Instance id
/*18080*/ char      groupMembers[MAX_GROUP_MEMBERS][64];// 384 all the members in group, including self
/*18464*/ char      groupLeader[64];    // Leader of the group ?
/*18528*/ uint8 unknown13728[604];  // was [348]
/*19132*/ uint32  entityid;
/*19136*/ uint32  leadAAActive;       // 0 = leader AA off, 1 = leader AA on
/*19140*/ uint8 unknown14392[4];
/*19144*/ sint32  ldon_points_guk;    // Earned GUK points
/*19148*/ sint32  ldon_points_mir;    // Earned MIR points
/*19152*/ sint32  ldon_points_mmc;    // Earned MMC points
/*19156*/ sint32  ldon_points_ruj;    // Earned RUJ points
/*19160*/ sint32  ldon_points_tak;    // Earned TAK points
/*19164*/ sint32  ldon_points_available;  // Available LDON points
/*19168*/ uint8 unknown14420[68]; // was [136]
/*19236*/ float  tribute_time_remaining;        // Time remaining on tribute (millisecs)
/*19240*/ uint32  career_tribute_points;      // Total favor points for this char
/*19244*/ uint32  unknown7208;        // *** Placeholder
/*19248*/ uint32  tribute_points;     // Current tribute points
/*19252*/ uint32  unknown7216;        // *** Placeholder
/*19256*/ uint32  tribute_active;      // 0 = off, 1=on
/*19260*/ Tribute_Struct tributes[MAX_PLAYER_TRIBUTES]; // [40] Current tribute loadout
/*19300*/ uint8 unknown14616[4];
/*19304*/ double group_leadership_exp;     // Current group lead exp points
/*19312*/ double raid_leadership_exp;      // Current raid lead AA exp points
/*19320*/ uint32  group_leadership_points; // Unspent group lead AA points
/*19324*/ uint32  raid_leadership_points;  // Unspent raid lead AA points
/*19328*/ LeadershipAA_Struct leader_abilities; // [128]Leader AA ranks 19332
/*19456*/ uint8 unknown14772[132];		// was [128]
/*19588*/ uint32  air_remaining;       // Air supply (seconds)
/*19592*/ uint32  PVPKills;
/*19596*/ uint32  PVPDeaths;
/*19600*/ uint32  PVPCurrentPoints;
/*19604*/ uint32  PVPCareerPoints;
/*19608*/ uint32  PVPBestKillStreak;
/*19612*/ uint32  PVPWorstDeathStreak;
/*19616*/ uint32  PVPCurrentKillStreak;
/*19620*/ PVPStatsEntry_Struct PVPLastKill;		// size 88
/*19708*/ PVPStatsEntry_Struct PVPLastDeath;	// size 88
/*19796*/ uint32  PVPNumberOfKillsInLast24Hours;
/*19800*/ PVPStatsEntry_Struct PVPRecentKills[50];	// size 4400 - 88 each
/*24200*/ uint32 expAA;               // Exp earned in current AA point
/*24204*/ uint8 unknown19516[40];
/*24244*/ uint32 currentRadCrystals;  // Current count of radiant crystals
/*24248*/ uint32 careerRadCrystals;   // Total count of radiant crystals ever
/*24252*/ uint32 currentEbonCrystals; // Current count of ebon crystals
/*24256*/ uint32 careerEbonCrystals;  // Total count of ebon crystals ever
/*24260*/ uint8  groupAutoconsent;    // 0=off, 1=on
/*24261*/ uint8  raidAutoconsent;     // 0=off, 1=on
/*24262*/ uint8  guildAutoconsent;    // 0=off, 1=on
/*24263*/ uint8  unknown19575;     // ***Placeholder (6/29/2005)
/*24264*/ uint32 level3;		// SoF looks at the level here to determine how many leadership AA you can bank.
/*24268*/ uint32 showhelm;            // 0=no, 1=yes
/*24272*/ uint32 RestTimer;
/*24276*/ uint8   unknown19584[1036]; // ***Placeholder (2/13/2007) was[1028]or[940]or[1380] - END of Struct 
/*25312*/
};

/**
 * Shroud spawn. For others shrouding, this has their spawnId and
 * spawnStruct.
 *
 * Length: 586
 * OpCode: OP_Shroud
 */
//struct spawnShroudOther_Struct
//{
//*0000*/ uint32 spawnId;          // Spawn Id of the shrouded player
//*0004*/ spawn_Struct spawn;         // Updated spawn struct for the player
//*0586*/
//};

/**
 * Shroud yourself. For yourself shrouding, this has your spawnId, spawnStruct,
 * bits of your charProfileStruct (no checksum, then charProfile up till
 * but not including name), and an itemPlayerPacket for only items on the player
 * and not the bank.
 *
 * Length: Variable
 * OpCode: OP_Shroud
 */
//struct spawnShroudSelf_Struct
//{
//*00000*/ uint32 spawnId;            // Spawn Id of you
//*00004*/ spawn_Struct spawn;           // Updated spawnStruct for you
//*00586*/ PlayerProfile_Struct profile; // Character profile for shrouded char
//*13522*/ uint8 items;  // Items on the player
/*xxxxx*/
//};



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
/*11*/	char	sayer[1];				// Who is the source of the info - Was 1
/*12*/	uint8	unknown12[12];
/*24*/	char	message[128];				// What is being said? - was 128
};

/*
** When somebody changes what they're wearing
**      or give a pet a weapon (model changes)
** Length: 19 Bytes
*/
struct WearChange_Struct{
/*000*/ int16 spawn_id;
/*002*/ int32 material;
/*006*/ uint32 unknown06;
/*010*/ uint32 c;	// 1 for Drakkin Elite Material
/*014*/ Color_Struct color;
/*018*/ int8 wear_slot_id;
/*019*/
};

/*
** Type:   Bind Wound Structure
** Length: 8 Bytes
*/
//Fixed for 7-14-04 patch
struct BindWound_Struct
{
/*000*/	int16	to;			// TargetID
/*002*/	int16	unknown2;		// ***Placeholder
/*004*/	int16	type;
/*006*/	int16	unknown6;
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
/*00*/	int16 target;			// id of target
/*02*/	int16 source;			// id of caster
/*04*/	uint16 level;			// level of caster - Seen 0
/*06*/  uint32 unknown06;
/*10*/	uint16 instrument_focus;
/*12*/	uint16 unknown12;		// seems to always be set to something and it doesn't change between casts except in special cases like changing instrument mods
/*14*/  uint32 unknown14;       // seen 0
/*18*/  float knockback_angle;  //seems to go from 0-512 then it rolls over again
/*22*/  uint32 unknown22;
/*26*/  uint8 type;
/*27*/  uint32 damage;
/*31*/  uint16 unknown31;
/*33*/	int16 spell;			// spell id being cast
/*35*/	int8 level2;			// level of caster again? Or maybe the castee
/*36*/	int8 effect_flag;		// if this is 4, the effect is valid: or if two are sent at the same time?
};



// Starting with 2/21/2006, OP_Actions seem to come in pairs, duplicating
// themselves, with the second one with slightly more information. Maybe this
// has to do with buff blocking??
struct ActionAlt_Struct
{
/*00*/	int16 target;			// id of target
/*02*/	int16 source;			// id of caster
/*04*/	uint16 level;			// level of caster - Seen 0
/*06*/  uint32 unknown06;
/*10*/	uint16 instrument_focus;
/*12*/	uint16 unknown12;		// seems to always be set to something and it doesn't change between casts except in special cases like changing instrument mods
/*14*/  uint32 unknown14;       // seen 0
/*18*/  float knockback_angle;  //seems to go from 0-512 then it rolls over again
/*22*/  uint32 unknown22;
/*26*/  uint8 type;
/*27*/  uint32 damage;
/*31*/  uint16 unknown31;
/*33*/	int16 spell;			// spell id being cast
/*35*/	int8 level2;			// level of caster again? Or maybe the castee
/*36*/	int8 effect_flag;		// if this is 4, the effect is valid: or if two are sent at the same time?
/*37*/	int32 unknown37;		// New field to Underfoot - Seen 14
/*41*/	int8 unknown41;			// New field to Underfoot - Seen 0
/*42*/	int8 unknown42;			// New field to Underfoot - Seen 0
/*43*/	int8 unknown43;			// New field to Underfoot - Seen 0
/*44*/	int32 unknown44;		// New field to Underfoot - Seen 23
/*48*/	int32 unknown48;		// New field to Underfoot - Seen -1
/*52*/	int32 unknown52;		// New field to Underfoot - Seen -1
/*56*/	int32 unknown56;		// New field to Underfoot - Seen 0
/*60*/	int32 unknown60;		// New field to Underfoot - Seen 0
/*64*/
};

// solar: this is what prints the You have been struck. and the regular
// melee messages like You try to pierce, etc.  It's basically the melee
// and spell damage message
struct CombatDamage_Struct
{
/* 00 */	int16	target;
/* 02 */	int16	source;
/* 04 */	int8	type;			//slashing, etc.  231 (0xE7) for spells
/* 05 */	int16	spellid;
/* 07 */	sint32	damage;
/* 11 */	float	unknown11;		// cd cc cc 3d
/* 15 */	float	sequence;		// see above notes in Action_Struct
/* 19 */	uint8	unknown19[9];	// was [9]
/* 28 */
};
    

/*
** Consider Struct
** Length: 20 Bytes
*/
struct Consider_Struct{
/*000*/ uint32	playerid;               // PlayerID
/*004*/ uint32	targetid;               // TargetID
/*008*/ int32	faction;                // Faction
/*012*/ int32	level;					// Level
/*016*/ int8	pvpcon;					// Pvp con flag 0/1
/*017*/ int8	unknown017[3];			//
/*020*/ 
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

struct ZonePlayerToBind_Struct {
/*000*/	uint32	bind_zone_id;
/*004*/	float	x;
/*008*/	float	y;
/*012*/	float	z;
/*016*/	float	heading;
/*020*/	char	zone_name[1];  // Or "Bind Location"
/*000*/	uint8	unknown021;	// Seen 1 - Maybe 0 would be to force a rezone and 1 is just respawn
/*000*/	uint32	unknown022;	// Seen 32 or 59
/*000*/	uint32	unknown023;	// Seen 0
/*000*/	uint32	unknown024;	// Seen 21 or 43
};

struct ZonePlayerToBindHeader_Struct
{
	/*000*/	uint32	bind_zone_id;
	/*004*/	float	x;
	/*008*/	float	y;
	/*012*/	float	z;
	/*016*/	float	heading;
	/*020*/	char	zone_name[1];  // Or "Bind Location"
};

struct ZonePlayerToBindFooter_Struct
{
	/*000*/	uint8	unknown021;	// Seen 1 - Maybe 0 would be to force a rezone and 1 is just respawn
	/*000*/	uint32	unknown022;	// Seen 32 or 59
	/*000*/	uint32	unknown023;	// Seen 0
	/*000*/	uint32	unknown024;	// Seen 21 or 43
};

typedef struct {
/*000*/	uint32	bind_number;		// Number of this bind in the iteration
/*004*/	uint32	bind_zone_id;		// ID of the zone for this bind point or resurect point
/*008*/	float	x;					// X loc for this bind point
/*012*/	float	y;					// Y loc for this bind point
/*016*/	float	z;					// Z loc for this bind point
/*020*/	float	heading;			// Heading for this bind point
/*024*/	char	bind_zone_name[1];	// Or "Bind Location" or "Resurrect"
/*000*/	uint8	validity;		// 0 = valid choice, 1 = not a valid choice at this time (resurrection)
} RespawnOptions_Struct;

struct RespawnWindow_Struct {
/*000*/	uint32	unknown000;		// Seen 0
/*004*/	uint32	time_remaining;	// Total time before respawn in milliseconds
/*008*/	uint32	unknown008;		// Seen 0
/*012*/	uint32	total_binds;	// Total Bind Point Options? - Seen 2
/*016*/ RespawnOptions_Struct bind_points;
// First bind point is "Bind Location" and the last one is "Ressurect"
};


/*
** Spawn position update - Size: 22
**	Struct sent from server->client to update position of
**	another spawn's position update in zone (whether NPC or PC)
**
*/
struct PlayerPositionUpdateServer_Struct
{
/*0000*/ uint16		spawn_id;
/*0002*/ signed		padding0000:12; // ***Placeholder
         signed		delta_x:13;      // change in x
         signed		padding0005:7;  // ***Placeholder
/*0006*/ signed		delta_heading:10;// change in heading
         signed		delta_y:13;      // change in y
         signed		padding0006:9;  // ***Placeholder
/*0010*/ signed		y_pos:19;           // y coord
         signed		animation:10;   // animation
         signed		padding0010:3;  // ***Placeholder
/*0014*/ unsigned	heading:12;     // heading
         signed		x_pos:19;           // x coord
         signed		padding0014:1;  // ***Placeholder
/*0018*/ signed		z_pos:19;           // z coord
         signed		delta_z:13;      // change in z
/*0022*/
};


/*
** Player position update - Size: 40
**	Struct sent from client->server to update
**	player position on server
**
*/
struct PlayerPositionUpdateClient_Struct
{
/*0000*/ uint16		spawn_id;			// Player's spawn id
/*0002*/ uint16		sequence;			// increments one each packet - Verified
/*0004*/ uint8		unknown0004[4];		// ***Placeholder
/*0008*/ float		x_pos;				// x coord (2nd loc value)
/*0012*/ float		y_pos;				// y coord (1st loc value)
/*0016*/ signed		delta_heading:10;	// change in heading
         unsigned	padding0036:10;		// animation
         unsigned	padding0016:12;		// ***Placeholder 
/*0020*/ float		delta_x;			// Change in x
/*0024*/ float		delta_y;			// Change in y
/*0028*/ float		z_pos;				// z coord (3rd loc value)
/*0032*/ float		delta_z;			// Change in z
/*0036*/ unsigned	animation:10;		// ***Placeholder 
         unsigned	heading:12;			// Directional heading
         unsigned	padding0037:10;		// ***Placeholder 
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

/*
** SendExpZonein
** Length: 152 Bytes
** OpCode: OP_SendExpZonein
*/
struct SendExpZonein_Struct
{
/*0000*/ uint16 spawn_id;          // ID of the spawn
/*0002*/ uint16 type;              // Values associated with the type
/*0004*/ uint32 parameter;         // Type of data sent
/*0008*/ uint32 exp;                    // Current experience ratio from 0 to 330
/*0012*/ uint32	expAA;
/*0016*/ uint8	unknown0016[4];
/*0020*/ char	name[64];
/*0084*/ char	last_name[64];
/*00148*/ int32	unknown132;
/*00152*/
};

/*
** SendExpZonein
** Length: 0 Bytes
** OpCode: OP_SendExpZonein
*/
//struct SendExpZonein_Struct {};

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
/*04*/	char			SerializedItem[1]; //was 1
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
/*0016*/
};

struct ItemNamePacket_Struct {
/*000*/	uint32 item_id;
/*004*/	uint32 unkown004;
/*008*/ char name[64];
/*072*/
};

// Length: 16
struct ItemProperties_Struct_Old {

/*000*/ int8	unknown01[2];
/*002*/ int8	charges;
/*003*/ int8	unknown02[13];
/*016*/
};

// Length: 8
struct ItemProperties_Struct {

/*000*/ int8	unknown01[4];
/*004*/ int8	charges;
/*005*/ int8	unknown02[3];
/*008*/
};

struct DeleteItem_Struct {
/*0000*/ uint32 from_slot;
/*0004*/ uint32 to_slot;
/*0008*/ uint32 number_in_stack;
/*0012*/
};

struct MoveItem_Struct
{
/*0000*/ uint32 from_slot;
/*0004*/ uint32 to_slot;
/*0008*/ uint32 number_in_stack;
/*0012*/
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

struct LootingItem_Struct {
/*000*/	int32	lootee;
/*004*/	int32	looter;
/*008*/	int32	slot_id;
/*012*/	int32	auto_loot;
/*016*/	int32	unknown16;
/*020*/
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

struct OnLevelMessage_Struct {
/*0000*/	char    Title[128];
/*0128*/	char    Text[4096];
/*4224*/	char	ButtonName0[25];	// If Buttons = 1, these two are the text for the left and right buttons respectively
/*4249*/	char	ButtonName1[25];
/*4274*/	uint8	Buttons;
/*4275*/	uint8	Unknown4275;	// Something to do with audio controls
/*4276*/	uint32  Duration;
/*4280*/	uint32  PopupID;	// If none zero, a response packet with 00 00 00 00 <PopupID> is returned on clicking the left button
/*4284*/	uint32  NegativeID;	// If none zero, a response packet with 01 00 00 00 <NegativeID> is returned on clicking the right button
/*4288*/	uint32  Unknown4288;
/*4292*/
};

//Combat Abilities
struct CombatAbility_Struct {
	int32 m_target;		//the ID of the target mob
	int32 m_atk;
	int32 m_skill;
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
	int16	year;
/*0006*/ uint16 unknown0016;            // Placeholder
/*0008*/
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
/*012*/	int32	unknown12;
/*016*/	int8	quantity;		// Already sold
/*017*/ int8	Unknown017[3];
/*020*/	uint32	Unknown020;
/*024*/	uint32	price;
/*028*/	uint32	pricehighorderbits;	// It appears the price is 64 bits in Live+
/*032*/
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
/*004*/	int32	npcid;
/*008*/	int32	itemid;
/*012*/	int32	variable;
};

struct Adventure_Sell_Struct {
/*000*/	int32	unknown000;	//0x01 - Stack Size/Charges?
/*004*/	int32	npcid;
/*008*/	int32	slot;
/*012*/	int32	charges;
/*016*/	int32	sell_price;
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

//this is mostly right but something is off that causes the client to crash sometimes
//I don't really care enough about the feature to work on it anymore though.
struct AdventureLeaderboardEntry_Struct
{
/*004*/ char name[64];
/*008*/ int32 success;
/*012*/ int32 failure;
/*016*/
};

struct AdventureLeaderboard_Struct
{
/*000*/ int32 unknown000;
/*004*/ int32 unknown004;
/*008*/ int32 success;
/*012*/ int32 failure;
/*016*/ int32 our_rank;
/*020*/	
};

/*struct Item_Shop_Struct {
	uint16 merchantid;
	int8 itemtype;
	Item_Struct item;
	int8 iss_unknown001[6];
};*/

struct Illusion_Struct {  //size: 256
/*000*/	uint32	spawnid;
/*004*/	char	charname[64];	//
/*068*/	uint16	race;			//
/*070*/	char	unknown006[2];	// Weird green name
/*072*/	uint8	gender;
/*073*/	uint8	texture;
/*074*/	uint8	unknown074;		//
/*075*/	uint8	unknown075;		//
/*076*/	uint8	helmtexture;	//
/*077*/	uint8	unknown077;		//
/*078*/	uint8	unknown078;		//
/*079*/	uint8	unknown079;		//
/*080*/	uint32	face;			//
/*084*/	uint8	hairstyle;		// Some Races don't change Hair Style Properly in SoF
/*085*/	uint8	haircolor;		// 
/*086*/	uint8	beard;			// 
/*087*/	uint8	beardcolor;		// 
/*088*/ float	size;			// 
/*092*/	uint8	unknown092[148];
/*240*/ int32	unknown240;		// Removes armor?
/*244*/ int32	drakkin_heritage;	// 
/*248*/ int32	drakkin_tattoo;		// 
/*252*/ int32	drakkin_details;	// 
/*256*/
};

struct ZonePoint_Entry { //28 octets
/*0000*/	int32	iterator;
/*0004*/	float	y;
/*0008*/	float	x;
/*0012*/	float	z;
/*0016*/	float	heading;
/*0020*/	int16	zoneid;
/*0022*/	int16	zoneinstance; // LDoN instance
/*0024*/    uint32  unknown024;
/*0028*/
};

struct ZonePoints {
/*0000*/	int32	count;
/*0004*/	struct	ZonePoint_Entry zpe[0]; // Always add one extra to the end after all zonepoints
//*0xxx*/    uint8     unknown0xxx[24]; //New from SEQ
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

struct GroupInvite_Struct {
/*0000*/	char	invitee_name[64];
/*0064*/	char	inviter_name[64];
/*0128*/	int32	unknown0128;
/*0132*/	int32	unknown0132;
/*0136*/	int32	unknown0136;
/*0140*/	int32	unknown0140;
/*0144*/	int32	unknown0144;
/*0148*/
};

struct GroupGeneric_Struct {
/*0000*/	char	name1[64];
/*0064*/	char	name2[64];
/*0128*/	int32	unknown0128;
/*0132*/	int32	unknown0132;
/*0136*/	int32	unknown0136;
/*0140*/	int32	unknown0140;
/*0144*/	int32	unknown0144;
/*0148*/
};

struct GroupCancel_Struct {
/*000*/	char	name1[64];
/*064*/	char	name2[64];
/*128*/	uint8	unknown128[20];
/*148*/	uint32	toggle;
/*152*/
};

struct GroupUpdate_Struct {
/*0000*/	int32	action;
/*0004*/	char	yourname[64];
/*0068*/	char	membername[5][64];
/*0388*/	char	leadersname[64];
/*0452*/
};

struct GroupUpdate2_Struct {
/*0000*/	int32	action;
/*0004*/	char	yourname[64];
/*0068*/	char	membername[5][64];
/*0388*/	char	leadersname[64];
/*0452*/	GroupLeadershipAA_Struct leader_aas;
/*0516*/	int8	unknown[252];	// Titanium uses [188] here
/*0768*/
};

struct GroupUpdate_Struct_Live {	// New for Live
/*0000*/	int32	groupid;		// Guess - Matches unknown0136 from GroupFollow_Struct
/*0004*/	int32	totalmembers;	// Guess
/*0000*/	//int32	leadersname[0];	// Group Leader Name Null Terminated
/*0008*/	//GroupMembers_Struct groupmembers;
};

struct GroupMembers_Struct {	// New for Live
/*0000*/	int32	membernumber;	// Guess - number of member in the group (0 to 5?)
/*0000*/	//char	membername[0];	// Member Name Null Terminated
/*0000*/	int8	unknown001[3];	// Seen 0
/*0000*/	int32	memberlevel;	// Guess
/*0000*/	int8	unknown002[11];	// Seen 0
};

struct GroupJoin_Struct_Live {	// New for Live
/*0000*/	int32	unknown0000;	// Matches unknown0136 from GroupFollow_Struct
/*0004*/	int32	action;
/*0008*/	int8	unknown0008[5];	// Seen 0
/*0013*/	//char	membername[0];	// Null Terminated?
/*0000*/	int8	unknown0013[3];	// Seen 0
/*0000*/	int32	unknown0016;	// Matches unknown0132 from GroupFollow_Struct
/*0000*/	int8	unknown0020[11];	// Seen 0
};

struct GroupJoin_Struct {
/*000*/	char	unknown000[64];
/*064*/	char	membername[64];
/*128*/	int8	unknown128[20];	// Leadership AA ?
/*148*/
};

struct GroupFollow_Struct { // Live Follow Struct
/*0000*/	char	name1[64];	// inviter
/*0064*/	char	name2[64];	// invitee
/*0128*/	int32	unknown0128;	// Seen 0
/*0132*/	int32	unknown0132;	// Group ID or member level?
/*0136*/	int32	unknown0136;	// Maybe Voice Chat Channel or Group ID?
/*0140*/	int32	unknown0140;	// Seen 0
/*0144*/	int32	unknown0144;	// Seen 0
/*0148*/	int32	unknown0148;
/*0152*/
};

struct LFG_Struct {
/*000*/	uint32 unknown000;
/*004*/	uint32 value; // 0x00 = off 0x01 = on
/*008*/	uint32 unknown008;
/*012*/	uint32 unknown012;
/*016*/	char	name[64];
};

struct FaceChange_Struct {
/*000*/	int8	haircolor;
/*001*/	int8	beardcolor;
/*002*/	int8	eyecolor1;
/*003*/	int8	eyecolor2;
/*004*/	int8	hairstyle;
/*005*/	int8	beard;
/*006*/	int8	face;
/*007*/ int8	unknown007;
/*008*/ int32	drakkin_heritage;
/*012*/ int32	drakkin_tattoo;
/*016*/ int32	drakkin_details;
/*020*/ int32	unknown020;
/*024*/
};
//there are only 10 faces for barbs changing woad just
//increase the face value by ten so if there were 8 woad
//designs then there would be 80 barb faces

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

/*068*/	int32	wclass;		// FF FF = no class
/*072*/	int32	lvllow;		// FF FF = no numbers
/*076*/	int32	lvlhigh;	// FF FF = no numbers
/*080*/	int32	gmlookup;	// FF FF = not doing /who all gm
/*084*/	int32	guildid;	// Also used for Buyer/Trader/LFG
/*088*/	int8	unknown088[64];
/*156*/	int32	type;		// 0 = /who 3 = /who all
};

struct Stun_Struct { // 8 bytes total
/*000*/	int32	duration; // Duration of stun
/*004*/	int8	unknown004; // seen 0
/*005*/	int8	unknown005; // seen 163
/*006*/	int8	unknown006; // seen 67
/*007*/	int8	unknown007; // seen 0
/*008*/
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
/*0004*/	char message[1024]; // was 1024
/*1028*/
};

// Inspect
struct Inspect_Struct {
	int16 TargetID;
	int16 PlayerID;
};

//OP_InspectAnswer - Size: 1860
struct InspectResponse_Struct{
/*000*/	int32 TargetID;
/*004*/	int32 playerid;
/*008*/	char itemnames[22][64];
/*1416*/char unknown_zero[64];
/*1480*/int32 itemicons[22];
/*1568*/int32 unknown_zero2;
/*1572*/char text[288];	// Max number of chars in Inspect Window appears to be 254
/*1860*/
};

//OP_SetDataRate
struct SetDataRate_Struct {
	float newdatarate;
};

//OP_SetServerFilter
struct SetServerFilter_Struct {
	uint32 filters[34];		//see enum eqFilterType [31]
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

// The BookText_Struct is not used in  SoF and later clients.
// The BookRequest_Struct is used instead for both request and reply.
//
struct BookText_Struct {
	uint8 window;	// where to display the text (0xFF means new window)
	uint8 type;             //type: 0=scroll, 1=book, 2=item info.. prolly
	char booktext[1]; // Variable Length - was 1
};
// This is the request to read a book.
// This is just a "text file" on the server
// or in our case, the 'name' column in our books table.
struct BookRequest_Struct {
/*0000*/	uint32 window;		// where to display the text (0xFFFFFFFF means new window).
/*0004*/	uint32 invslot;		// The inventory slot the book is in. Not used, but echoed in the response packet.
/*0008*/	uint32 type;		// 0 = Scroll, 1 = Book, 2 = Item Info. Possibly others
/*0012*/	uint32 unknown0012;	
/*0016*/	uint16 unknown0016;
/*0018*/	char txtfile[8194];
};

/*
** Object/Ground Spawn struct
** Used for Forges, Ovens, ground spawns, items dropped to ground, etc
** Size: 104 bytes
** OpCodes: OP_CreateObject
** Last Updated: Oct-17-2003
**
*/
struct Object_Struct {
/*00*/	uint32	linked_list_addr[2];// <Zaphod> They are, get this, prev and next, ala linked list
/*08*/	uint32	unknown008;			// Something related to the linked list?
/*12*/	uint32	drop_id;			// Unique object id for zone
/*16*/	uint16	zone_id;			// Redudant, but: Zone the object appears in
/*18*/	uint16	zone_instance;		//
/*20*/	uint32	unknown020;			// 00 00 00 00
/*24*/	uint32	unknown024;			// 53 9e f9 7e - same for all objects in the zone?
/*40*/	float	heading;			// heading
/*32*/	uint8	unknown032[8];		// 00 00 00 00 00 00 00 00
/*28*/	float	size;			// Size - default 1
/*44*/	float	z;					// z coord
/*48*/	float	x;					// x coord
/*52*/	float	y;					// y coord
/*56*/	char	object_name[16];	// Name of object, usually something like IT63_ACTORDEF was [20]
/*72*/	float	unknown072;			// seems like coords, not always valid, all 0 on most world objects
/*76*/	float	unknown076;			// seems like coords, not always valid, all 0 on most world objects
/*80*/	float	unknown080;			// seems like coords, not always valid, all 0 on most world objects
/*84*/	float	unknown084;			// seems like coords, not always valid, all 0 on most world objects
/*88*/	uint32	unknown088;			// unique ID?  Maybe for a table that includes the contents?
/*92*/	uint32	object_type;		// Type of object, not directly translated to OP_OpenObject
/*96*/	uint8	unknown096[4];		// ff ff ff ff
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
/*0052*/ int32   size;			// 100 is normal, smaller number = smaller model
/*0054*/ int8    unknown0054[4]; // 00 00 00 00
/*0060*/ uint8   doorId;             // door's id #
/*0061*/ uint8   opentype;
/*0062*/ uint8  state_at_spawn;  
/*0063*/ uint8  invert_state;	// if this is 1, the door is normally open
/*0064*/ int32  door_param; // normally ff ff ff ff (-1)
/*0068*/ int32	unknown0068; // 00 00 00 00
/*0072*/ int32	unknown0072; // 00 00 00 00
/*0076*/ int8	unknown0076; // seen 1 or 0
/*0077*/ int8	unknown0077; // seen 1 (always?)
/*0078*/ int8	unknown0078; // seen 0 (always?)
/*0079*/ int8	unknown0079; // seen 1 (always?)
/*0080*/ int8	unknown0080; // seen 0 (always?)
/*0081*/ int8	unknown0081; // seen 1 or 0 or rarely 2C or 90 or ED or 2D or A1
/*0082*/ uint8  unknown0082; // seen 0 or rarely FF or FE or 10 or 5A or 82
/*0083*/ uint8  unknown0083; // seen 0 or rarely 02 or 7C
/*0084*/ uint8  unknown0084[8]; // mostly 0s, the last 3 bytes are something tho
/*0092*/
};

struct DoorSpawns_Struct {
	struct Door_Struct doors[0];
};

/*
 OP Code: 	Op_ClickDoor
 Size:		16
*/
struct ClickDoor_Struct {
/*000*/	int8	doorid;
/*001*/	int8	unknown001;		// This may be some type of action setting
/*002*/	int8	unknown002;		// This is sometimes set after a lever is closed
/*003*/	int8	unknown003;		// Seen 0
/*004*/	int8	picklockskill;
/*005*/	int8	unknown005[3];
/*008*/ int32	item_id;
/*012*/ int16	player_id;
/*014*/ int8	unknown014[2];
/*016*/
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
	int8 mode;                    //01=run  00=walk
	int8 unknown[3];
};

// EnvDamage is EnvDamage2 without a few bytes at the end.
// Size: 37 bytes
struct EnvDamage2_Struct {
/*0000*/	int32 id;
/*0004*/	int16 unknown4;
/*0006*/	int32 damage;
/*0010*/	float unknown10;	// New to Underfoot - Seen 1
/*0014*/	int8 unknown14[12];
/*0026*/	int8 dmgtype;		// FA = Lava; FC = Falling
/*0027*/	int8 unknown27[4];
/*0031*/	int16 unknown31;	// New to Underfoot - Seen 66
/*0033*/	int16 constant;		// Always FFFF
/*0035*/	int16 unknown35;
/*0037*/
};

//Bazaar Stuff

enum {
	BazaarTrader_StartTraderMode = 1,
	BazaarTrader_EndTraderMode = 2,
	BazaarTrader_UpdatePrice = 3,
	BazaarTrader_EndTransaction = 4,
	BazaarSearchResults = 7,
	BazaarWelcome = 9,
	BazaarBuyItem = 10,
	BazaarTrader_ShowItems = 11,
	BazaarSearchDone = 12,
	BazaarTrader_CustomerBrowsing = 13
};

enum {
	BazaarPriceChange_Fail = 0,
	BazaarPriceChange_UpdatePrice = 1,
	BazaarPriceChange_RemoveItem = 2,
	BazaarPriceChange_AddItem = 3
};

struct BazaarWindowStart_Struct {
	int8   Action;
	int8   Unknown001;
	int16  Unknown002;
};


struct BazaarWelcome_Struct {
	BazaarWindowStart_Struct Beginning;
	int32  Traders;
	int32  Items;
	int8   Unknown012[8];
};

struct BazaarSearch_Struct {
	BazaarWindowStart_Struct Beginning;
	int32	TraderID;
	int32	Class_;
	int32	Race;
	int32	ItemStat;
	int32	Slot;
	int32	Type;
	char	Name[64];
	int32	MinPrice;
	int32	MaxPrice;
	int32	Minlevel;
	int32	MaxLlevel;
};
struct BazaarInspect_Struct{
	int32 ItemID;
	int32 Unknown004;
	char Name[64];
};

struct NewBazaarInspect_Struct {
/*000*/	BazaarWindowStart_Struct Beginning;
/*004*/	char Name[64];
/*068*/	int32 Unknown068;
/*072*/	sint32 SerialNumber;
/*076*/	int32 Unknown076;
/*080*/	int32 SellerID;
/*084*/	int32 Unknown084;
};

struct BazaarReturnDone_Struct{
	int32 Type;
	int32 TraderID;
	int32 Unknown008;
	int32 Unknown012;
	int32 Unknown016;
};

struct BazaarSearchResults_Struct {
/*000*/	BazaarWindowStart_Struct Beginning;
/*004*/	int32	SellerID;
/*008*/	char	SellerName[64];
/*072*/	int32	NumItems;
/*076*/	int32	ItemID;
/*080*/	int32	SerialNumber;
/*084*/	int32	Unknown084;
/*088*/	char	ItemName[64];
/*152*/	int32	Cost;
/*156*/	int32	ItemStat;
/*160*/
};

struct ServerSideFilters_Struct {
int8	clientattackfilters; // 0) No, 1) All (players) but self, 2) All (players) but group
int8	npcattackfilters;	 // 0) No, 1) Ignore NPC misses (all), 2) Ignore NPC Misses + Attacks (all but self), 3) Ignores NPC Misses + Attacks (all but group)
int8	clientcastfilters;	 // 0) No, 1) Ignore PC Casts (all), 2) Ignore PC Casts (not directed towards self)
int8	npccastfilters;		 // 0) No, 1) Ignore NPC Casts (all), 2) Ignore NPC Casts (not directed towards self)
};

/*
** Client requesting item statistics
** Size: 48 bytes
** Used In: OP_ItemLinkClick
** Last Updated: 2/15/2009
**
*/
struct	ItemViewRequest_Struct {
/*000*/	uint32	item_id;
/*004*/	uint32	augments[5];
/*024*/ uint32	link_hash;
/*028*/	uint32	unknown028;	//seems to always be 4 on SoF client
/*032*/	char	unknown032[12];	//probably includes loregroup & evolving info. see Client::MakeItemLink() in zone/inventory.cpp:469
/*044*/	uint16	icon;
/*046*/	char	unknown046[2];
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
/*004*/	uint8	enable_pvp;
/*005*/	uint8	unknown005;
/*006*/	uint8	unknown006;
/*007*/	uint8	unknown007;
/*008*/	uint8	enable_FV;
/*009*/	uint8	unknown009;
/*010*/	uint8	unknown010;
/*011*/	uint8	unknown011;
/*012*/	uint32	unknown012;	// htonl(1) on live
/*016*/	uint32	unknown016;	// htonl(1) on live
/*020*/	uint8	unknown020[12];
/*032*/ uint32	unknown032;
/*036*/	char	worldshortname[32];
/*068*/	uint8	unknown064[32];
/*100*/	char	unknown096[16];	// 'pacman' on live
/*116*/	char	unknown112[16];	// '64.37,148,36' on live
/*132*/	uint8	unknown128[48];
/*180*/	uint32	unknown176;	// htonl(0x00002695)
/*184*/	char	unknown180[80];	// 'eqdataexceptions@mail.station.sony.com' on live
/*264*/	uint8	unknown260;	// 0x01 on live
/*265*/	uint8	enablevoicemacros;
/*266*/	uint8	enablemail;
/*267*/	uint8	unknown263[41];
/*308*/
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

// Looks like new tracking structures - Opcode: 0x57a7
struct Tracking_Struct_New {
	int16 totalcount;			// Total Count of mobs within tracking range
	Track_Struct Entrys[0];
};

struct Track_Struct_New {
	int16 entityid;				// Entity ID
	int16 unknown002;			// 00 00
	int32 unknown004;			// 
	int8  level;				// level of mob
	int8  unknown009;			// 01 maybe type of mob? player/npc?
	char  name[1];				// name of mob
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

// The following four structs are the WhoAllPlayer struct above broken down
// for use in World ClientList::SendFriendsWho to accomodate the user of variable
// length strings within the struct above.

struct	WhoAllPlayerPart1 {
	int32	FormatMSGID;
	int32	Unknown04;
	int32	Unknown08;
	char	Name[1];
};

struct	WhoAllPlayerPart2 {
	int32	RankMSGID;
	char	Guild[1];
};

struct	WhoAllPlayerPart3 {
	int32	Unknown80[2];
	int32	ZoneMSGID;
	int32	Zone;
	int32	Class_;
	int32	Level;
	int32	Race;
	char	Account[1];
};

struct	WhoAllPlayerPart4 {
	int32	Unknown100;
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

struct TraderBuy_Struct {
/*000*/ int32   Action;
/*004*/	int32	Unknown004;
/*008*/ int32   Price;
/*012*/	int32	Unknown008;	// Probably high order bits of a 64 bit price.
/*016*/ int32   TraderID;
/*020*/ char    ItemName[64];
/*084*/ int32   Unknown076;
/*088*/ int32   ItemID;
/*092*/ int32   AlreadySold;
/*096*/ int32   Quantity;
/*100*/ int32   Unknown092;
/*104*/
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

//struct MoneyUpdate_Struct
//{
//*0000*/ uint32 spawn_id;            // ***Placeholder
//*0004*/ uint32 cointype;           // Coin Type
//*0008*/ uint32 amount;             // Amount
//*0012*/
//};


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
//*0???*/ uint8  unknown0[8];         // ***Placeholder
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
/*0136*/	char	motd[0]; //was 512
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
	uint32	unknown000;
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


#if 0
// Old struct not used by Task System implementation but left for reference
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
#endif

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

#if 0
// Old structs not used by Task System implentation but left for reference
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
#endif

struct AcceptNewTask_Struct {
	uint32  unknown00;
	uint32	task_id;		//set to 0 for 'decline'
	uint32	task_master_id;	//entity ID
};

//was all 0's from client, server replied with same op, all 0's
struct CancelTask_Struct {
	uint32 SequenceNumber;
	uint32 unknown4; // Only seen 0x00000002
};

#if 0
// old struct, not used by Task System implementation but left for reference.
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
#endif


// Many of the Task System packets contain variable length strings, as well as variable numbers
// of records, hence splitting them into multiple structs (header, middle, trailer) etc.
//
struct AvailableTaskHeader_Struct {
	uint32	TaskCount;
	uint32	unknown1;
	uint32	TaskGiver;
};

struct AvailableTaskData1_Struct {
	uint32	TaskID;
	uint32	unknown1;
	uint32	TimeLimit;
	uint32	unknown2;
};

struct AvailableTaskData2_Struct {
	uint32	unknown1,unknown2,unknown3,unknown4;
};

struct AvailableTaskTrailer_Struct {
	uint32	ItemCount;
	uint32	unknown1, unknown2;
	uint32	StartZone;
};

struct TaskDescriptionHeader_Struct {
	uint32	SequenceNumber; // The order the tasks appear in the journal. 0 for first task, 1 for second, etc.
	uint32	TaskID;
	uint32	unknown2;
	uint32	unknown3;
	uint8	unknown4;
};

struct TaskDescriptionData1_Struct {
	uint32	Duration;
	uint32	unknown2;
	uint32	StartTime;
};

struct TaskDescriptionData2_Struct {
	uint32	RewardCount; // ??
	uint32	unknown1;
	uint32	unknown2;
	uint16	unknown3;
	//uint8	unknown4;
};

struct TaskDescriptionTrailer_Struct {
	//uint16  unknown1; // 0x0012
	uint32	Points;
};

struct TaskActivityHeader_Struct {
	uint32	TaskSequenceNumber;
	uint32	unknown2; // Seen 0x00000002
	uint32	TaskID;
	uint32	ActivityID;
	uint32	unknown3;
	uint32	ActivityType;
	uint32	Optional;
	uint32	unknown5;
};

struct TaskActivityData1_Struct {
	uint32	GoalCount;
	uint32	unknown1; // 0xffffffff
	uint32	unknown2; // 0xffffffff
	uint32	ZoneID; // seen 0x36
	uint32  unknown3;
};

struct TaskActivityTrailer_Struct {
	uint32	DoneCount;
	uint32	unknown1; // Seen 1
};

// The Short_Struct is sent for tasks that are hidden and act as a placeholder
struct TaskActivityShort_Struct {
	uint32	TaskSequenceNumber;
	uint32	unknown2; // Seen 0x00000002
	uint32	TaskID;
	uint32	ActivityID;
	uint32	unknown3;
	uint32	ActivityType; // 0xffffffff for the short packet
	uint32  unknown4;
};

struct TaskActivityComplete_Struct {
	uint32	TaskIndex;
	uint32	unknown2; // 0x00000002
	uint32	unknown3;
	uint32	ActivityID;
	uint32	unknown4; // 0x00000001
	uint32	unknown5; // 0x00000001
};

#if 0
// This is a dupe of the CancelTask struct
struct TaskComplete_Struct {
	uint32	unknown00; // 0x00000000
	uint32	unknown04; // 0x00000002
};
#endif

struct TaskHistoryRequest_Struct {
	uint32	TaskIndex; // This is the sequence the task was sent in the Completed Tasks packet.
};

struct TaskHistoryReplyHeader_Struct {
	uint32	TaskID;
	uint32	ActivityCount;
};

struct TaskHistoryReplyData1_Struct {
	uint32	ActivityType;
};

struct TaskHistoryReplyData2_Struct {
	uint32  GoalCount;
	uint32	unknown04; // 0xffffffff
	uint32	unknown08; // 0xffffffff
	uint32	ZoneID;
	uint32	unknown16;
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
/*00*/	double	group_leadership_exp;
/*08*/	uint32	group_leadership_points;
/*12*/	uint32	Unknown12;
/*16*/	double	raid_leadership_exp;
/*24*/	uint32	raid_leadership_points;
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
/*00*/	uint32		action;
/*04*/	char		player_name[64];
/*68*/	uint32		unknown68;
/*72*/	char		leader_name[64];
/*136*/	uint32		parameter;
};

struct RaidAddMember_Struct {
/*000*/ RaidGeneral_Struct raidGen; //param = (group num-1); 0xFFFFFFFF = no group
/*136*/ uint8 _class;
/*137*/	uint8 level;
/*138*/	uint8 isGroupLeader;
/*139*/	uint8 flags[5]; //no idea if these are needed...
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
/*000*/	float	src_y;
/*004*/	float	src_x;
/*008*/	float	src_z;
/*012*/	int8	unknown012[12];
/*024*/	float	velocity;		//4 is normal, 20 is quite fast
/*028*/	float	launch_angle;	//0-450ish, not sure the units, 140ish is straight
/*032*/	float	tilt;		//on the order of 125
/*036*/	int8	unknown036[8];
/*044*/	float	arc;
/*048*/	uint32	source_id;
/*052*/	uint32	target_id;	//entity ID
/*056*/	uint32	item_id;
/*060*/	uint32	unknown060;
/*064*/	uint32	unknown064;
/*068*/	uint8	unknown068;
/*069*/	uint8	unknown069;
/*070*/	uint8	unknown070;
/*071*/	uint8	item_type;
/*072*/	uint8	skill;
/*073*/	char	model_name[43];
/*116*/
};

//made a bunch of trivial structs for stuff for opcode finder to use
struct Consent_Struct {
	char name[1];	//always at least a null - was 1
};

struct AdventureMerchant_Struct {
	uint32	unknown_flag;		//seems to be 1
	uint32	entity_id;
};

struct Save_Struct {
	int8	unknown00[192];
	int8	unknown0192[176];
};

struct GMToggle_Struct {
	int8 unknown0[64];
	uint32 toggle;
};

struct BuffFadeMsg_Struct {
	uint32 color;
	char msg[1]; //was 1
/*0???*/ uint8  paddingXXX[3];          // always 0's
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
/*16*/
};

struct SendAA_Struct {
/*0000*/	int32 id;
/*0004*/	int8 unknown004;		// int32 unknown004; set to 1.
/*0005*/	int32 hotkey_sid;
/*0009*/	int32 hotkey_sid2;
/*0013*/	int32 title_sid;
/*0017*/	int32 desc_sid;
/*0021*/	int32 class_type;
/*0025*/	int32 cost;
/*0029*/	int32 seq;
/*0033*/	int32 current_level; //1s, MQ2 calls this AARankRequired
/*0037*/	int32 prereq_skill;		//is < 0, abs() is category #
/*0041*/	int32 prereq_minpoints; //min points in the prereq
/*0045*/	int32 type;
/*0049*/	int32 spellid;
/*0053*/	int32 spell_type;
/*0057*/	int32 spell_refresh;
/*0061*/	int16 classes;
/*0063*/	int16 berserker; //seems to be 1 if its a berserker ability
/*0065*/	int32 max_level;
/*0069*/	int32 last_id;
/*0073*/	int32 next_id;
/*0077*/	int32 cost2;
/*0081*/	int8 unknown80[7];
/*0088*/	int32 aa_expansion;
/*0092*/	int32 special_category;
/*0096*/	int32 unknown0096;
/*0100*/	int32 total_abilities;
/*0104*/	AA_Ability abilities[0];
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
/*00*/	int32	aa_skill;						// Total AAs Spent
/*04*/  int32	aa_value;
/*08*/  int32	unknown08;
};

struct AAExpUpdate_Struct {
/*00*/	int32 unknown00;	//seems to be a value from AA_Action.ability
/*04*/	int32 aapoints_unspent;
/*08*/	int8 aaxp_percent;	//% of exp that goes to AAs
/*09*/	int8 unknown09[3];	//live doesn't always zero these, so they arnt part of aaxp_percent
};


struct AltAdvStats_Struct {
/*000*/  uint32 experience;
/*004*/  uint16 unspent;
/*006*/  uint16	unknown006;
/*008*/  int8	percentage;
/*009*/  int8	unknown009[3];
};

struct PlayerAA_Struct {						// Is this still used?
	AA_Skills aa_list[MAX_PP_AA_ARRAY];
};

struct AATable_Struct {
/*00*/ sint32	aa_spent;						// Total AAs Spent
/*04*/ AA_Skills aa_list[MAX_PP_AA_ARRAY];
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

struct LoadSpellSet_Struct {
      uint32  spell[MAX_PP_MEMSPELL];      // 0xFFFFFFFF if no action, slot number if to unmem starting at 0
      uint32 unknown;	//there seems to be an extra field in this packet...
};

struct BlockedBuffs_Struct {
/*000*/	uint8	unknown000[80];
/*080*/	uint8	unknown081;
/*081*/	uint8	unknown082;
/*082*/	uint8	unknown083;
/*083*/	uint8	unknown084;
/*084*/	uint8	unknown085;
/*085*/	uint8	unknown086;
/*086*/	uint8	unknown087;
/*087*/	uint8	unknown088;
/*088*/
};

//Size 24 Bytes
struct WorldObfuscator_Struct
{
/*000*/ uint32 var1;
/*004*/ uint32 Unknown1;
/*008*/ uint32 Unknown2;
/*012*/ uint32 Unknown3;
/*016*/ uint32 var2;
/*020*/ uint32 Unknown4;
/*024*/
};

struct ExpansionInfo_Struct {
/*000*/	char	Unknown000[64];
/*064*/	uint32	Expansions;
};

struct ApplyPoison_Struct {
	uint32 inventorySlot;
	uint32 success;
};

struct ItemVerifyRequest_Struct {
/*000*/	sint32	slot;		// Slot being Right Clicked
/*004*/	uint32	target;		// Target Entity ID
/*008*/
};

struct ItemVerifyReply_Struct {
/*000*/	sint32	slot;		// Slot being Right Clicked
/*004*/	uint32	spell;		// Spell ID to cast if different than item effect
/*008*/	uint32	target;		// Target Entity ID
/*012*/
};

struct ItemSerializationHeader
{
/*000*/	uint32 stacksize;
/*004*/	uint32 unknown004;
/*008*/	uint32 slot;
/*012*/	uint32 price;
/*016*/	uint32 merchant_slot; //1 if not a merchant item
/*020*/	uint32 unknown020; //0
/*024*/	uint32 instance_id; //unique instance id if not merchant item, else is merchant slot
/*028*/	uint32 unknown028; //0
/*032*/	uint32 last_cast_time;	// Unix Time from PP of last cast for this recast type if recast delay > 0
/*036*/	uint32 charges; //Total Charges an item has (-1 for unlimited)
/*040*/	uint32 inst_nodrop; // 1 if the item is no drop (attuned items)
/*044*/	uint32 unknown044; //0
/*048*/	uint32 unknown048; //0
/*052*/	uint32 unknown052; //0
/*056*/	uint32 unknown056; //0
/*060*/	uint8 unknown060; //0
/*061*/	uint8 unknown061; //0 - Add Evolving Item struct if this isn't set to 0?
/*062*/	uint8 unknown062; // New to Live
/*063*/	uint8 ItemClass; //0, 1, or 2
};

struct ItemBodyStruct
{
	uint32 id;
	uint8 weight;
	uint8 norent;
	uint8 nodrop;
	uint8 attune;
	uint8 size;
	uint32 slots;
	uint32 price;
	uint32 icon;
	uint8 unknown1;
	uint8 unknown2;
	uint32 BenefitFlag; 
	uint8 tradeskills;
	sint8 CR;
	sint8 DR;
	sint8 PR;
	sint8 MR;
	sint8 FR;
	sint8 SVCorruption;
	sint8 AStr;
	sint8 ASta;
	sint8 AAgi;
	sint8 ADex;
	sint8 ACha;
	sint8 AInt;
	sint8 AWis;
	sint32 HP;
	sint32 Mana;
	uint32 Endur;
	sint32 AC;
	sint32 regen;
	sint32 mana_regen;
	sint32 end_regen;
	uint32 Classes;
	uint32 Races;
	uint32 Deity;
	sint32 SkillModValue;
	uint32 unknown5;
	uint32 SkillModType;
	uint32 BaneDmgRace;
	uint32 BaneDmgBody;
	uint32 BaneDmgRaceAmt;
	sint32 BaneDmgAmt;
	uint8 Magic;
	sint32 CastTime_;
	uint32 ReqLevel;
	uint32 RecLevel;
	uint32 RecSkill;
	uint32 BardType;
	sint32 BardValue;
	uint8 Light;
	uint8 Delay;
	uint8 ElemDmgType;
	uint8 ElemDmgAmt;
	uint8 Range;
	uint32 Damage;
	uint32 Color;
	uint8 ItemType;
	uint32 Material;
	uint32 unknown7;
	uint32 EliteMaterial;
	float SellRate;
	sint32 CombatEffects;
	sint32 Shielding;
	sint32 StunResist;
	sint32 StrikeThrough;
	sint32 ExtraDmgSkill;
	sint32 ExtraDmgAmt;
	sint32 SpellShield;
	sint32 Avoidance;
	sint32 Accuracy;
	uint32 CharmFileID; //not sure the point of this; disables all stats on item if > 0
	uint32	FactionMod1;
	sint32	FactionAmt1;
	uint32	FactionMod2;
	sint32	FactionAmt2;
	uint32  FactionMod3;
	sint32	FactionAmt3;
	uint32  FactionMod4;
	sint32	FactionAmt4;
};

struct AugSlotStruct
{
	uint32 type;
	uint8 visible;
	uint8 unknown;
};

struct ItemTertiaryBodyStruct
{
	sint32 loregroup;
	uint8 artifact;
	uint8 summonedflag;
	uint32 favor;
	uint8 fvnodrop;
	sint32 dotshield;
	sint32 atk;
	sint32 haste;
	sint32 damage_shield;
	uint32 guildfavor;
	uint32 augdistil;
	sint32 unknown3;
	uint32 unknown4;
	uint8 no_pet;
	uint8 unknown5;

	uint8 potion_belt_enabled;
	uint32 potion_belt_slots;

	uint32 stacksize;
	uint8 no_transfer;
	uint16 expendablearrow;

	uint32 unknown8; 
	uint32 unknown9; 
	uint32 unknown10; 
	uint32 unknown11;
	uint8 unknown12;
	uint8 unknown13;
	uint8 unknown14;
};

struct ClickEffectStruct
{
	sint32 effect;
	uint8 level2;
	uint32 type;
	uint8 level;
	sint32 max_charges;
	sint32 cast_time;
	uint32 recast;
	sint32 recast_type;
	uint32 clickunk5;
	//uint8 effect_string; //unused
	//sint32 clickunk7;
};

struct ProcEffectStruct
{
	uint32 effect;
	uint8 level2;
	uint32 type;
	uint8 level;
	uint32 unknown1; // poison?
	uint32 unknown2;
	uint32 unknown3;
	uint32 unknown4;
	uint32 procrate;
	//uint8 effect_string;
	//uint32 unknown5;
};

struct WornEffectStruct //worn, focus and scroll effect
{
	uint32 effect;
	uint8 level2;
	uint32 type;
	uint8 level;
	uint32 unknown1;
	uint32 unknown2;
	uint32 unknown3;
	uint32 unknown4;
	uint32 unknown5;
	//uint8 effect_string;
	//uint32 unknown6;
};


struct ItemSecondaryBodyStruct{
	uint32 augtype;
	uint32 augrestrict;
	AugSlotStruct augslots[5];

	uint32 ldonpoint_type;
	uint32 ldontheme;
	uint32 ldonprice;
	uint32 ldonsellbackrate;
	uint32 ldonsold;

	uint8 bagtype;
	uint8 bagslots;
	uint8 bagsize;
	uint8 wreduction;

	uint8 book;
	uint8 booktype;
	//sint32 filename; filename is either 0xffffffff/0x00000000 or the null term string ex: CREWizardNote\0
};

struct ItemQuaternaryBodyStruct
{

	uint32 scriptfileid;
	uint8 quest_item;
	uint32 unknown15; //0xffffffff - Power Source Capacity?
	uint32 Purity;
	uint32 BackstabDmg;
	uint32 DSMitigation;
	sint32 HeroicStr;
	sint32 HeroicInt;
	sint32 HeroicWis;
	sint32 HeroicAgi;
	sint32 HeroicDex;
	sint32 HeroicSta;
	sint32 HeroicCha;
	sint32 HeroicMR;
	sint32 HeroicFR;
	sint32 HeroicCR;
	sint32 HeroicDR;
	sint32 HeroicPR;
	sint32 HeroicSVCorrup;
	sint32 HealAmt;
	sint32 SpellDmg;
	sint32 clairvoyance;
	uint8 unknown18;	//Power Source Capacity or evolve filename?
	uint32 evolve_string; // Some String, but being evolution related is just a guess
	uint8 unknown19;	
	uint32 unknown20;	// Bard Stuff?
	uint32 unknown21;
	uint32 unknown22;
	uint32 subitem_count;
};

struct AugmentInfo_Struct
{
/*000*/ uint32	itemid;		// id of the solvent needed
/*004*/ uint8	window;		// window to display the information in
/*005*/	uint8	unknown005[71];	// total packet length 76, all the rest were always 00
/*076*/
};

struct VeteranRewardItem
{
/*000*/	int32 item_id;
/*004*/	int32 charges;
/*008*/	char item_name[64];
};

struct VeteranReward
{
/*000*/	int32 claim_id;
/*004*/	int32 number_available;
/*008*/	int32 claim_count;
/*012*/	VeteranRewardItem items[8];
};

struct ExpeditionEntryHeader_Struct
{
/*000*/ uint32 unknown000;
/*000*/ uint32 number_of_entries;
};

struct ExpeditionJoinPrompt_Struct
{
/*000*/ uint32 clientid;
/*004*/ uint32 unknown004;
/*008*/ char player_name[64];
/*072*/ char expedition_name[64];
};

struct ExpeditionExpireWarning
{
/*000*/ uint32 clientid;
/*004*/ uint32 unknown004;
/*008*/ uint32 minutes_remaining;
};

struct ExpeditionInfo_Struct
{
/*000*/ uint32 clientid;
/*004*/ uint32 unknown004;
/*008*/ uint32 unknown008;
/*012*/ uint32 max_players;
/*016*/ char expedition_name[128];
/*142*/ char leader_name[64];
};

struct ExpeditionCompassEntry_Struct
{
/*000*/ float unknown000; //seen *((uint32*)) = 1584791871
/*004*/ uint32 enabled; //guess
/*008*/ uint32 unknown008; //seen 1019
/*012*/ float y;
/*016*/ float x;
/*020*/ float z;
};

struct ExpeditionCompass_Struct
{
/*000*/ uint32 clientid;
/*004*/ uint32 count;
/*008*/ ExpeditionCompassEntry_Struct entries[0];
};

	};	//end namespace structs
};	//end namespace Live

#endif /*Live_STRUCTS_H_*/
