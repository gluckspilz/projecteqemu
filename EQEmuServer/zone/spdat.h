/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2005  EQEMu Development Team (http://eqemulator.net)

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
#ifndef SPDAT_H
#define SPDAT_H

#include "../common/classes.h"
#include "../common/skills.h"

#define SPELL_UNKNOWN 0xFFFF
#define SPELLBOOK_UNKNOWN 0xFFFFFFFF		//player profile spells are 32 bit

//some spell IDs which will prolly change, but are needed
#define SPELL_LEECH_TOUCH 2766
#define SPELL_LAY_ON_HANDS 87
#define SPELL_HARM_TOUCH 88
#define SPELL_HARM_TOUCH2 2821
#define SPELL_IMP_HARM_TOUCH 2774
#define SPELL_NPC_HARM_TOUCH 929


//#define SPDAT_SIZE		1824000
/* 
   solar: look at your spells_en.txt and find the id of the last spell.
   this number has to be 1 more than that.  if it's higher, your zone will
   NOT start up.  gonna autodetect this later..
*/
//#define NEW_LoadSPDat
#define DB_LoadSPDat	//load from DB vs spells_us.txt. for now, we're piggybacking NEW_LoadSPDat, so it will take precedence

#define EFFECT_COUNT 12
#define MAX_SPELL_TRIGGER 12	// One for each slot(only 6 for AA since AA use 2)

const int Z_AGGRO=10;

const int MobAISpellRange=100; // max range of buffs
const int SpellType_Nuke=1;
const int SpellType_Heal=2;
const int SpellType_Root=4;
const int SpellType_Buff=8;
const int SpellType_Escape=16;
const int SpellType_Pet=32;
const int SpellType_Lifetap=64;
const int SpellType_Snare=128;
const int SpellType_DOT=256;
const int SpellType_Dispel=512;
const int SpellType_InCombatBuff=1024;
const int SpellType_Mez=2048;
const int SpellType_Charm=4096;

const int SpellTypes_Detrimental = SpellType_Nuke|SpellType_Root|SpellType_Lifetap|SpellType_Snare|SpellType_DOT|SpellType_Dispel|SpellType_Mez|SpellType_Charm;
const int SpellTypes_Beneficial = SpellType_Heal|SpellType_Buff|SpellType_Escape|SpellType_Pet|SpellType_InCombatBuff;

#define SpellType_Any		0xFFFF

enum SpellAffectIndex {
	SAI_Calm			= 12, // Lull and Alliance Spells
	SAI_Dispell_Sight	= 14, // Dispells and Spells like Bind Sight
	SAI_Memory_Blur		= 27,
	SAI_Calm_Song		= 43  // Lull and Alliance Songs
};
enum RESISTTYPE
{
	RESIST_NONE = 0,
	RESIST_MAGIC = 1,
	RESIST_FIRE = 2,
	RESIST_COLD = 3,
	RESIST_POISON = 4,
	RESIST_DISEASE = 5,
	RESIST_CHROMATIC = 6,
	RESIST_PRISMATIC = 7,
	RESIST_PHYSICAL = 8,	// see Muscle Shock, Back Swing
	RESIST_CORRUPTION = 9	
};

//Target Type IDs
typedef enum {
	ST_TargetOptional = 0x01,
	ST_AEClientV1 = 0x02,
	ST_GroupTeleport = 0x03,
	ST_AECaster = 0x04,
	ST_Target = 0x05,
	ST_Self = 0x06,
	ST_AETarget = 0x08,
	ST_Animal = 0x09,
	ST_Undead = 0x0a,
	ST_Summoned = 0x0b,
	ST_Tap = 0x0d,
	ST_Pet = 0x0e,
	ST_Corpse = 0x0f,
	ST_Plant = 0x10,
	ST_Giant = 0x11, //special giant
	ST_Dragon = 0x12, //special dragon
	ST_TargetAETap = 0x14,
	ST_UndeadAE = 0x18,
	ST_SummonedAE = 0x19,
	ST_AECaster2 = 0x20, //ae caster hatelist maybe?
	ST_HateList = 0x21,
	ST_LDoNChest_Cursed = 0x22,
	ST_Muramite = 0x23, //only works on special muramites
	ST_AreaClientOnly = 0x24,
	ST_AreaNPCOnly = 0x25,
	ST_SummonedPet = 0x26,
	ST_GroupNoPets = 0x27,
	ST_AEBard = 0x28,
	ST_Group = 0x29,
	ST_Directional = 0x2a, //ae around this target between two angles
	ST_GroupClientAndPet = 0x2b,
	ST_Beam = 0x2c, //like directional but facing in front of you always
} SpellTargetType;

typedef enum  {
	DS_DECAY = 244,
	DS_CHILLED = 245,
	DS_FREEZING = 246,
	DS_TORMENT = 247,
	DS_BURN = 248,
	DS_THORNS = 249
} DmgShieldType;

//Spell Effect IDs
#define SE_CurrentHP					0	// implemented - Heals and nukes, repeates every tic if in a buff
#define SE_ArmorClass					1	// implemented
#define SE_ATK							2	// implemented
#define SE_MovementSpeed				3	// implemented - SoW, SoC, etc
#define SE_STR							4	// implemented
#define SE_DEX							5	// implemented
#define SE_AGI							6	// implemented
#define SE_STA							7	// implemented
#define SE_INT							8	// implemented
#define SE_WIS							9	// implemented
#define SE_CHA							10	// implemented - used as a spacer
#define SE_AttackSpeed					11	// implemented
#define SE_Invisibility					12	// implemented
#define SE_SeeInvis						13	// implemented
#define SE_WaterBreathing				14	// implemented
#define SE_CurrentMana					15	// implemented
//#define SE_Unknown16					16	// not used
//#define SE_Unknown17					17	// not used
#define SE_Lull							18	// implemented - Reaction Radius
#define SE_AddFaction					19	// implemented - Alliance line
#define SE_Blind						20	// implemented
#define SE_Stun							21	// implemented
#define SE_Charm						22	// implemented
#define SE_Fear							23	// implemented
#define SE_Stamina						24	// implemented - Invigor and such
#define SE_BindAffinity					25	// implemented
#define SE_Gate							26	// implemented - Gate to bind point
#define SE_CancelMagic					27	// implemented
#define SE_InvisVsUndead				28	// implemented
#define SE_InvisVsAnimals				29	// implemented
#define SE_ChangeFrenzyRad				30	// implemented - Pacify
#define SE_Mez							31	// implemented
#define SE_SummonItem					32	// implemented
#define SE_SummonPet					33	// implemented
//#define SE_Unknown34					34	// not used
#define SE_DiseaseCounter				35	// implemented
#define SE_PoisonCounter				36	// implemented
//#define SE_Unknown37					37	// not used
//#define SE_Unknown38					38	// not used
//#define SE_Unknown39					39	// not used
#define SE_DivineAura					40	// implemented
#define SE_Destroy						41	// implemented - Disintegrate, Banishment of Shadows
#define SE_ShadowStep					42	// implemented
//#define SE_Unknown43					43	// not used
#define SE_Lycanthropy					44	// implemented
//#define SE_Unknown45					45	// not used
#define SE_ResistFire					46	// implemented
#define SE_ResistCold					47	// implemented
#define SE_ResistPoison					48	// implemented
#define SE_ResistDisease				49	// implemented
#define SE_ResistMagic					50	// implemented
//#define SE_Unknown51					51	// not used
#define SE_SenseDead					52	// implemented
#define SE_SenseSummoned				53	// implemented
#define SE_SenseAnimals					54	// implemented
#define SE_Rune							55	// implemented
#define SE_TrueNorth					56	// implemented
#define SE_Levitate						57	// implemented
#define SE_Illusion						58	// implemented
#define SE_DamageShield					59	// implemented
//#define SE_Unknown60					60	// not used
#define SE_Identify						61	// implemented
//#define SE_Unknown62					62	// not used
#define SE_WipeHateList					63	// implemented
#define SE_SpinTarget					64	// implemented
#define SE_InfraVision					65	// implemented
#define SE_UltraVision					66	// implemented
#define SE_EyeOfZomm					67	// implemented
#define SE_ReclaimPet					68	// implemented
#define SE_TotalHP						69	// implemented
//#define SE_Unknown70					70	// not used
#define SE_NecPet						71	// implemented
//#define SE_Unknown72					72	// not used
#define SE_BindSight					73	// implemented
#define SE_FeignDeath					74	// implemented
#define SE_VoiceGraft					75	// *not implemented
#define SE_Sentinel						76	// *not implemented?(just seems to send a message)
#define SE_LocateCorpse					77	// implemented
#define SE_AbsorbMagicAtt				78	// implemented - Rune for spells
#define SE_CurrentHPOnce				79	// implemented - Heals and nukes, non-repeating if in a buff
//#define SE_Unknown80					80	// not used
#define SE_Revive						81	// implemented - Resurrect
#define SE_SummonPC						82	// implemented
#define SE_Teleport						83	// implemented
#define SE_TossUp						84	// implemented - Gravity Flux
#define SE_WeaponProc					85	// implemented - i.e. Call of Fire
#define SE_Harmony						86	// implemented
#define SE_MagnifyVision				87	// implemented - Telescope
#define SE_Succor						88	// implemented - Evacuate/Succor lines
#define SE_ModelSize					89	// implemented - Shrink, Growth
#define SE_Cloak						90	// *not implemented - Used in only 2 spells
#define SE_SummonCorpse					91	// implemented
#define SE_Calm							92	// implemented - Hate modifier stuff(poorly named)
#define SE_StopRain						93	// implemented - Wake of Karana
#define SE_NegateIfCombat				94	// *not implemented? - Works client side but there is comment todo in spell effects...Component of Spirit of Scale
#define SE_Sacrifice					95	// implemented
#define SE_Silence						96	// implemented
#define SE_ManaPool						97	// implemented
#define SE_AttackSpeed2					98	// implemented - Melody of Ervaj
#define SE_Root							99	// implemented
#define SE_HealOverTime					100	// implemented
#define SE_CompleteHeal					101	// implemented
#define SE_Fearless						102	// implemented - Valiant Companion
#define SE_CallPet						103	// implemented - Summon Companion
#define SE_Translocate					104	// implemented
#define SE_AntiGate						105	// implemented - Translocational Anchor
#define SE_SummonBSTPet					106	// implemented 
//#define SE_Unknown107					107	// not used
#define SE_Familiar						108	// implemented 
#define SE_SummonItemIntoBag			109	// implemented - summons stuff into container
//#define SE_Unknown110					110	//not used
#define SE_ResistAll					111	// implemented 
#define SE_CastingLevel					112	// implemented 
#define	SE_SummonHorse					113	// implemented 
#define SE_ChangeAggro					114	// implemented - Hate modifing buffs(ie horrifying visage)
#define SE_Hunger						115	// implemented - Song of Sustenance
#define SE_CurseCounter					116	// implemented 
#define SE_MagicWeapon					117	// implemented - makes weapon magical
#define SE_SingingSkill					118	// *implemented - needs AA conversion
#define SE_AttackSpeed3					119	// implemented
#define SE_HealRate						120	// implemented - reduces healing by a %
#define SE_ReverseDS					121 // implemented
//#define SE_Unknown122					122	// not used
#define SE_Screech						123	// implemented? Spell Blocker(can only have one buff with this effect at one time)
#define SE_ImprovedDamage				124 // implemented
#define SE_ImprovedHeal					125 // implemented
#define SE_SpellResistReduction			126 // implemented
#define SE_IncreaseSpellHaste			127 // implemented
#define SE_IncreaseSpellDuration		128 // implemented
#define SE_IncreaseRange				129 // implemented
#define SE_SpellHateMod					130 // implemented
#define SE_ReduceReagentCost			131 // implemented
#define SE_ReduceManaCost				132 // implemented
//#define SE_Unknown133					133	// not used
#define SE_LimitMaxLevel				134 // implemented
#define SE_LimitResist					135 // implemented
#define SE_LimitTarget					136 // implemented
#define SE_LimitEffect					137 // implemented
#define SE_LimitSpellType				138 // implemented
#define SE_LimitSpell					139 // implemented
#define SE_LimitMinDur					140 // implemented
#define SE_LimitInstant					141 // implemented
#define SE_LimitMinLevel				142 // implemented
#define SE_LimitCastTime				143 // implemented
//#define SE_Unknown144					144	// not used
#define SE_Teleport2					145	// implemented - Banishment of the Pantheon
//#define SE_Unknown146					146	// not used
#define SE_PercentalHeal				147 // implemented
#define SE_StackingCommand_Block		148 // implemented?
#define SE_StackingCommand_Overwrite 	149 // implemented?
#define SE_DeathSave					150 // implemented
#define SE_SuspendPet					151	// *not implemented as bonus
#define SE_TemporaryPets				152	// implemented
#define SE_BalanceHP					153 // implemented
#define SE_DispelDetrimental			154 // implemented
#define SE_SpellCritDmgIncrease			155 // implemented
#define SE_IllusionCopy					156	// implemented - Deception
#define SE_SpellDamageShield			157	// implemented - Petrad's Protection
#define SE_Reflect						158 // implemented
#define SE_AllStats						159	// implemented
#define SE_MakeDrunk					160 // implemented - poorly though, should check against tolerance
#define SE_MitigateSpellDamage			161	// implemented - rune with max value
#define SE_MitigateMeleeDamage			162	// implemented - rune with max value
#define SE_NegateAttacks				163	// implemented
#define SE_AppraiseLDonChest			164	// implemented
#define SE_DisarmLDoNTrap				165	// implemented
#define SE_UnlockLDoNChest				166	// implemented
#define SE_PetPowerIncrease				167 // implemented
#define SE_MeleeMitigation				168	// implemented
#define SE_CriticalHitChance			169	// implemented
#define SE_SpellCritChance				170	// implemented
#define SE_CrippBlowChance				171	// implemented
#define SE_AvoidMeleeChance				172	// implemented
#define SE_RiposteChance				173	// implemented
#define SE_DodgeChance					174	// implemented
#define SE_ParryChance					175	// implemented
#define SE_DualWieldChance				176	// implemented
#define SE_DoubleAttackChance			177	// implemented
#define SE_MeleeLifetap					178	// implemented
#define SE_AllInstrumentMod				179	// implemented
#define SE_ResistSpellChance			180	// implemented
#define SE_ResistFearChance				181	// implemented
#define SE_HundredHands					182	// implemented
#define SE_MeleeSkillCheck				183	// implemented
#define SE_HitChance					184	// implemented
#define SE_DamageModifier				185	// implemented
#define SE_MinDamageModifier			186	// implemented
//#define SE_Unknown187					187	// not used
#define SE_IncreaseBlockChance			188	// implemented
#define SE_CurrentEndurance				189	// implemented
#define SE_EndurancePool				190	// implemented
#define SE_Amnesia						191	// implemented - Silence vs Melee Effect
#define SE_Hate2						192	// implemented
#define SE_SkillAttack					193	// implemented
#define SE_FadingMemories				194	// implemented
#define SE_StunResist					195	// implemented
#define SE_Strikethrough				196	// implemented
#define SE_SkillDamageTaken				197	// implemented
#define SE_CurrentEnduranceOnce			198	// implemented
#define SE_Taunt						199	// *not implemented - % chance to taunt the target
#define SE_ProcChance					200	// implemented
#define SE_RangedProc					201	// implemented
#define SE_IllusionOther				202	// *not implemented as bonus(Project Illusion)
#define SE_MassGroupBuff				203	// *not implemented as bonus
#define SE_GroupFearImmunity			204	// *not implemented as bonus
#define SE_Rampage						205	// implemented
#define SE_AETaunt						206	// implemented
#define SE_FleshToBone					207	// implemented
//#define SE_Unknown208					208	// not used
#define SE_DispelBeneficial				209 // implemented
#define SE_PetShield					210	// *not implemented
#define SE_AEMelee						211	// implemented
#define SE_ProlongedDestruction			212	// *not implemented
//#define SE_Unknown213					213	// not used
#define SE_MaxHPChange 					214	// implemented
//#define SE_Unknown215					215	// not used
#define SE_Accuracy						216	// implemented
//#define SE_Unknown217					217	// not used
#define SE_PetCriticalHit				218 // not implemented as bonus
#define SE_SlayUndead					219	// not implemented as bonus
#define SE_SkillDamageAmount			220	// implemented
#define SE_Packrat						221 // not implemented as bonus
//#define SE_Unknown222					222	// not used
//#define SE_Unknown223					223	// not used
#define	SE_GiveDoubleRiposte			224 // not implemented as bonus
#define SE_GiveDoubleAttack				225	// not implemented as bonus
#define SE_TwoHandBash					226 // not implemented as bonus
#define SE_ReduceSkillTimer				227	// not implemented
//#define SE_Unknown228					228	// not used
#define SE_PersistantCasting			229 // implemented
//#define SE_Unknown230					230	// not used
//#define SE_Unknown231					231	// not used
#define SE_DivineSave					232	// not implemented (base == % chance on death to insta-res)
//#define SE_Unknown233					233	// not used
//#define SE_Unknown234					234	// not used
#define	SE_ChannelingChance				235 // not implemented
//#define SE_Unknown236					236	// not used
#define SE_GivePetGroupTarget			237 // not implemented as bonus(Pet Affinity)
//#define SE_Unknown238					238	// not used
//#define SE_Unknown239					239	// not used
//#define SE_Unknown240					240	// not used
//#define SE_Unknown241					241	// not used
//#define SE_Unknown242					242	// not used
//#define SE_Unknown243					243	// not used
//#define SE_Unknown244					244	// not used
//#define SE_Unknown245					245	// not used
#define SE_SetBreathLevel				246 // not implemented as bonus
//#define SE_Unknown247					247	// not used
#define SE_SecondaryForte				248 // not implemented as bonus(gives you a 2nd specialize skill that can go past 50 to 100)
#define SE_SecondaryDmgInc				249 // not implemented as bonus(Sinister Strikes)
//#define SE_Unknown250					250	// not used
//#define SE_Unknown251					251	// not used
//#define SE_Unknown252					252	// not used
//#define SE_Unknown253					253	// not used
#define SE_Blank						254 // implemented
//#define SE_Unknown255					255	// not used
//#define SE_Unknown256					256	// not used
#define SE_PetDiscipline				257 // not implemented as bonus - /pet hold
#define SE_TripleBackstab				258 // not implemented as bonus
#define SE_CombatStability				259 // not implemented as bonus
#define SE_AddInstrumentMod				260 // *not implemented
//#define SE_Unknown261					261	// not used
#define SE_RaiseStatCap					262 // implemented
#define	SE_TradeSkillMastery			263	// implemented - lets you raise more than one tradeskill above master.
#define SE_HastenedAASkill				264 // not implemented as bonus
#define SE_MasteryofPast				265 // not implemented as bonus
#define SE_ExtraAttackChance			266 // implemented
#define SE_PetDiscipline2				267 // *not implemented - /pet focus, /pet no cast
#define SE_ReduceTradeskillFail			268 // *not implemented? - reduces chance to fail with given tradeskill by a percent chance
//#define SE_Unknown269					269	// not used
//#define SE_Unknown270					270	// not used
#define SE_BaseMovementSpeed			271 // *not implemented - mods basemove speed, doesn't stack with other move mods, aa effect
#define SE_CastingLevel2				272 // *not implemented
#define SE_CriticalDoTChance			273	// implemented
#define SE_CriticalHealChance			274	// implemented
//#define SE_Unknown275					275	// not used
#define SE_Ambidexterity				276 // not implemented as bonus - aa effect
//#define SE_Unknown277					277	// not used
#define	SE_FinishingBlow				278 // not implemented as bonus - aa effect
#define SE_Flurry						279	// implemented
#define SE_PetFlurry					280 // not implemented as bonus
//#define SE_Unknown281					281	// not used
//#define SE_Unknown282					282	// not used
//#define SE_Unknown283					283	// not used
//#define SE_Unknown284					284	// not used
#define SE_NimbleEvasion				285	// *not implemented - base1 = 100 for max
#define SE_SpellDamage					286	// implemented - adds direct spell damage
#define SE_FocusCombatDurationMod		287 // *not implemented
//#define SE_Unknown288					288	// not used
#define SE_ImprovedSpellEffect			289 // implemented 
//#define SE_Unknown290					290	// not used
#define SE_Purify						291 // not implemented
//#define SE_Unknown292					292	// not used
//#define SE_Unknown293					293	// not used
#define SE_CriticalSpellChance			294 // implemented
//#define SE_Unknown295					295	// not used
#define SE_SpellVulnerability			296	// implemented - increase in incoming spell damage
#define SE_Empathy						297 // *not implemented - some kind of damage focus effect, maybe defensive?
#define SE_ChangeHeight					298	// implemented
#define SE_WakeTheDead					299	// implemented
#define SE_Doppelganger					300	// implemented
//#define SE_Unknown301					301	// not used
//#define SE_Unknown302					302	// not used
#define SE_FF_Damage_Amount				303	// *not implemented - focus effect for specific spells
#define SE_OffhandRiposteFail			304 // not implemented as bonus - enemy cannot riposte offhand attacks
#define SE_MitigateDamageShield			305 // implemented
#define SE_ArmyOfTheDead				306 // *not implemented
#define SE_Appraisal					307 // *not implemented
#define SE_SuspendMinion				308 // not implemented as bonus
#define SE_YetAnotherGate				309 // implemented
#define SE_ReduceReuseTimer				310 // *not implemented
#define SE_NoCombatSkills				311 // implemented
#define SE_Sanctuary					312 // *not implemented
//#define SE_Unknown313					313	// not used
#define SE_Invisibility2				314 // *not implemented correctly, this is supposed to be a higher level invis than reg invis
#define SE_InvisVsUndead2				315 // *not implemented correctly, this is supposed to be a higher level invis than reg invis
//#define SE_Unknown316					316	// not used
//#define SE_Unknown317					317	// not used
#define SE_ItemManaRegenCapIncrease		318 // implemented - increases amount of mana regen you can gain via items
#define SE_CriticalHealOverTime			319 // implemented
//#define SE_Unknown320					320	// not used
#define SE_ReduceHate					321 // *not implemented
#define SE_GateToHomeCity				322 // implemented
#define SE_DefensiveProc				323 // implemented
#define SE_HPToMana						324 // implemented
//#define SE_Unknown325					325	// not used
#define SE_SpellSlotIncrease			326 // not implemented as bonus - increases your spell slot availability
#define SE_MysticalAttune				327 // implemented - increases amount of buffs that a player can have
#define SE_DelayDeath					328 // not implemented as bonus - increases how far you can fall below 0 hp before you die
#define SE_ManaAbsorbPercentDamage		329 // implemented
#define SE_CriticalDamageMob			330	// implemented
#define SE_Salvage						331 // *not implemented - chance to recover items that would be destroyed in failed tradeskill combine
#define SE_SummonToCorpse				332 // *not implemented
#define SE_EffectOnFade					333 // implemented
#define SE_BardAEDot					334	// implemented
#define SE_BlockNextSpellFocus			335	// *not implemented - base1 chance to block next spell ie Puratus (8494)
//#define SE_Unknown336					336	// not used
#define SE_PercentXPIncrease			337	// implemented
#define SE_SummonAndResAllCorpses		338	// implemented
#define SE_TriggerOnCast				339	// implemented
#define SE_SpellTrigger     			340	// implemented - chance to trigger spell
//#define SE_Unknown341					341	// not used
#define SE_ImmuneFleeing				342	// *not implemented
#define SE_InterruptCasting				343	// implemented - % chance to interrupt spells being cast every tic. Cacophony (8272)
//#define SE_Unknown344					344	// not used
//#define SE_Unknown345					345	// not used
//#define SE_Unknown346					346	// not used
//#define SE_Unknown347					347	// not used
#define SE_LimitManaCost				348	// implemented
//#define SE_Unknown349					349	// not used
#define SE_ManaBurn						350	// *not implemented
#define SE_PersistentEffect				351	// *not implemented. creates a trap/totem that casts a spell (spell id + base1?) when anything comes near it. can probably make a beacon for this
#define SE_Unknown352					352	// *not implemented - looks to be some type of invulnerability? Test ITC (8755)
#define SE_Unknown353					353	// *not implemented - allows use of more than 1 aura, aa effect
#define SE_Unknown354					354	// *not implemented - looks to be some type of invulnerability? Test DAT (8757)
#define SE_Unknown355					355	// *not implemented - looks to be some type of invulnerability? Test LT (8758)
//#define SE_Unknown356					356	// not used
//#define SE_Unknown357					357	// not used
#define SE_CurrentManaOnce				358	// implemented
//#define SE_Unknown359					359	// not used
#define SE_SpellOnKill					360	// implemented - has a base1 % to cast spell base2 when you kill a "challenging foe" (>= max?)
#define SE_SpellOnDeath2				361	// *not implemented - same as spellondeath
//#define SE_Unknown362					362	// not used
//#define SE_Unknown363					363	// not used
//#define SE_Unknown364					364	// not used
#define SE_SpellOnDeath					365	// *not implemented - casts base2 spell on the originator of the spell base1 % of the time when the person it was cast on dies. have to be >= max (level) for it to work?
//#define SE_Unknown366					366	// not used - corr test(9429) only spell
#define SE_AddBodyType					367	// *not implemented - adds body type of base1 so it can be affected by spells that are limited to that type (Plant, Animal, Undead, etc)
#define SE_FactionMod					368	// *not implemented - increases faction with base1 (faction id, live won't match up w/ ours) by base2
#define SE_CorruptionCounter			369	// implemented
#define SE_ResistCorruption				370	// implemented
#define SE_InhibitMeleeAttacks			371 // *not implemented - some type of melee slow
//#define SE_Unknown372					372	// not used
#define SE_CastOnWearoff				373 // implemented
#define SE_ApplyEffect					374 // implemented 
//#define SE_Unknown375					375	// not used
//#define SE_Unknown376					376	// *not implemented - used in 2 spells
#define SE_BossSpellTrigger				377	// implemented - spell is cast on fade
//#define SE_Unknown372					378	// not used
#define SE_ShadowStepDirectional		379 // *not implemented - Shadowstep in a certain direction
#define SE_Knockdown					380 // *not implemented - small knock back + stun or feign?
//#define SE_Unknown381					381	// not used
#define SE_BlockDS						382 // *not implemented - something to do with blocking a % of certain ds?
#define SE_SympatheticProc				383 // implemented - focus on items that has chance to proc a spell when you cast
#define SE_Leap							384	// *not implemented - Leap effect, ie stomping leap
#define SE_LimitSpellGroup				385	// implemented - Limits to spell group(ie type 3 reuse reduction augs that are class specific and thus all share s SG)
#define SE_CastOnCure					387 // *not implemented - Casts a spell when you are cured
#define SE_Forceful_Rejuv				389 // *not implemented - Refresh spell icons
#define SE_HealingFocus					392 // *not implemented - Adds or removes healing from spells
#define SE_HealRate2					393 // not implemented - Appears to be exactly the same as HealRate except with focus restrictions
#define SE_AdditionalHeal				396 // not implemented - Adds/removes healing from specific spells(restricted to spell groups)
#define SE_SwarmPetDuration				398 // *not implemented - Affects the duration of swarm pets
#define SE_Twincast						399 // implemented - cast 2 spells for every 1
#define SE_HealFromMana					400 // *not implemented - Drains mana and heals for each point of mana drained
#define SE_ManaDrainWithDmg				401 // *not implemented - Deals damage based on the amount of mana drained
#define SE_EndDrainWithDmg				402 // *not implemented - Deals damage for the amount of endurance drained
#define SE_Twinproc						405 // implemented - Proc twice
#define SE_SetMaxHP						408 // *not implemented - Sets your max hp to a percent of your original
#define SE_SetMaxMana					409 // *not implemented - Sets your max mana to a percent of your original
#define SE_SetMaxEnd					410 // *not implemented - Sets your max end to a percent of your original
#define SE_LimitClass					411 // *not implemented - Limits to spells of a certain class
#define SE_IncreaseSpellPower			413 // *not implemented - Increases the power of spells(bard only?)
#define SE_LimitSpellSkill				414 // *not implemented - Limits to a skill of spells(ie wind, evocation)
#define SE_AddMeleeProc					419 // *not implemented - Adds a proc
#define SE_GravityEffect				424 // implemented - Pulls/pushes you toward/away the mob at a set pace
#define SE_LimitToSkill					428 // *not implemented - Only fires when a particular skills is used(ie dodge)	
// LAST


#define DF_Permanent		50

// solar: note this struct is historical, we don't actually need it to be
// aligned to anything, but for maintaining it it is kept in the order that
// the fields in the text file are.  the numbering is not offset, but field
// number.  note that the id field is counted as 0, this way the numbers
// here match the numbers given to sep in the loading function net.cpp
//
struct SPDat_Spell_Struct
{
/* 000 */	int			id;	// not used
/* 001 */	char		name[64]; // Name of the spell
/* 002 */	char		player_1[32]; // "PLAYER_1"
/* 003 */	char		teleport_zone[64];	// Teleport zone, pet name summoned, or item summoned
/* 004 */	char		you_cast[64]; // Message when you cast
/* 005 */	char		other_casts[64]; // Message when other casts
/* 006 */	char		cast_on_you[64]; // Message when spell is cast on you 
/* 007 */	char		cast_on_other[64]; // Message when spell is cast on someone else
/* 008 */	char		spell_fades[64]; // Spell fades
/* 009 */	float		range;
/* 010 */	float		aoerange;
/* 011 */	float		pushback;
/* 012 */	float		pushup;
/* 013 */	int32		cast_time; // Cast time
/* 014 */	int32		recovery_time; // Recovery time
/* 015 */	int32		recast_time; // Recast same spell time
/* 016 */	int32		buffdurationformula;
/* 017 */	int32		buffduration;
/* 018 */	int32		AEDuration;	// sentinel, rain of something
/* 019 */	int16		mana; // Mana Used
/* 020 */	sint32		base[EFFECT_COUNT];	//various purposes
/* 032 */	int			base2[EFFECT_COUNT]; //various purposes
/* 044 */	sint16		max[EFFECT_COUNT];
/* 056 */	int16		icon; // Spell icon
/* 057 */	int16		memicon; // Icon on membarthing
/* 058 */	sint32		components[4]; // reagents
/* 062 */	int			component_counts[4]; // amount of regents used
/* 066 */	signed		NoexpendReagent[4];	// focus items (Need but not used; Flame Lick has a Fire Beetle Eye focus.)
											// If it is a number between 1-4 it means components[number] is a focus and not to expend it
											// If it is a valid itemid it means this item is a focus as well
/* 070 */	int16		formula[EFFECT_COUNT]; // Spell's value formula
/* 082 */	int			LightType; // probaly another effecttype flag
/* 083 */	int			goodEffect; //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
/* 084 */	int			Activated; // probaly another effecttype flag	
/* 085 */	int			resisttype;
/* 086 */	int			effectid[EFFECT_COUNT];	// Spell's effects
/* 098 */	SpellTargetType	targettype;	// Spell's Target
/* 099 */	int			basediff; // base difficulty fizzle adjustment
/* 100 */	SkillType	skill;
/* 101 */	sint16		zonetype;	// 01=Outdoors, 02=dungeons, ff=Any 
/* 102 */	int16		EnvironmentType;
/* 103 */	int			TimeOfDay;
/* 104 */	int8		classes[PLAYER_CLASS_COUNT]; // Classes, and their min levels
/* 120 */	int8		CastingAnim;
/* 121 */	int8		TargetAnim;
/* 122 */	int32		TravelType;
/* 123 */	int16		SpellAffectIndex;
/* 124 */   int         disallow_sit;	// 124: high-end Yaulp spells (V, VI, VII, VIII [Rk 1, 2, & 3], & Gallenite's Bark of Fury
/* 125 */   int         spacing125;   // 125: Words of the Skeptic
/* 126 */	sint8		deities[16];	// Deity check. 201 - 216 per http://www.eqemulator.net/wiki/wikka.php?wakka=DeityList
										// -1: Restrict to Deity; 1: Restrict to Deity, but only used on non-Live (Test Server "Blessing of ...") spells; 0: Don't restrict
/* 142 */ int spacing142[2];	// 142: between 0 & 100
								// 143: always set to 0
/* 144 */	sint16		new_icon;	// Spell icon used by the client in uifiles/default/spells??.tga, both for spell gems & buff window. Looks to depreciate icon & memicon
/* 145 */	sint16		spellanim;	// Doesn't look like it's the same as #doanim, so not sure what this is
/* 146 */	sint8		uninterruptable;	// Looks like anything != 0 is uninterruptable. Values are mostly -1, 0, & 1 (Fetid Breath = 90?)
/* 147 */	sint16		ResistDiff; 
/* 148 */   int			dot_stacking_exempt;
/* 149 */   int			deletable;
/* 150 */	int16		RecourseLink;
/* 151 */ int spacing151[3];	// 151: -1, 0, or 1
								// 152 & 153: all set to 0
/* 154 */	sint8		short_buff_box;	// != 0, goes to short buff box. Not really supported in the server code
/* 155 */   int			descnum; // eqstr of description of spell
/* 156 */   int			typedescnum; // eqstr of type description
/* 157 */   int			effectdescnum; // eqstr of effect description
/* 158 */ int spacing158[4];
/* 162 */   int			bonushate;
/* 163 */ int spacing163[3];
/* 166 */   int			EndurCost;
/* 167 */   int			EndurTimerIndex;
/* 168 */ int spacing168[5];
/* 173 */   int			HateAdded;
/* 174 */   int			EndurUpkeep;
/* 175 */ int spacing175;
/* 176 */ int numhits;
/* 177 */ int pvpresistbase;
/* 178 */ int pvpresistcalc;
/* 179 */ int pvpresistcap;
/* 180 */ int spell_category;
/* 181 */ int spacing181[4];
/* 185 */ int can_mgb;	// 0=no, -1 or 1 = yes
/* 186 */ int dispel_flag;
/* 189 */ int MinResist;
/* 190 */ int MaxResist;
/* 192 */ int NimbusEffect;
/* 195 */ float directional_start;
/* 196 */ float directional_end;
/* 207 */ int spellgroup;
/* 209 */ int field209; // Need more investigation to figure out what to call this, for now we know -1 makes charm spells not break before their duration is complete, it does alot more though
/* 212 */ bool AllowRest;


//shared memory errors
/* 186 */	/*sint8		nodispell;*/	// 0=can be dispelled, -1=can't be dispelled at all, 1=most can be cancelled w/ a cure but not dispelled
/* 187 */	/*uint8		npc_category;*/	// 0=not used, 1=AoE Detrimental, 2=DD, 3=Buffs, 4=Pets, 5=Healing, 6=Gate, 7=Debuff, 8=Dispell
/* 188 */	/*uint32		npc_usefulness;*/	// higher number = more useful, lower number = less useful
/* 189 */ /*int spacing189[18];*/
/* 207 */	/*uint32		spellgroup;*/
/* 208 */ /*int spacing208[7];*/
// Might be newer fields in the live version, which is what some of the last fields are


			int8		DamageShieldType; // This field does not exist in spells_us.txt
};

#if defined(NEW_LoadSPDat) || defined(DB_LoadSPDat)
	extern const SPDat_Spell_Struct* spells; 
	extern sint32 SPDAT_RECORDS;
#else
	#define SPDAT_RECORDS	3602
#endif

bool IsTargetableAESpell(int16 spell_id);
bool IsSacrificeSpell(int16 spell_id);
bool IsLifetapSpell(int16 spell_id);
bool IsMezSpell(int16 spell_id);
bool IsStunSpell(int16 spell_id);
bool IsSlowSpell(int16 spell_id);
bool IsHasteSpell(int16 spell_id);
bool IsHarmonySpell(int16 spell_id);
bool IsPercentalHealSpell(int16 spell_id);
bool IsGroupOnlySpell(int16 spell_id);
bool IsBeneficialSpell(int16 spell_id);
bool IsDetrimentalSpell(int16 spell_id);
bool IsInvulnerabilitySpell(int16 spell_id);
bool IsCHDurationSpell(int16 spell_id);
bool IsPoisonCounterSpell(int16 spell_id);
bool IsDiseaseCounterSpell(int16 spell_id);
bool IsSummonItemSpell(int16 spell_id);
bool IsSummonSkeletonSpell(int16 spell_id);
bool IsSummonPetSpell(int16 spell_id);
bool IsSummonPCSpell(int16 spell_id);
bool IsCharmSpell(int16 spell_id);
bool IsBlindSpell(int16 spell_id);
bool IsEffectHitpointsSpell(int16 spell_id);
bool IsReduceCastTimeSpell(int16 spell_id);
bool IsIncreaseDurationSpell(int16 spell_id);
bool IsReduceManaSpell(int16 spell_id);
bool IsExtRangeSpell(int16 spell_id);
bool IsImprovedHealingSpell(int16 spell_id);
bool IsImprovedDamageSpell(int16 spell_id);
bool IsAEDurationSpell(int16 spell_id);
bool IsPureNukeSpell(int16 spell_id);
bool IsPartialCapableSpell(int16 spell_id);
bool IsResistableSpell(int16 spell_id);
bool IsGroupSpell(int16 spell_id);
bool IsTGBCompatibleSpell(int16 spell_id);
bool IsBardSong(int16 spell_id);
bool IsEffectInSpell(int16 spellid, int effect);
bool IsBlankSpellEffect(int16 spellid, int effect_index);
bool IsValidSpell(int32 spellid);
bool IsSummonSpell(int16 spellid);
bool IsEvacSpell(int16 spellid);
bool IsDamageSpell(int16 spellid);
bool IsFearSpell(int16 spellid);
bool BeneficialSpell(int16 spell_id);
bool GroupOnlySpell(int16 spell_id);
int GetSpellEffectIndex(int16 spell_id, int effect);
int CanUseSpell(int16 spellid, int classa, int level);
int GetMinLevel(int16 spell_id);
int CalcBuffDuration_formula(int level, int formula, int duration);
sint32 CalculatePoisonCounters(int16 spell_id);
sint32 CalculateDiseaseCounters(int16 spell_id);
sint32 CalculateCurseCounters(int16 spell_id);
sint32 CalculateCorruptionCounters(int16 spell_id);
bool IsDisciplineBuff(int16 spell_id);
bool IsDiscipline(int16 spell_id);
bool IsResurrectionEffects(int16 spell_id);
bool IsRuneSpell(int16 spell_id);
bool IsMagicRuneSpell(int16 spell_id);
bool IsManaTapSpell(int16 spell_id);
bool IsAllianceSpellLine(int16 spell_id);
bool IsDeathSaveSpell(int16 spell_id);
bool IsFullDeathSaveSpell(int16 spell_id);
bool IsPartialDeathSaveSpell(int16 spell_id);
bool IsShadowStepSpell(int16 spell_id);
bool IsSuccorSpell(int16 spell_id);
bool IsTeleportSpell(int16 spell_id);
bool IsGateSpell(int16 spell_id);
bool IsPlayerIllusionSpell(int16 spell_id); // seveian 2008-09-23
bool IsLDoNObjectSpell(int16 spell_id);
sint32 GetSpellResistType(int16 spell_id);
sint32 GetSpellTargetType(int16 spell_id);
bool IsHealOverTimeSpell(int16 spell_id);
bool IsCompleteHealSpell(int16 spell_id);
bool IsFastHealSpell(int16 spell_id);
bool IsRegularSingleTargetHealSpell(int16 spell_id);
uint32 GetMorphTrigger(uint32 spell_id);
uint32 GetPartialMeleeRuneReduction(uint32 spell_id);
uint32 GetPartialMagicRuneReduction(uint32 spell_id);
uint32 GetPartialMeleeRuneAmount(uint32 spell_id);
uint32 GetPartialMagicRuneAmount(uint32 spell_id);

int CalcPetHp(int levelb, int classb, int STA = 75);
const char *GetRandPetName();
int GetSpellEffectDescNum(int16 spell_id);
DmgShieldType GetDamageShieldType(int16 spell_id);
bool DetrimentalSpellAllowsRest(int16 spell_id);
uint32 GetNimbusEffect(int16 spell_id);
sint32 GetFuriousBash(int16 spell_id);

#endif
