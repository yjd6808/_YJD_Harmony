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
Gunner,
Begin = Gunner,
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
SEnumMiddle(AvatarType)
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


SEnumMiddleEnd(AvatarType)


SEnumBegin(VisualType)
Skin	= AvatarType::Skin	,
Begin = Skin,
AvatarBegin = Begin,
Shoes	= AvatarType::Shoes	,
Pants	= AvatarType::Pants	,
Neck	= AvatarType::Neck	,
Hair	= AvatarType::Hair	,
Face	= AvatarType::Face	,
Coat	= AvatarType::Coat	,
Cap		= AvatarType::Cap	,
Belt	= AvatarType::Belt	,
AvatarEnd = Belt,
Weapon,
Max,
None
SEnumMiddle(VisualType)


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

SEnumBegin(WeaponType)
Automatic,
Begin = Automatic,
GunnerBegin = Begin,
Revolver,
Bowgun,
Musket,
HandCannon,
GunnerEnd = HandCannon,
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


static constexpr const char* ImgPrefix[] {
	"auto",
	"rev",
	"bowgun",
	"musket",
	"hcan"
};

static WeaponType_t getType(const JCore::String& name);

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

SEnumBegin(HAlignment)
Left,
Center,
Right
SEnumEnd(HAlignment)

SEnumBegin(VAlignment)
Top,
Center,
Bottom
SEnumEnd(VAlignment)

SEnumBegin(UIElementType)
Group,
Button,
Label,
Sprite,
EditBox,
CheckBox,
ToggleButton,
ScrollBar,
ProgressBar,
Static,
Max

SEnumMiddle(UIElementType)

static constexpr const char* Name[Max]{
	"그룹",
	"버튼",
	"라벨",
	"스프라이트",
	"에딧박스",
	"체크박스",
	"토글버튼",
	"스크롤바",
	"프로그래스바",
	"스태틱"
};


SEnumMiddleEnd(UIElementType)


SEnumBegin(ConfigFileType)
Effect,
Begin = Effect,
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
ItemOpt,
Enchant,
Common,
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
	"수동로딩"				,
	"item_opt.json"			,
	"enchant.json"			,
	"common.json"
};
SEnumMiddleEnd(ConfigFileType)


SEnumBegin(InvenItemType)
Quest,
Etc,
Consume,
Equip,
Avatar,
Max
SEnumMiddle(InvenItemType)

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
SEnumMiddleEnd(InvenItemType)


// https://wiki.dfo-world.com/view/Armor
SEnumBegin(ItemType)
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
SEnumMiddle(ItemType)

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

SEnumMiddleEnd(ItemType)


SEnumBegin(EquipArmorType)
Cloth,			// 천
Begin = Cloth,
Leather,		// 가죽
Light,			// 경갑
Heavy,			// 중갑
Place,			// 판금
End,
Max = End
SEnumMiddle(EquipArmorType)

static constexpr const char* Name[Max]{
	"cloth"		,
	"leather" 	,	
	"light"		,
	"heavy"	  	,
	"plate"	  	
};

SEnumMiddleEnd(EquipArmorType)



SEnumBegin(RarityType)
Normal,
Begin = Normal,
Magic,
Rare,
Legacy,
Unique,
Epic,
End = Epic,
Max
SEnumMiddle(RarityType)

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

SEnumMiddleEnd(RarityType)



SEnumBegin(ItemOptType)
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
SEnumMiddle(ItemOptType)

SEnumMiddleEnd(ItemOptType)


SEnumBegin(ServerType)
Begin = 1,
Luke = Begin,
Hilder,
Siroco,
Prey,
Kasias,		// 대충 발음나는데..
Diregie,
Cain,
Seria,
End = Seria
SEnumMiddle(ServerType)

SEnumMiddleEnd(ServerType)

SEnumBegin(EnteranceType)
Begin = 1,
Newbie = Begin,
Auction,
Normal,
End = Normal
SEnumMiddle(EnteranceType)

SEnumMiddleEnd(EnteranceType)


SEnumBegin(ChannelDensity)
Free,
Normal,
Busy,
Full,
Max
SEnumMiddle(ChannelDensity)

float Density[Max]{
	25.0f,
	50.0f,
	75.0f,
	100.0f
};

inline static const char* Name[Max]{
	"쾌적",
	"보통",
	"혼잡",
	"포화"
};

SEnumMiddleEnd(ChannelDensity)

SEnumBegin(ProgressIncreaseDirection)
LeftRight,
RightLeft,
TopBottom,
BottomTop
SEnumEnd(ProgressIncreaseDirection)