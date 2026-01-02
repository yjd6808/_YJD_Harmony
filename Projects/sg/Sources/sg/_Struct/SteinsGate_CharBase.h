/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 12:21:52 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(CharType)
	Gunner,
	Begin = Gunner,
	End = Gunner,
	Max
JC_SENUM_MIDDLE(CharType)
static constexpr const char* Name[Max]{
	"gunner"
};
JC_SENUM_MIDDLE_END(CharType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(SpriteDirection)
	Right,
	Left,
	Max
JC_SENUM_MIDDLE(SpriteDirection)
static constexpr const char* Name[Max]{
	"Right",
	"Left"
};

static constexpr SpriteDirection_t Reverse[Max]{
	Left,
	Right
};
JC_SENUM_MIDDLE_END(SpriteDirection)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(Direction)
	Right,
	Left,
	Up,
	Down,
	Max,
	None
JC_SENUM_END(Direction)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(JumpDirection)
	None,
	Up = Direction::Up,
	Down
JC_SENUM_END(JumpDirection)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AvatarType)
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
JC_SENUM_MIDDLE(AvatarType)
static constexpr const char* Name[Max]{
	"skin",
	"shoes",
	"pants",
	"neck",
	"hair",
	"face",
	"coat",
	"cap",
	"belt"
};

static constexpr const char* ImgPrefix[Max]{
	"gn_body",
	"gn_shoes",
	"gn_pants",
	"gn_neck",
	"gn_hair",
	"gn_face",
	"gn_coat",
	"gn_cap",
	"gn_belt"
};
JC_SENUM_MIDDLE_END(AvatarType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(VisualType)
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
JC_SENUM_MIDDLE(VisualType)

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
	190 // Weapon
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
	true // Weapon
};

JC_SENUM_MIDDLE_END(VisualType)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(ActorType)
None,
Character,
Monster,
Npc,
Projectile,
MapObject,
Effect,
Max
JC_SENUM_MIDDLE(ActorType)

static constexpr const char* Name[Max]{
	"None",
	"Character",
	"Monster",
	"Npc",
	"Projectile",
	"MapObject",
	"Effect"
};
JC_SENUM_MIDDLE_END(ActorType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(WeaponType)
Automatic,
Begin = Automatic,
GunnerBegin = Begin,
Revolver,
Bowgun,
Musket,
HandCannon,
GunnerEnd = HandCannon,
Max
JC_SENUM_MIDDLE(WeaponType)

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

static WeaponType_t getType(const jc::String& _name);

JC_SENUM_MIDDLE_END(WeaponType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(BaseAction)
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
JC_SENUM_END(BaseAction)

//////////////////////////////////////////////////////////////////////////////////////////
struct CharBaseInfo : SDescBase
{
	~CharBaseInfo() override = default;

	// 캐릭터 기본 정보
	// int Code;        // 캐릭터 타입
	int hp_;
	int mp_;
	int strength_;
	int dexterity_;
	int vitality_;
	int intelligence_;
	int hpLevelUp_;
	int mpLevelUp_;
	int levelUpStat_;
	float jumpForce_;
	float slidingForce_;
	float downRecoverTime_;
	ThicknessBox thicknessBox_;
	WeaponType_t defaultWeaponType_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct CharInfo : CharBaseInfo
{
	~CharInfo() override = default;

	VisualData visual_[VisualType::Max];
	int visualCount_[VisualType::Max];
	bool hasVisual_[VisualType::Max];
};

//////////////////////////////////////////////////////////////////////////////////////////
struct GunnerInfo : CharInfo
{
	~GunnerInfo() override = default;

	int shotCount_[WeaponType::Max]; // 지상 공격 횟수
	int jumpShotCount_[WeaponType::Max]; // 점프 공격 횟수
	float attackSpeed_[WeaponType::Max]; // 공속
	float jumpShotForceX_[WeaponType::Max]; // 점프 공격 X 반동
	float jumpShotForceY_[WeaponType::Max]; // 점프 공격 Y 반동
	float jumpShotMoveSpeedX_[WeaponType::Max]; // 점프 공격 X 반동스피드
};
