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


SEnumBegin(CharType)
Begin,
Gunner = Begin,
End = Gunner,
Max
SEnumMiddle(CharType)
static constexpr const char* Name[Max]{
	"gunner"
};
SEnumMiddleEnd(CharType)

SEnumBegin(SpriteDirection)
Right,
Left,
Max
SEnumMiddle(SpriteDirection)
static constexpr const char* Name[Max]{
	"Right",
	"Left"
};

static constexpr SpriteDirection_t Reverse[Max]{
	Left,
	Right
};
SEnumMiddleEnd(SpriteDirection)


SEnumBegin(Direction)
Right,
Left,
Up,
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
Max,
None
SEnumEnd(AvatarType)


SEnumBegin(VisualType)
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
Max,
None
SEnumMiddle(VisualType)

inline static int ZOrder[Max]{
	0, // Skin
	1, // Shoes
	2, // Pants
	5, // Neck
	2, // Hair
	1, // Face
	4, // Coat
	3, // Cap
	6, // Belt
	8 // WeaponLeft
	// 7  // WeaponRight
};

SEnumMiddleEnd(VisualType)


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



SEnumBegin(ActorType)
None,
Character,
Monster,
Npc,
Projectile,
Obstacle,
Effect,
Max
SEnumMiddle(ActorType)

static constexpr const char* Name[Max]{
	"None",
	"Character",
	"Monster",
	"Npc",
	"Projectile",
	"Obstacle",
	"Effect"
};

SEnumMiddleEnd(ActorType)


SEnumBegin(FrameEventType)
Projectile = 1,
AttackBox,
AttackBoxInstant,
Max
SEnumEnd(FrameEventType)



SEnumBegin(GunnerWeaponType)
Begin,
Automatic = Begin,
Revolver,
Bowgun,
Musket,
HandCannon,
Max
SEnumEnd(GunnerWeaponType)


SEnumBegin(WeaponType)
Begin,
Automatic = Begin,
Revolver,
Bowgun,
Musket,
HandCannon,
Max
SEnumMiddle(WeaponType)

// 핸드캐논은 총 쏘는 방식이 히트박스임, 나머진 투사체 날림
static constexpr FrameEventType_t ShotFrameEventType[Max]{
	FrameEventType::Projectile,
	FrameEventType::Projectile,
	FrameEventType::Projectile,
	FrameEventType::Projectile,
	FrameEventType::AttackBox
};


static constexpr const char* Name[] {
	"automatic",
	"revolver",
	"bowgun",
	"musket",
	"handcannon"
};

SEnumMiddleEnd(WeaponType)


SEnumBegin(AttackDamageType)
None,
Physics,
Magic,
Fixed,
Max
SEnumEnd(AttackDamageType)

SEnumBegin(AttackXForceDirection)
None,
Forward,
Backward,
Spread,
Max
SEnumEnd(AttackXForceDirection)


SEnumBegin(BaseAction)
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
SEnumEnd(BaseAction)

SEnumBegin(AIWanderDecision)
Walk,
Idle,
Max
SEnumEnd(AIWanderDecision)

SEnumBegin(AITrackDecision)
Wander,
Attack,
Follow,
Max
SEnumEnd(AITrackDecision)

SEnumBegin(AIAngryDecision)
Wander,
Track,
Attack,
Max
SEnumEnd(AIAngryDecision)


SEnumBegin(AIActivity)
Idle,
Walk,
Attack,
Hit,
FallDown,
Sit,
// TODO: Custom 몬스터 특수행동 그런거 추가할 수 있을 듯?
Max
SEnumMiddle(AIActivity)
static constexpr const char* Name[Max]{
	"Idle",
	"Walk",
	"Attack",
	"Hit",
	"FallDown",
	"Sit"
};

SEnumMiddleEnd(AIActivity)

SEnumBegin(SceneType)
Login,
ChannelSelect,
Game,
None,
Max = None
SEnumMiddle(SceneType)
static constexpr const char* Name[Max]{
	"로그인",
	"채널 선택",
	"게임",
};
SEnumMiddleEnd(SceneType)

SEnumBegin(HorizontalAlignment)
Left,
Center,
Right
SEnumEnd(HorizontalAlignment)

SEnumBegin(VerticalAlignment)
Top,
Center,
Bottom
SEnumEnd(VerticalAlignment)

SEnumBegin(UIElementType)
Group,
Button,
Label,
Sprite,
Max
SEnumEnd(UIElementType)



SEnumBegin(ConfigFileType)
Begin,
Effect = Begin,
Map,
Monster,
Obstacle,
Projectile,
Server,
Tile,
UI,
Action,
AI,
AttackBox,
AttackData,
Channel,
Char_Animation,
Char_Base,
Client,
Item,
End,
Max = End
SEnumMiddle(ConfigFileType)

static constexpr const char* FileName[Max]{
	"effect.json"			,
	"map.json"				,
	"monster.json"			,
	"obstacle.json"			,
	"projectile.json"		,
	"server.json"			,
	"tile.json"				,
	"ui.json"				,
	"action.json"			,
	"ai.json"				,
	"attackbox.json"		,
	"attackdata.json"		,
	"channel.json"			,
	"char_animation.json"	,
	"char_base.json"		,
	"client.json"			,
	"수동로딩"
};
SEnumMiddleEnd(ConfigFileType)


SEnumBegin(ItemInvenType)
Quest,
Etc,
Consume,
Equip,
Avatar,
Max
SEnumMiddle(ItemInvenType)

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
SEnumMiddleEnd(ItemInvenType)

//Shoulder = Begin,	// 천
//Bottom,				// 가죽

// https://wiki.dfo-world.com/view/Armor
SEnumBegin(EquipItemType)
Begin,
Shoulder = Begin,	// 어깨
Top,				// 상의
Bottom,				// 하의
Shoes,				// 신발
Belt,				// 벨트
Ring,				// 반지
Neck,				// 목걸이
Bracelet,			// 팔찌
Weapon,				// 무기
Title,				// 칭호
End,
Max = End
SEnumMiddle(EquipItemType)

static constexpr const char* Name[Max]{
	"shoulder"	,
	"top" 		,
	"bottom"	,  
	"shoes"	  	,
	"belt"	  	,
	"ring"	  	,
	"neck"	  	,
	"bracelet"	,
	"weapon"  	,
	"title"
};

SEnumMiddleEnd(EquipItemType)


SEnumBegin(EquipArmorItemType)
Begin,
Cloth,			// 천
Leather,		// 가죽
Light,			// 경갑
Heavy,			// 중갑
Place,			// 판금
End,
Max = End
SEnumMiddle(EquipArmorItemType)

static constexpr const char* Name[Max]{
	"cloth"		,
	"leather" 	,	
	"light"		,
	"heavy"	  	,
	"place"	  	
};

SEnumMiddleEnd(EquipArmorItemType)



SEnumBegin(RarityType)
Begin,
Normal = Begin,
Magic,
Rare,
Legacy,
Unique,
Epic,
Set,
End,
Max = End
SEnumMiddle(RarityType)

static constexpr const char* Name[Max]{
	"Normal"	,
	"Magic"		,
	"Rare"		,
	"Unique"	,
	"Legacy"	,
	"Epic"		,
	"Set"	
};

static constexpr int OptCount[Max]{
	1,
	2,
	3,
	4,
	3,
	0
};

SEnumMiddleEnd(RarityType)



SEnumBegin(ItemOptType)
Begin,
Str = Begin,
Dex,
Int,
Vit,
Lift,
Mana,
DamagePhysical,
DamageMagic,
ArmorPhysical,
ArmorMagic,
ChanceMagic,
ChanceGold,
AttackSpeed,
CastSpeed,
RegenLifePerMin,
RegenManaPerMin,
MoveSpeedInDungeon,
MoveSpeedInTown,
End,
Max = End
SEnumMiddle(ItemOptType)

SEnumMiddleEnd(ItemOptType)





