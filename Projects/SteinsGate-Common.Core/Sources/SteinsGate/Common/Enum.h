/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:06:04 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

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


JCORE_SENUM_BEGIN(Direction)
Right,
Left,
Up,
Down,
Max,
None
JCORE_SENUM_END(Direction)



JCORE_SENUM_BEGIN(JumpDirection)
None,
Up = Direction::Up,
Down
JCORE_SENUM_END(JumpDirection)

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


JCORE_SENUM_BEGIN(VisualType)
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


// 인덱스 기반 타입이므로 무조건 0부터 시작해야한다.
// 변수 초기화는 None으로 수동으로 할 것
JCORE_SENUM_BEGIN(ControlKey)
Right		,
Left		,
Up			,
Down		,
Attack		,
Jump		,
Max			,
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

static constexpr bool IsMoveKey[Max] {
	true, true, true, true, false, false
};

static constexpr ControlKey_t ReverseDirection[Direction::Max]{
	Left,
	Right,
	Down,
	Up
};

JCORE_SENUM_MIDDLE_END(ControlKey)



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


JCORE_SENUM_BEGIN(FrameEventType)
None,
Spawn,
AttackBoxInstant,
Max
JCORE_SENUM_END(FrameEventType)

JCORE_SENUM_BEGIN(FrameEventSpawnType)
Projectile,
AttackBox,
Max
JCORE_SENUM_END(FrameEventSpawnType)

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

JCORE_SENUM_MIDDLE_END(WeaponType)



JCORE_SENUM_BEGIN(AttackDamageType)
None,
Physics,
Magic,
Fixed,
Max
JCORE_SENUM_END(AttackDamageType)

JCORE_SENUM_BEGIN(AttackXForceDirection)
None,
Forward,
Backward,
Spread,
Max
JCORE_SENUM_END(AttackXForceDirection)


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

JCORE_SENUM_BEGIN(AIState)
None,
Wander,
Track,
Angry
JCORE_SENUM_END(AIState)

JCORE_SENUM_BEGIN(AIWanderDecision)
Walk,
Idle,
Max
JCORE_SENUM_END(AIWanderDecision)

JCORE_SENUM_BEGIN(AITrackDecision)
Wander,
Attack,
Follow,
Max
JCORE_SENUM_END(AITrackDecision)

JCORE_SENUM_BEGIN(AIAngryDecision)
Wander,
Track,
Attack,
Max
JCORE_SENUM_END(AIAngryDecision)


JCORE_SENUM_BEGIN(AIActivityType)
Idle,
Walk,
Attack,
Hit,
FallDown,
Sit,
// TODO: Custom 몬스터 특수행동 그런거 추가할 수 있을 듯?
Max,
None
JCORE_SENUM_MIDDLE(AIActivityType)
static constexpr const char* Name[Max]{
	"Idle",
	"Walk",
	"Attack",
	"Hit",
	"FallDown",
	"Sit"
};

JCORE_SENUM_MIDDLE_END(AIActivityType)

JCORE_SENUM_BEGIN(SceneType)
Login,
ChannelSelect,
Game,
None,
Max = None
JCORE_SENUM_MIDDLE(SceneType)
static constexpr const char* Name[Max]{
	"로그인",
	"채널 선택",
	"게임",
};
JCORE_SENUM_MIDDLE_END(SceneType)

JCORE_SENUM_BEGIN(HAlignment)
Left,
Center,
Right
JCORE_SENUM_END(HAlignment)

JCORE_SENUM_BEGIN(VAlignment)
Top,
Center,
Bottom
JCORE_SENUM_END(VAlignment)

JCORE_SENUM_BEGIN(UIElementType)
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

JCORE_SENUM_MIDDLE(UIElementType)

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


JCORE_SENUM_MIDDLE_END(UIElementType)


JCORE_SENUM_BEGIN(ConfigFileType)
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
JCORE_SENUM_MIDDLE(ConfigFileType)

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
JCORE_SENUM_MIDDLE_END(ConfigFileType)


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

JCORE_SENUM_BEGIN(GameServerType)
Begin,
Luke = Begin,
Hilder,
Siroco,
Prey,
Kasias,		// 대충 발음나는데..
Diregie,
Cain,
Seria,
End = Seria,
Max
JCORE_SENUM_MIDDLE(GameServerType)

static constexpr const char* Name[Max]{
	"루크",
	"힐더",
	"시로코",
	"프레이",
	"카시아스",
	"디레지에",
	"카인",
	"세리아"
};
JCORE_SENUM_MIDDLE_END(GameServerType)

JCORE_SENUM_BEGIN(EnteranceType)
Newbie,
Begin = Newbie,
Auction,
Normal,
End = Normal,
Max
JCORE_SENUM_MIDDLE(EnteranceType)

JCORE_SENUM_MIDDLE_END(EnteranceType)



JCORE_SENUM_BEGIN(ChannelDensity)
Free,
Normal,
Busy,
Full,
Max
JCORE_SENUM_MIDDLE(ChannelDensity)

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

JCORE_SENUM_MIDDLE_END(ChannelDensity)

JCORE_SENUM_BEGIN(ProgressIncreaseDirection)
LeftRight,
RightLeft,
TopBottom,
BottomTop
JCORE_SENUM_END(ProgressIncreaseDirection)

JCORE_SENUM_BEGIN(DatabaseType)
Game = 1
JCORE_SENUM_END(DatabaseType)

JCORE_SENUM_BEGIN(ServerProcessType)
None,
Auth,
Begin = Auth,
Center,
Lobby,
Game,
End = Game
JCORE_SENUM_MIDDLE(ServerProcessType)
static constexpr int Count = End;
static constexpr const char* Name[Count + 1]{
	"없음",
	"인증",
	"중앙",
	"로비",
	"게임",
};

JCORE_SENUM_MIDDLE_END(ServerProcessType)

// 클라이언트가 게임을 플레이하는 동안 한번이라도 접속하게되는 서버타입
JCORE_SENUM_BEGIN(ClientConnectServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
End = Area,
Max
JCORE_SENUM_END(ClientConnectServerType)

// 전체 서버 종류
JCORE_SENUM_BEGIN(ServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
Center,
End = Center,
Max
JCORE_SENUM_MIDDLE(ServerType)

static constexpr const char* Name[Max]{
	"인증",
	"로비",
	"로직",
	"채팅",
	"지역",
	"중앙"
};

JCORE_SENUM_MIDDLE_END(ServerType)


JCORE_SENUM_BEGIN(MapObjectType)
Obstacle,
Begin = Obstacle,
Gate,
End = Gate,
JCORE_SENUM_END(MapObjectType)

JCORE_SENUM_BEGIN(MapType)
Dungeon,
Private,
Town
JCORE_SENUM_END(MapType)

JCORE_SENUM_BEGIN(MapBundleType)
Dungeon,
Town
JCORE_SENUM_END(MapBundleType)


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
JCORE_SENUM_BEGIN(ActorPartSpritePositioningRule)
InFrameSize,		// 캐릭터 모션
InCustomFrameSize,	// 맵 오브젝트(게이트, 장애물)
InIgnoredFrameSize	// 프로젝틸
JCORE_SENUM_END(ActorPartSpritePositioningRule)


JCORE_SENUM_BEGIN(GamePlayMode)
Single,
Lan,
Wan,
Max
JCORE_SENUM_MIDDLE(GamePlayMode)

static constexpr bool IsNetworkMode[Max]{
	false,
	true,
};

JCORE_SENUM_MIDDLE_END(GamePlayMode)


JCORE_SENUM_BEGIN(AuthenticationState)
LobbyWait,
Lobby,
GameWait,
Game,
Max
JCORE_SENUM_MIDDLE(AuthenticationState)

static constexpr const char* Name[Max]{
	"LobbyWait",
	"Lobby",
	"GameWait",
	"Game"
};

JCORE_SENUM_MIDDLE_END(AuthenticationState)


JCORE_SENUM_BEGIN(SingleServerType)
Center,
Begin = Center,
Auth,
Lobby,
End = Lobby,
Max
JCORE_SENUM_END(SingleServerType)