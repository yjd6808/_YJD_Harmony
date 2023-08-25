/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:47:54 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

#include <SteinsGate/Common/Enum_ANIMATION.h>

JCORE_SENUM_BEGIN(CharType)
Gunner,
Begin = Gunner,
End = Gunner,
Max
JCORE_SENUM_MIDDLE(CharType)
static constexpr const char* Name[Max]{
	"gunner"
};
JCORE_SENUM_MIDDLE_END(CharType)

// ==========================================================================================

JCORE_SENUM_BEGIN(SpriteDirection)
Right,
Left,
Max
JCORE_SENUM_MIDDLE(SpriteDirection)
static constexpr const char* Name[Max]{
	"Right",
	"Left"
};

static constexpr SpriteDirection_t Reverse[Max]{
	Left,
	Right
};
JCORE_SENUM_MIDDLE_END(SpriteDirection)

// ==========================================================================================

JCORE_SENUM_BEGIN(Direction)
Right,
Left,
Up,
Down,
Max,
None
JCORE_SENUM_END(Direction)

// ==========================================================================================

JCORE_SENUM_BEGIN(JumpDirection)
None,
Up = Direction::Up,
Down
JCORE_SENUM_END(JumpDirection)

// ==========================================================================================

JCORE_SENUM_BEGIN(AvatarType)
Skin,
Begin = Skin,
Shoes,
Pants,
Neck,
Hair,
Face,
Coat,
Cap,
Belt,
End,
Max = End,
None
JCORE_SENUM_MIDDLE(AvatarType)
static constexpr const char* Name[Max]{
	"skin"	,
	"shoes"	,
	"pants"	,
	"neck"	,
	"hair"	,
	"face"	,
	"coat"	,
	"cap"	,
	"belt"
};

static constexpr const char* ImgPrefix[Max]{
	"gn_body"	,
	"gn_shoes"	,
	"gn_pants"	,
	"gn_neck"	,
	"gn_hair"	,
	"gn_face"	,
	"gn_coat"	,
	"gn_cap"	,
	"gn_belt"
};


JCORE_SENUM_MIDDLE_END(AvatarType)

// ==========================================================================================

JCORE_SENUM_BEGIN(VisualType)
Skin = AvatarType::Skin,
Begin = Skin,
AvatarBegin = Begin,
Shoes = AvatarType::Shoes,
Pants = AvatarType::Pants,
Neck = AvatarType::Neck,
Hair = AvatarType::Hair,
Face = AvatarType::Face,
Coat = AvatarType::Coat,
Cap = AvatarType::Cap,
Belt = AvatarType::Belt,
AvatarEnd = Belt,
Weapon,
Max,
None
JCORE_SENUM_MIDDLE(VisualType)


// 스킨 0으로 두면 안됨
inline static constexpr int ZOrder[Max]{
	100, // Skin
	110, // Shoes
	120, // Pants
	150, // Neck
	120, // Hair
	110, // Face
	140, // Coat
	130, // Cap
	160, // Belt
	190, // Weapon
};

inline static const char* Name[Max]{
	AvatarType::Name[Skin],
	AvatarType::Name[Shoes],
	AvatarType::Name[Pants],
	AvatarType::Name[Neck],
	AvatarType::Name[Hair],
	AvatarType::Name[Face],
	AvatarType::Name[Coat],
	AvatarType::Name[Cap],
	AvatarType::Name[Belt],
	"weapon"
};

inline static constexpr bool IsAvatar[Max]{
	true, // Skin
	true, // Shoes
	true, // Pants
	true, // Neck
	true, // Hair
	true, // Face
	true, // Coat
	true, // Cap
	true, // Belt
	false // Weapon
};


inline static constexpr bool IsWeapon[Max]{
	false, // Skin
	false, // Shoes
	false, // Pants
	false, // Neck
	false, // Hair
	false, // Face
	false, // Coat
	false, // Cap
	false, // Belt
	true   // Weapon
};



JCORE_SENUM_MIDDLE_END(VisualType)


// ==========================================================================================

// 인덱스 기반 타입이므로 무조건 0부터 시작해야한다.
// 변수 초기화는 None으로 수동으로 할 것
JCORE_SENUM_BEGIN(ControlKey)
Right,
Left,
Up,
Down,
Attack,
Jump,
Max,
None
JCORE_SENUM_MIDDLE(ControlKey)

static constexpr const char* Name[Max]{
	"→"	,
	"←"	,
	"↑"	,
	"↓"	,
	"X",
	"C"
};

static constexpr bool IsMoveKey[Max]{
	true, true, true, true, false, false
};

static constexpr ControlKey_t ReverseDirection[Direction::Max]{
	Left,
	Right,
	Down,
	Up
};

JCORE_SENUM_MIDDLE_END(ControlKey)

// ==========================================================================================

JCORE_SENUM_BEGIN(ActorType)
None,
Character,
Monster,
Npc,
Projectile,
MapObject,
Effect,
Max
JCORE_SENUM_MIDDLE(ActorType)

static constexpr const char* Name[Max]{
	"None",
	"Character",
	"Monster",
	"Npc",
	"Projectile",
	"MapObject",
	"Effect"
};

JCORE_SENUM_MIDDLE_END(ActorType)

// ==========================================================================================

JCORE_SENUM_BEGIN(WeaponType)
Automatic,
Begin = Automatic,
GunnerBegin = Begin,
Revolver,
Bowgun,
Musket,
HandCannon,
GunnerEnd = HandCannon,
Max
JCORE_SENUM_MIDDLE(WeaponType)

// 핸드캐논은 총 쏘는 방식이 히트박스임, 나머진 투사체 날림
static constexpr FrameEventSpawnType_t ShotFrameEventSpawnType[Max]{
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::AttackBox
};

static constexpr const char* Name[]{
	"automatic",
	"revolver",
	"bowgun",
	"musket",
	"handcannon"
};


static constexpr const char* ImgPrefix[]{
	"auto",
	"rev",
	"bowgun",
	"musket",
	"hcan"
};

static WeaponType_t getType(const JCore::String& name);

JCORE_SENUM_MIDDLE_END(WeaponType)

// ==========================================================================================

JCORE_SENUM_BEGIN(AttackDamageType)
None,
Physics,
Magic,
Fixed,
Max
JCORE_SENUM_END(AttackDamageType)

// ==========================================================================================

JCORE_SENUM_BEGIN(AttackXForceDirection)
None,
Forward,
Backward,
Spread,
Max
JCORE_SENUM_END(AttackXForceDirection)

// ==========================================================================================

JCORE_SENUM_BEGIN(BaseAction)
Idle,
Walk,
Run,
Sliding,
Attack,
Jump,
Hit,
FallDown,
SitRecover,
SitPickUp,
Max
JCORE_SENUM_END(BaseAction)

// ==========================================================================================