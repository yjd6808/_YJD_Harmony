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
Right	,
Left	,
Max
SEnumEnd(SpriteDirection)


SEnumInteritedBegin(Direction, SpriteDirection)
Up = SpriteDirection::Max,
Down	,
Max		,
None
SEnumEnd(Direction)



SEnumBegin(JumpDirection)
None,
Up = Direction::Up,
Down
SEnumEnd(JumpDirection)


SEnumBegin(AvatarPartType)
Begin		,
Skin = Begin,
Shoes		,
Pants		,
Neck		,
Hair		,
Face		,
Coat		,
Cap			,
Belt		,
End = Belt	,
Max
SEnumEnd(AvatarPartType)


SEnumInteritedBegin(CharacterPartType, AvatarPartType)
Begin		,
Skin = Begin,
Shoes		,
Pants		,
Neck		,
Hair		,
Face		,
Coat		,
Cap			,
Belt		,
Weapon		,
End = Weapon,
Max
SEnumEnd(CharacterPartType)

SEnumBegin(MotionState)

IdleBegin						,
IdleBreath = IdleBegin			,
IdleReload						,
IdleGunRolling					,
IdleEnd = IdleGunRolling		,
ShotBegin						,
ShotRightBegin = ShotBegin	,
ShotRightShot 		 		,		
ShotRightEnd 					,	 
ShotLeftBegin 				 	,
ShotLeftShot 				 	,
ShotLeftEnd 	 				,
ShotRightDownBegin 	 		,		
ShotRightDownShot 	 		,		
ShotRightDownEnd 				,	 
ShotLeftDownBegin				,
ShotLeftDownShot				,
ShotLeftDownEnd				,
ShotEnd = ShotLeftDownEnd		,
Walk 							, 
JumpBegin 		 				,
JumpingUp 		 				,
JumpingDown 					, 
JumpingDownGround				,
JumpingShotBegin  				,
JumpingShot					,
JumpEnd = JumpingShot			,
Run				 				,
Sliding 						, 
Windmill 						, 
Hit1 							, 
Hit2 							, 
AirHit 			 				,
AirHitGround 					, 
Sit 							, 
JackSpike 		 				,
MahaKick 						,
Max

SEnumEnd(MotionState)

SEnumBegin(MotionStateBegin)
IdleBreath = 0,
IdleReload = 4,
IdleGunRolling = 15,
ShotBegin = 23,
ShotShot = 25,
ShotEnd = 28,
ShotOtherBegin = 30,
ShotOtherShot = 34,
ShotOtherEnd = 37,
ShotDownBegin = 39,
ShotDownShot = 41,
ShotDownEnd = 44,
ShotOtherDownBegin = 46,
ShotOtherDownShot = 50,
ShotOtherDownEnd = 53,
Walk = 55,
JumpBegin = 63,
JumpingUp = 64,
JumpingDown = 66,
JumpingDownGround = 68,
JumpingShotBegin = 70,
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
ShotBegin = 24,
ShotShot = 27,
ShotEnd = 29,
ShotOtherBegin = 33,
ShotOtherShot = 36,
ShotOtherEnd = 38,
ShotDownBegin = 40,
ShotDownShot = 43,
ShotDownEnd = 45,
ShotOtherDownBegin = 49,
ShotOtherDownShot = 52,
ShotOtherDownEnd = 54,
Walk = 62,
JumpBegin = 63,
JumpingUp = 65,
JumpingDown = 67,
JumpingDownGround = 69,
JumpingShotBegin = 70,
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
	"J"	
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
Shot,
Sliding,
Run,
Jump,
Max
SEnumEnd(ActionType)


SEnumBegin(ActorType)
Player,
Monster,
Npc,
Max
SEnumEnd(ActorType)