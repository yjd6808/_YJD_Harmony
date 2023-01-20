/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:06:04 PM
 * =====================
 *
 */


#pragma once
#pragma warning(disable: 26482) // Only index into arrays using constant expressions.


// enum class 쓰긴 싫고 enum은 타입 이름이 겹쳐서 싫고 대안으로 struct안에 enum 끼여서 쓰는건 어떨까?
// 이번에 개발 할때 써보고 영 아니면 폐기하는걸로, 쓴지 하루지났는데 아직 갠찮은듯
#define SEnumBegin(enum_name) struct enum_name { enum_name() = delete; enum _ {
#define SEnumInteritedBegin(enum_name, base_enum) struct enum_name : base_enum { enum_name() = delete; enum _ {
#define SEnumMiddle(enum_name) }; using enum_name##_t = enum_name::_;
#define SEnumMiddleEnd(enum_name) }; using enum_name##_t = enum_name::_;
#define SEnumEnd(enum_name) }; }; using enum_name##_t = enum_name::_;


SEnumBegin(CharacterType)
Gunner,
Max
SEnumEnd(CharacterType)

SEnumBegin(SpriteDirection)
Right,
Left,
Max
SEnumEnd(SpriteDirection)


SEnumInteritedBegin(Direction, SpriteDirection)
Up = SpriteDirection::Max,
Down,
Max,
None
SEnumEnd(Direction)



SEnumBegin(JumpDirection)
None,
Up = Direction::Up,
Down
SEnumEnd(JumpDirection)


SEnumBegin(AvatarType)
Begin,
Skin = Begin,
Shoes,
Pants,
Neck,
Hair,
Face,
Coat,
Cap,
Belt,
Weapon,
End = Weapon,
Max
SEnumEnd(AvatarType)

SEnumBegin(MotionState)

IdleBreath,
IdleReload,
IdleGunRolling,
ShotRightBegin,
ShotRightShot,
ShotRightEnd,
ShotLeftBegin,
ShotLeftShot,
ShotLeftEnd,
ShotRightDownBegin,
ShotRightDownShot,
ShotRightDownEnd,
ShotLeftDownBegin,
ShotLeftDownShot,
ShotLeftDownEnd,
Walk,
JumpBegin,
JumpingUp,
JumpingDown,
JumpingDownGround,
JumpingShotRightBegin,
JumpingShot,
Run,
Sliding,
Windmill,
Hit1,
Hit2,
AirHit,
AirHitGround,
Sit,
JackSpike,
MahaKick,
Max

SEnumMiddle(MotionState)

inline static constexpr const char* Name[]{
"IdleBreath",
"IdleReload",
"IdleGunRolling",
"ShotRightBegin",
"ShotRightShot",
"ShotRightEnd",
"ShotLeftBegin",
"ShotLeftShot",
"ShotLeftEnd",
"ShotRightDownBegin",
"ShotRightDownShot",
"ShotRightDownEnd",
"ShotLeftDownBegin",
"ShotLeftDownShot",
"ShotLeftDownEnd",
"Sliding",
"Windmill",
"Hit1",
"Hit2",
"AirHit",
"AirHitGround",
"Sit",
"JackSpike",
"MahaKick"
"Max"
};

SEnumMiddleEnd(MotionState)

SEnumBegin(MotionStateBegin)
IdleBreath = 0,
IdleReload = 4,
IdleGunRolling = 15,
ShotRightBegin = 23,
ShotRightShot = 26,
ShotRightEnd = 28,
ShotLeftBegin = 30,
ShotLeftShot = 35,
ShotLeftEnd = 37,
ShotRightDownBegin = 39,
ShotRightDownShot = 42,
ShotDownEnd = 44,
ShotLeftDownBegin = 46,
ShotLeftDownShot = 51,
ShotLeftDownEnd = 53,
Walk = 55,
JumpBegin = 63,
JumpingUp = 64,
JumpingDown = 66,
JumpingDownGround = 68,
JumpingShotRightBegin = 70,
JumpingShot = 71,

// (이동사격)

Run = 103,
Sliding = 111,
Windmill = 115,
Hit1 = 121,
Hit2 = 122,
AirHit = 123,
AirHitGround = 126,
Sit = 128,

// (중화기)

JackSpike = 155,
MahaKick = 159,

// (퍼니셔)

Max = 250
SEnumEnd(MotionStateBegin)



SEnumBegin(MotionStateEnd)
IdleBreath = 3,
IdleReload = 14,
IdleGunRolling = 22,
ShotRightBegin = 24,
ShotRightShot = 27,
ShotRightEnd = 29,
ShotLeftBegin = 33,
ShotLeftShot = 36,
ShotLeftEnd = 38,
ShotRightDownBegin = 40,
ShotRightDownShot = 43,
ShotDownEnd = 45,
ShotLeftDownBegin = 49,
ShotLeftDownShot = 52,
ShotLeftDownEnd = 54,
Walk = 62,
JumpBegin = 63,
JumpingUp = 65,
JumpingDown = 67,
JumpingDownGround = 69,
JumpingShotRightBegin = 70,
JumpingShot = 74,
Run = 110,
Sliding = 114,
Windmill = 120,
Hit1 = 121,
Hit2 = 122,
AirHit = 124,
AirHitGround = 127,
Sit = 128,
JackSpike = 158,
MahaKick = 164,

// (퍼니셔)

Max
SEnumEnd(MotionStateEnd)


// 인덱스 기반 타입이므로 무조건 0부터 시작해야한다.
// 변수 초기화는 None으로 수동으로 할 것
SEnumBegin(ControlKey)
Right		,
Left		,
Up			,
Down		,
Attack		,
Jump		,
Max			,
None
SEnumMiddle(ControlKey)

static constexpr const char* Name[Max]{
	"→"	,
	"←"	,
	"↑"	,
	"↓"	,
	"X",
	"C"	
};

static constexpr bool IsMoveKey[Max] {
	true, true, true, true, false, false
};

static constexpr ControlKey_t ReverseDirection[Direction::Max]{
	Left,
	Right,
	Down,
	Up
};

SEnumMiddleEnd(ControlKey)


SEnumBegin(ActionType)
Idle,
Shot,
SlidingLeft,
SlidingRight,
RunLeft,
RunRight,
Jump,
Max
SEnumEnd(ActionType)


SEnumBegin(ActorType)
None,
Player,
Monster,
Npc,
Projectile,
Obstacle,
Max
SEnumEnd(ActorType)


SEnumBegin(WeaponType)
None,
Auto,
Revolver,
Bowgun,
Musket,
HandCannon,
Max
SEnumMiddle(WeaponType)

static constexpr const char* Name[] {
	"none",
	"auto",
	"rev",
	"bowgun",
	"musket",
	"hcan"
};

SEnumMiddleEnd(WeaponType)


SEnumBegin(AttackDamangeType)
None,
Physics,
Magic,
Fixed,
Max
SEnumEnd(AttackDamangeType)

