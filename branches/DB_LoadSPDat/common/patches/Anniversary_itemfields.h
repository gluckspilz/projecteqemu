/*


These fields must be in the order of how they are serialized!



*/

#ifdef NEW_TRY
/* 000 */	//I(ItemClass) Leave this one off on purpose
/* 001 */	S(Name)
/* 002 */	S(Lore)
/* 003 */	MISSINGCS("")	//LoreFile? missing from binary
/* 003 */	S(IDFile)
/* 004 */	I4(ID)
/* 005 */	I1(Weight)
/* 006 */	I1(NoRent)
/* 007 */	I2(NoDrop)
/* 008 */	I4(Size)
/* 009 */	I1(Slots)
/* 010 */	I4(Price)
/* 011 */	I4(Icon)
/* 013 */	C4(0)
/* 014 */	C1(0)
/* 014 */	I1(BenefitFlag)
/* 015 */	I1(Tradeskills)
/* 016 */	I1(CR)
/* 017 */	I1(DR)
/* 018 */	I1(PR)
/* 019 */	I1(MR)
/* 020 */	I1(FR)
			C1(0)	//svcorruption
/* 021 */	I1(AStr)
/* 022 */	I1(ASta)
/* 023 */	I1(AAgi)
/* 024 */	I1(ADex)
/* 025 */	I1(ACha)
/* 026 */	I1(AInt)
/* 027 */	I1(AWis)
/* 028 */	I4(HP)
/* 029 */	I4(Mana)
			I4(Endur)
/* 030 */	I4(AC)	//?
/* 114 */	I4(Regen)	//moved here, this is right
/* 115 */	I4(ManaRegen)	//moved here, guess
/* 116 */	I4(EnduranceRegen)	//moved here, guess
/* 117 */	I4(Haste)	//moved here, guess
/* 052 */	I4(Classes)
/* 053 */	I4(Races)
/* 031 */	I4(Deity)
/* 032 */	I4(SkillModValue)
/* 033 */	C4(0)
/* 034 */	I4(SkillModType)
/* 035 */	I4(BaneDmgRace)
/* 037 */	I4(BaneDmgBody)
/* 036 */	I4(BaneDmgRaceAmt)
/* 036 */	I4(BaneDmgAmt)
/* 038 */	I1(Magic)
/* 039 */	I4(CastTime_)
/* 040 */	I4(ReqLevel)	//?
/* 045 */	I4(RecLevel)	//?
/* 046 */	I4(RecSkill)	//?
/* 041 */	I4(BardType)	//?
/* 042 */	I4(BardValue)	//?
/* 043 */	I1(Light)		//?
/* 044 */	I4(Delay)
/* 047 */	I1(ElemDmgType)
/* 048 */	I1(ElemDmgAmt)
/* 049 */	I1(Range)
/* 050 */	I1(Damage)
/* 051 */	I4(Color)
/* 056 */	I1(ItemType)
/* 057 */	I4(Material)
/* 060 */	C4("0")
/* 061 */	C4(1065353216)	//no idea what it means
/* 058 */	F(SellRate)
/* 063 */	I4(CombatEffects)
/* 064 */	I4(Shielding)
/* 065 */	I4(StunResist)
/* 066 */	I4(StrikeThrough)
/* 067 */	I4(ExtraDmgSkill)
/* 068 */	I4(ExtraDmgAmt)
/* 069 */	I4(SpellShield)
/* 070 */	I4(Avoidance)
/* 071 */	I4(Accuracy)
/* 072 */	I4(CharmFileID)
/* 073 */	I4(FactionMod1)
/* 077 */	I4(FactionAmt1)
/* 074 */	I4(FactionMod2)
/* 078 */	I4(FactionAmt2)
/* 075 */	I4(FactionMod3)
/* 079 */	I4(FactionAmt3)
/* 076 */	I4(FactionMod4)
/* 080 */	I4(FactionAmt4)
/* 081 */	S(CharmFile)
//something isnt right with size in here, the next 4 should be 12 bytes
/* 082 */	I4(AugType)		//?
/* 082 */	I4(AugRestrict)		//?
/* 082 */	I4(AugDistiller)		//?
/* 083 */	I4(AugSlotType[0])
/* 084 */	I1(AugSlotUnk[0])
/* 084 */	I1(AugSlotUnk2[0])
/* 085 */	I4(AugSlotType[1])
/* 086 */	I1(AugSlotUnk[1])
/* 086 */	I1(AugSlotUnk2[1])
/* 087 */	I4(AugSlotType[2])
/* 088 */	I1(AugSlotUnk[2])
/* 088 */	I1(AugSlotUnk2[2])
/* 089 */	I4(AugSlotType[3])
/* 090 */	I1(AugSlotUnk[3])
/* 090 */	I1(AugSlotUnk2[3])
/* 091 */	I4(AugSlotType[4])
/* 092 */	I1(AugSlotUnk[4])
/* 092 */	I1(AugSlotUnk2[4])
/* 093 */	I4(PointType)
/* 093 */	I4(LDoNTheme)
/* 094 */	I4(LDoNPrice)
/* 094 */	C4(70)	//no idea
/* 095 */	I4(LDoNSold)
/* 096 */	I1(BagType)
/* 097 */	I1(BagSlots)
/* 098 */	I1(BagSize)
/* 099 */	I1(BagWR)

/* 100 */	I1(Book)
/* 101 */	I1(BookType)
/* 102 */	S(Filename)
/* 105 */	I1(LoreGroup)
/* 106 */	//I(PendingLoreFlag)
/* 107 */	I1(ArtifactFlag)
/* 094 */	MISSINGC("0")	//not in binary version
/* 108 */	//I(SummonedFlag)
/* 109 */	I4(Favor)
/* 110 */	I1(FVNoDrop)
/* 112 */	I4(DotShielding)
/* 113 */	I4(Attack)
/*  */	UC4(0)	//not in the serialized version
/*  */	UC4(0)
/* 121 */	I4(GuildFavor)
/* 118 */	I(DamageShield)
/* 120 */	C("0")
/* 121 */	C("0")
/* 125 */	I1(Attuneable)
/* 126 */	I(NoPet)
/* 124 */	C("0")
/* 129 */	I(PotionBelt)
/* 130 */	I(PotionBeltSlots)
/* 131 */	I4(StackSize)
/* 132 */	I1(NoTransfer)
/* 129 */	MISSINGC("0")	//not in binary version
/* 132 */	I1(QuestItemFlag)
/* 131 */	MISSINGC("0")	//not in binary version
//this is all one big field in the serialized version:
	C4(0)
	C4(0)
	C4(0)
	C4(0)
	C1(0)
	C1(0)
	C1(0)

/* 134 */	I4(Click.Effect)
		UC1(0)	//not in the serialized version
/* 135 */	I1(Click.Type)
/* 136 */	I2(Click.Level2)
/* 137 */	I2(Click.Level)
/* 055 */	I4(MaxCharges)
/* 060 */	I4(CastTime)
/* 119 */	I4(RecastDelay)
/* 120 */	I4(RecastType)
/* 138 */	C4(0)	//clickunk5	(prolly ProcRate)
/* 138 */	CS("")	//clickunk6
/* 138 */	C4(-1)	//clickunk7
/* 139 */	I4(Proc.Effect)
		UC1(0)	//not in the serialized version
/* 140 */	I1(Proc.Type)
/* 141 */	I2(Proc.Level2)
/* 142 */	I2(Proc.Level)
/* 143 */	C4(0)	//procunk1	(prolly MaxCharges)
/* 143 */	C4(0)	//procunk2	(prolly CastTime)
/* 143 */	C4(0)	//procunk3	(prolly RecastDelay)
/* 143 */	C4(0)	//procunk4	(prolly RecastType)
/* 062 */	I4(ProcRate)
/* 143 */	CS("")	//procunk6
/* 143 */	C4(-1)	//procunk7
/* 144 */	I4(Worn.Effect)
		UC1(0)	//not in the serialized version
/* 145 */	I1(Worn.Type)
/* 146 */	I2(Worn.Level2)
/* 147 */	I2(Worn.Level)
/* 143 */	C4(0)	//wornunk1	(prolly MaxCharges)
/* 143 */	C4(0)	//wornunk2	(prolly CastTime)
/* 143 */	C4(0)	//wornunk3	(prolly RecastDelay)
/* 143 */	C4(0)	//wornunk4	(prolly RecastType)
/* 143 */	C4(0)	//wornunk5	(prolly ProcRate)
/* 143 */	CS("")	//wornunk6
/* 143 */	C4(-1)	//wornunk7
/* 149 */	I4(Focus.Effect)
		UC1(0)	//not in the serialized version
/* 150 */	I1(Focus.Type)
/* 151 */	I2(Focus.Level2)
/* 152 */	I2(Focus.Level)
/* 143 */	C4(0)	//focusunk1	(prolly MaxCharges)
/* 143 */	C4(0)	//focusunk2	(prolly CastTime)
/* 143 */	C4(0)	//focusunk3	(prolly RecastDelay)
/* 143 */	C4(0)	//focusunk4	(prolly RecastType)
/* 143 */	C4(0)	//focusunk5	(prolly ProcRate)
/* 143 */	CS("")	//focusunk6
/* 143 */	C4(-1)	//focusunk7
/* 154 */	I4(Scroll.Effect)
		UC1(0)	//not in the serialized version
/* 155 */	I1(Scroll.Type)
/* 156 */	I2(Scroll.Level2)
/* 157 */	I2(Scroll.Level)
/* 143 */	C4(0)	//scrollunk1	(prolly MaxCharges)
/* 143 */	C4(0)	//scrollunk2	(prolly CastTime)
/* 143 */	C4(0)	//scrollunk3	(prolly RecastDelay)
/* 143 */	C4(0)	//scrollunk4	(prolly RecastType)
/* 143 */	C4(0)	//scrollunk5	(prolly ProcRate)
/* 143 */	CS("")	//scrollunk6
/* 143 */	C4(-1)	//scrollunk7
/* 193 */	C4(0)	//Power Source Capacity 
/* 194 */	C4(0)	//purity

#else
/* 000 */	//I(ItemClass) Leave this one off on purpose
/* 001 */	S(Name)
/* 002 */	S(Lore)
/* 003 */	C("")	//LoreFile?
/* 003 */	S(IDFile)
/* 004 */	I(ID)
/* 005 */	I(Weight)
/* 006 */	I(NoRent)
/* 007 */	I(NoDrop)
/* 008 */	I(Size)
/* 009 */	I(Slots)
/* 010 */	I(Price)
/* 011 */	I(Icon)
/* 013 */	C("0")
/* 014 */	C("0")
/* 014 */	I(BenefitFlag)
/* 015 */	I(Tradeskills)
/* 016 */	I(CR)
/* 017 */	I(DR)
/* 018 */	I(PR)
/* 019 */	I(MR)
/* 020 */	I(FR)
			C("0")	//svcorruption
/* 021 */	I(AStr)
/* 022 */	I(ASta)
/* 023 */	I(AAgi)
/* 024 */	I(ADex)
/* 025 */	I(ACha)
/* 026 */	I(AInt)
/* 027 */	I(AWis)
/* 028 */	I(HP)
/* 029 */	I(Mana)
			I(Endur)
/* 030 */	I(AC)
/* 052 */	I(Classes)
/* 053 */	I(Races)
/* 031 */	I(Deity)
/* 032 */	I(SkillModValue)
/* 033 */	C("0")
/* 034 */	I(SkillModType)
/* 035 */	I(BaneDmgRace)
/* 037 */	I(BaneDmgBody)
/* 036 */	I(BaneDmgRaceAmt)
/* 036 */	I(BaneDmgAmt)
/* 038 */	I(Magic)
/* 039 */	I(CastTime_)
/* 040 */	I(ReqLevel)
/* 045 */	I(RecLevel)
/* 046 */	I(RecSkill)
/* 041 */	I(BardType)
/* 042 */	I(BardValue)
/* 043 */	I(Light)
/* 044 */	I(Delay)
/* 047 */	I(ElemDmgType)
/* 048 */	I(ElemDmgAmt)
/* 049 */	I(Range)
/* 050 */	I(Damage)
/* 051 */	I(Color)
/* 056 */	I(ItemType)
/* 057 */	I(Material)
/* 060 */	C("0")
/* 061 */	C("0")
/* 058 */	F(SellRate)
/* 063 */	I(CombatEffects)
/* 064 */	I(Shielding)
/* 065 */	I(StunResist)
/* 059 */	//C("0")
/* 061 */	//C("0")
/* 066 */	I(StrikeThrough)
/* 067 */	I(ExtraDmgSkill)
/* 068 */	I(ExtraDmgAmt)
/* 069 */	I(SpellShield)
/* 070 */	I(Avoidance)
/* 071 */	I(Accuracy)
/* 072 */	I(CharmFileID)
/* 073 */	I(FactionMod1)
/* 077 */	I(FactionAmt1)
/* 074 */	I(FactionMod2)
/* 078 */	I(FactionAmt2)
/* 075 */	I(FactionMod3)
/* 079 */	I(FactionAmt3)
/* 076 */	I(FactionMod4)
/* 080 */	I(FactionAmt4)
/* 081 */	S(CharmFile)
/* 082 */	I(AugType)
/* 082 */	I(AugRestrict)
/* 082 */	I(AugDistiller)
/* 083 */	I(AugSlotType[0])
/* 084 */	I(AugSlotUnk[0])
/* 084 */	I(AugSlotUnk2[0])
/* 085 */	I(AugSlotType[1])
/* 086 */	I(AugSlotUnk[1])
/* 086 */	I(AugSlotUnk2[1])
/* 087 */	I(AugSlotType[2])
/* 088 */	I(AugSlotUnk[2])
/* 088 */	I(AugSlotUnk2[2])
/* 089 */	I(AugSlotType[3])
/* 090 */	I(AugSlotUnk[3])
/* 090 */	I(AugSlotUnk2[3])
/* 091 */	I(AugSlotType[4])
/* 092 */	I(AugSlotUnk[4])
/* 092 */	I(AugSlotUnk2[4])
/* 093 */	I(PointType)
/* 093 */	I(LDoNTheme)
/* 094 */	I(LDoNPrice)
/* 094 */	C("0")
/* 095 */	I(LDoNSold)
/* 096 */	I(BagType)
/* 097 */	I(BagSlots)
/* 098 */	I(BagSize)
/* 099 */	I(BagWR)
/* 100 */	I(Book)
/* 101 */	I(BookType)
/* 102 */	S(Filename)
/* 105 */	I(LoreGroup)
/* 106 */	//I(PendingLoreFlag)
/* 107 */	I(ArtifactFlag)
/* 094 */	C("0")
/* 108 */	//I(SummonedFlag)
/* 109 */	I(Favor)
/* 121 */	I(GuildFavor)
/* 110 */	I(FVNoDrop)
/* 112 */	I(DotShielding)
/* 113 */	I(Attack)
/* 114 */	I(Regen)
/* 115 */	I(ManaRegen)
/* 116 */	I(EnduranceRegen)
/* 117 */	I(Haste)
/* 118 */	I(DamageShield)
/* 120 */	C("0")
/* 121 */	C("0")
/* 125 */	I(Attuneable)
/* 126 */	I(NoPet)
/* 124 */	C("0")
/* 129 */	I(PotionBelt)
/* 130 */	I(PotionBeltSlots)
/* 131 */	I(StackSize)
/* 132 */	I(NoTransfer)
/* 129 */	C("0")
/* 132 */	I(QuestItemFlag)
/* 131 */	C("0")
/* 132 */	C("00000000000000000000000000000000000000")
/* 134 */	I(Click.Effect)
/* 135 */	I(Click.Type)
/* 136 */	I(Click.Level2)
/* 137 */	I(Click.Level)
/* 055 */	I(MaxCharges)
/* 060 */	I(CastTime)
/* 119 */	I(RecastDelay)
/* 120 */	I(RecastType)
/* 138 */	C("0")	//clickunk5	(prolly ProcRate)
/* 138 */	C("")	//clickunk6
/* 138 */	C("-1")	//clickunk7
/* 139 */	I(Proc.Effect)
/* 140 */	I(Proc.Type)
/* 141 */	I(Proc.Level2)
/* 142 */	I(Proc.Level)
/* 143 */	C("0")	//procunk1	(prolly MaxCharges)
/* 143 */	C("0")	//procunk2	(prolly CastTime)
/* 143 */	C("0")	//procunk3	(prolly RecastDelay)
/* 143 */	C("0")	//procunk4	(prolly RecastType)
/* 062 */	I(ProcRate)
/* 143 */	C("")	//procunk6
/* 143 */	C("-1")	//procunk7
/* 144 */	I(Worn.Effect)
/* 145 */	I(Worn.Type)
/* 146 */	I(Worn.Level2)
/* 147 */	I(Worn.Level)
/* 143 */	C("0")	//wornunk1	(prolly MaxCharges)
/* 143 */	C("0")	//wornunk2	(prolly CastTime)
/* 143 */	C("0")	//wornunk3	(prolly RecastDelay)
/* 143 */	C("0")	//wornunk4	(prolly RecastType)
/* 143 */	C("0")	//wornunk5	(prolly ProcRate)
/* 143 */	C("")	//wornunk6
/* 143 */	C("-1")	//wornunk7
/* 149 */	I(Focus.Effect)
/* 150 */	I(Focus.Type)
/* 151 */	I(Focus.Level2)
/* 152 */	I(Focus.Level)
/* 143 */	C("0")	//focusunk1	(prolly MaxCharges)
/* 143 */	C("0")	//focusunk2	(prolly CastTime)
/* 143 */	C("0")	//focusunk3	(prolly RecastDelay)
/* 143 */	C("0")	//focusunk4	(prolly RecastType)
/* 143 */	C("0")	//focusunk5	(prolly ProcRate)
/* 143 */	C("")	//focusunk6
/* 143 */	C("-1")	//focusunk7
/* 154 */	I(Scroll.Effect)
/* 155 */	I(Scroll.Type)
/* 156 */	I(Scroll.Level2)
/* 157 */	I(Scroll.Level)
/* 143 */	C("0")	//scrollunk1	(prolly MaxCharges)
/* 143 */	C("0")	//scrollunk2	(prolly CastTime)
/* 143 */	C("0")	//scrollunk3	(prolly RecastDelay)
/* 143 */	C("0")	//scrollunk4	(prolly RecastType)
/* 143 */	C("0")	//scrollunk5	(prolly ProcRate)
/* 143 */	C("")	//scrollunk6
/* 143 */	C("-1")	//scrollunk7
/* 193 */	C("0")	//Power Source Capacity 
/* 194 */	C("0")	//purity
#undef I
#undef C
#undef S
#undef F

#endif



