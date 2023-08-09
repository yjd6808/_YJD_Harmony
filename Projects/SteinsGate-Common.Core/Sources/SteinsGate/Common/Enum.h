/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:06:04 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/EnumDefine.h>

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
MapObject,
Effect,
Max
SEnumMiddle(ActorType)

static constexpr const char* Name[Max]{
	"None",
	"Character",
	"Monster",
	"Npc",
	"Projectile",
	"MapObject",
	"Effect"
};

SEnumMiddleEnd(ActorType)


SEnumBegin(FrameEventType)
None,
Spawn,
AttackBoxInstant,
Max
SEnumEnd(FrameEventType)

SEnumBegin(FrameEventSpawnType)
Projectile,
AttackBox,
Max
SEnumEnd(FrameEventSpawnType)

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
static constexpr FrameEventSpawnType_t ShotFrameEventSpawnType[Max]{
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::Projectile,
	FrameEventSpawnType::AttackBox
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


SEnumBegin(AIActivityType)
Idle,
Walk,
Attack,
Hit,
FallDown,
Sit,
// TODO: Custom 몬스터 특수행동 그런거 추가할 수 있을 듯?
Max
SEnumMiddle(AIActivityType)
static constexpr const char* Name[Max]{
	"Idle",
	"Walk",
	"Attack",
	"Hit",
	"FallDown",
	"Sit"
};

SEnumMiddleEnd(AIActivityType)

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
MapPhysics,
MapObject,
Monster,
Monster_Animation_Frame_Event,
Monster_Attack_Data,
Monster_Projectile,
Monster_Stat,
Server,
Tile,
UI,
Action,
AI,
AttackBox,
Channel,
Char_Animation,
Char_Animation_Frame_Event,
Char_Attack_Data,
Char_Base,
Char_Common,
Char_Projectile,
Client,
ClientText,
Item,
ItemOpt,
Enchant,
Common,
Database,
End,
Max = End
SEnumMiddle(ConfigFileType)

static constexpr const char* FileName[Max]{
	"effect.json"						,		// 파일
	"map"								,		// 디렉토리
	"map_physics.json"					,		// 파일
	"map_object.json"					,		// 파일
	"monster.json"						,		// 파일
	"monster_animation_frame_event.json",		// 파일
	"monster_attack_data.json"			,		// 파일
	"monster_projectile.json"			,		// 파일
	"monster_stat.json"					,		// 파일
	"server.json"						,		// 파일
	"tile.json"							,		// 파일
	"ui.json"							,		// 파일
	"action.json"						,		// 파일
	"ai.json"							,		// 파일
	"attack_box.json"					,		// 파일
	"channel.json"						,		// 파일
	"char_animation.json"				,		// 파일
	"char_animation_frame_event.json"	,		// 파일
	"char_attack_data.json"				,		// 파일
	"char_base.json"					,		// 파일
	"char_common.json"					,		// 파일
	"char_projectile.json"				,		// 파일
	"client.json"						,		// 파일
	"client_text.json"					,		// 파일
	"item"								,		// 디렉토리
	"item_opt.json"						,		// 파일
	"enchant.json"						,		// 파일
	"common.json"						,		// 파일 common.json에서 다른 이름으로 변경시, 클라이언트와 각 서버프로젝트들 PostBuild이벤트로 수행하는 설정파일이름도 변경필요
	"database.json"								// 파일
	
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

SEnumBegin(GameServerType)
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
SEnumMiddle(GameServerType)

static constexpr const char* Name[End + 1]{
	"",
	"루크",
	"힐더",
	"시로코",
	"프레이",
	"카시아스",
	"디레지에",
	"카인",
	"세리아"
};
SEnumMiddleEnd(GameServerType)

SEnumBegin(EnteranceType)
Newbie,
Begin = Newbie,
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

static constexpr const char* Name[Max]{
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

SEnumBegin(DatabaseType)
Game = 1
SEnumEnd(DatabaseType)

SEnumBegin(ServerProcessType)
Center,
Auth,
Lobby,
Game,
Max
SEnumEnd(ServerProcessType)

// 클라이언트가 게임을 플레이하는 동안 한번이라도 접속하게되는 서버타입
SEnumBegin(ClientConnectServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
End = Area,
Max
SEnumEnd(ClientConnectServerType)

// 전체 서버 종류
SEnumBegin(ServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
Center,
End = Center,
Max
SEnumMiddle(ServerType)

static constexpr const char* Name[Max]{
	"인증",
	"로비",
	"로직",
	"채팅",
	"지역",
	"중앙"
};

SEnumMiddleEnd(ServerType)


SEnumBegin(MapObjectType)
Obstacle,
Begin = Obstacle,
Gate,
End = Gate,
SEnumEnd(MapObjectType)

SEnumBegin(MapType)
Dungeon,
Private,
Town
SEnumEnd(MapType)

SEnumBegin(MapBundleType)
Dungeon,
Town
SEnumEnd(MapBundleType)


/*
 * TODO: ActorSpriteData가 아닌 ActorPartSpriteData의 멤버로 오는게 올바르긴하다. 이 TODO는 JsonUtilEx::parseActorSpriteData의 TODO를 처리할 때 알아서 해결됨.
 *
 * 프레임내부에서 스프라이트 위치지정을 어떻게 할지 정하는 규칙
 * 기존 리소스의 이미지를 유심히 관찰 후 스프라이트 위치를 어떻게 적용시킬지 규칙을 정하였다.
 *
 * [InFrameSize]
 * 기존 스프라이트 프레임의 위치를 리소스에서 정해진 위치 그대로 적용
 *  1. 캐릭터 모션같이 프레임 내부에서 스프라이트 위치가 고정되어 있는 경우
 *  2. 몬스터의 각 파츠별 위치가 프레임 내부에서 고정되어 있는 경우
 *
 * [InCustomFrameSize]
 * 리소스에 기입된 프레임 사이즈를 무시하고 내가 자체적으로 프레임사이즈를 정의한다.
 *
 *  1. 맵 오브젝트(게이트, 장애물)의 경우
 * [InIgnoredFrameSize]
 * 프레임없이 
 */ 
SEnumBegin(ActorPartSpritePositioningRule)
InFrameSize,		// 캐릭터 모션
InCustomFrameSize,	// 맵 오브젝트(게이트, 장애물)
InIgnoredFrameSize	// 프로젝틸
SEnumEnd(ActorPartSpritePositioningRule)


SEnumBegin(GamePlayMode)
Single,
PublicServer,
Max
SEnumMiddle(GamePlayMode)

static constexpr bool IsNetworkMode[Max]{
	false,
	true,
};

SEnumMiddleEnd(GamePlayMode)
