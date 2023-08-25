/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:50:02 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(InvenItemType)
Quest,
Etc,
Consume,
Equip,
Avatar,
Max
JCORE_SENUM_MIDDLE(InvenItemType)

static constexpr bool Stackable[Max] {
	true,
	true,
	true,
	false,
	false
};

static constexpr const char* Name[Max]{
	"quest",
	"etc",
	"consume",
	"equip",
	"avatar"
};
JCORE_SENUM_MIDDLE_END(InvenItemType)


// ==========================================================================================

// https://wiki.dfo-world.com/view/Armor
JCORE_SENUM_BEGIN(ItemType)
Shoulder,			// 어깨
Begin = Shoulder,
BeginEquip = Begin,
Top,				// 상의
Bottom,				// 하의
Shoes,				// 신발
Belt,				// 벨트
Ring,				// 반지
Neck,				// 목걸이
Bracelet,			// 팔찌
Weapon,				// 무기
Title,				// 칭호
Avatar,				// 아바타
EndEquip = Avatar,
MaxInvenEquip = EndEquip,
MaxEquip = EndEquip + 1,
Consume = MaxEquip,	// 소모품
Etc,				// 기타
Quest,				// 퀘스트
End,
Max = End
JCORE_SENUM_MIDDLE(ItemType)

inline static constexpr bool IsEquip[Max]{
	true,			// 어깨
	true,			// 상의
	true,			// 하의
	true,			// 신발
	true,			// 벨트
	true,			// 반지
	true,			// 목걸이
	true,			// 팔찌
	true,			// 무기
	true,			// 칭호
	true,			// 아바타
	false,			// 소모품
	false,			// 기타
	false,			// 퀘스트
};

inline static constexpr bool IsCommonEquip[Max]{
	true,			// 어깨
	true,			// 상의
	true,			// 하의
	true,			// 신발
	true,			// 벨트
	true,			// 반지
	true,			// 목걸이
	true,			// 팔찌
	false,			// 무기
	true,			// 칭호
	false,			// 아바타
	false,			// 소모품
	false,			// 기타
	false,			// 퀘스트
};

inline static constexpr bool HasDetailType[Max]{
	true,			// 어깨
	true,			// 상의
	true,			// 하의
	true,			// 신발
	true,			// 벨트
	false,			// 반지
	false,			// 목걸이
	false,			// 팔찌
	true,			// 무기
	false,			// 칭호
	true,			// 아바타
	true,			// 소모품
	false,			// 기타
	false,			// 퀘스트
};

inline static constexpr const char* Name[Max]{
	"shoulder"	,	// 어깨
	"top" 		,	// 상의
	"bottom"	,  	// 하의
	"shoes"	  	,	// 신발
	"belt"	  	,	// 벨트
	"ring"	  	,	// 반지
	"neck"	  	,	// 목걸이
	"bracelet"	,	// 팔찌
	"weapon"  	,	// 무기
	"title"		,	// 칭호
	"avatar"	,	// 아바타
	"consume"	,	// 소모품
	"etc"		,	// 기타
	"quest"			// 퀘스트
};

JCORE_SENUM_MIDDLE_END(ItemType)


// ==========================================================================================


JCORE_SENUM_BEGIN(EquipArmorType)
Cloth,			// 천
Begin = Cloth,
Leather,		// 가죽
Light,			// 경갑
Heavy,			// 중갑
Place,			// 판금
End,
Max = End
JCORE_SENUM_MIDDLE(EquipArmorType)

static constexpr const char* Name[Max]{
	"cloth"		,
	"leather" 	,	
	"light"		,
	"heavy"	  	,
	"plate"	  	
};

JCORE_SENUM_MIDDLE_END(EquipArmorType)

// ==========================================================================================

JCORE_SENUM_BEGIN(RarityType)
Normal,
Begin = Normal,
Magic,
Rare,
Legacy,
Unique,
Epic,
End = Epic,
Max
JCORE_SENUM_MIDDLE(RarityType)

static constexpr const char* Name[Max]{
	"Normal"	,
	"Magic"		,
	"Rare"		,
	"Legacy"	,
	"Unique"	,
	"Epic"		,
};

static constexpr int OptMinCount[Max]{
	0,
	1,
	2,
	3,
	3,
	3,
};

static constexpr int OptMaxCount[Max]{
	1,
	3,
	5,
	7,
	6,
	0
};

JCORE_SENUM_MIDDLE_END(RarityType)

// ==========================================================================================

JCORE_SENUM_BEGIN(ItemOptType)
Begin = 1,
Str = Begin,					// 1
Dex,							// 2
Int,							// 3
Vit,							// 4
Energe,							// 5
Lift,							// 6
Mana,							// 7 
DamagePhysical,					// 8
DamageMagic,					// 9
DamagePhysicalAdditional,		// 10
DamageMagicAdditional,			// 11
DamagePhysiclReduce,			// 12
DamageMagicReduce,				// 13
DamagePhysiclReducePercent,		// 14
DamageMagicReducePercent,		// 15
ArmorPhysical,					// 16
ArmorMagic,						// 17
ArmorPhysicalPercent,			// 18
ArmorMagicPercent,				// 19
ChanceMagic,					// 20
ChanceGold,						// 21
AttackSpeed,					// 22
CastSpeed,						// 23
RegenLifePerMin,				// 24
RegenManaPerMin,				// 25
MoveSpeedInDungeon,				// 26
MoveSpeedInTown,				// 27
End = MoveSpeedInTown,
Max
JCORE_SENUM_MIDDLE(ItemOptType)

JCORE_SENUM_MIDDLE_END(ItemOptType)

// ==========================================================================================