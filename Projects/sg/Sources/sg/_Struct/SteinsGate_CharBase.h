/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 12:21:52 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>
#include <sg/_Struct/SteinsGate_Server.h>
#include <sg/_Sga/SgaSpriteRect.h>

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
JC_SENUM_BEGIN(FrameEventType)
	None,
	Spawn,
	AttackBoxInstant,
	Max
JC_SENUM_END(FrameEventType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(FrameEventSpawnType)
	Projectile,
	AttackBox,
	Max
JC_SENUM_END(FrameEventSpawnType)


//////////////////////////////////////////////////////////////////////////////////////////
/*
 * TODO: ActorSpriteData가 아닌 ActorPartSpriteData의 멤버로 오는게 올바르긴하다. 이 TODO는 JsonUtil::parseActorSpriteData의 TODO를 처리할 때 알아서 해결됨.
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
JC_SENUM_BEGIN(ActorPartSpritePositioningRule)
	InFrameSize, // 캐릭터 모션
	InCustomFrameSize, // 맵 오브젝트(게이트, 장애물)
	InIgnoredFrameSize // 프로젝틸
JC_SENUM_END(ActorPartSpritePositioningRule)


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
struct FrameInfo
{
	int frameIndex_;
	float delay_;
	int frameEventCode_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct AnimationInfo : SDescBase
{
	AnimationInfo()
	{
		jc_assert_msg(false, "호출 금지");
	}

	AnimationInfo(int _frameSize)
	: loop_(false)
	, name_{ 0 }
	, frames_(_frameSize)
	{
	}

	bool loop_;
	jc::String name_;
	jc::Vector<FrameInfo> frames_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ThicknessBox
{
	float relativeX_;
	float relativeY_;
	float width_;
	float height_;

	ThicknessBox()
		: relativeX_(0)
		, relativeY_(0)
		, width_(0)
		, height_(0)
	{
	}

	ThicknessBox(float _relativeX, float _relativeY, float _width, float _height)
		: relativeX_(_relativeX)
		, relativeY_(_relativeY)
		, width_(_width)
		, height_(_height)
	{
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ActorRect
{
	cc::rect thicknessRect_;
	cc::rect bodyRect_;

	bool IsCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, cc::rect& _hitRect);
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ActorPartSpriteData
{
	ActorPartSpriteData();
	ActorPartSpriteData(int _zOrder, int _sga, int _img);

	// 스프라이트 정보(필수)
	int sgaIndex_;
	int imgIndex_;

	// 미입력시 순서대로 1씩증가
	int zOrder_;
	SgaSpriteRect customSizeInfo_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ActorSpriteData
{
	ActorSpriteData(ActorPartSpritePositioningRule_t _positioningRule, int _partCount, int _animationCount)
		: positioningRule_(_positioningRule)
		, parts_(_partCount)
		, animations_(_animationCount)
	{
	}

	ActorPartSpritePositioningRule_t positioningRule_;
	jc::Vector<ActorPartSpriteData> parts_;
	jc::Vector<AnimationInfo> animations_;
};

using ActorSpriteDataPtr = jc::SharedPtr<ActorSpriteData>;

// 캐릭터의 비주얼 아이템(아바타 혹은 무기)는 여러 조합을 만들기위해 하나의 부위가 여러개의 쉐이프(그냥 대충 지은 이름)로 구성된다.
// 그래서 모든 아바타+무기는 각각 최대 3개씩의 파츠 데이터를 가질 수 있도록 구성하였다.
// 예를들어 거너 아바타 하의는 2개의 파츠 데이터로 구성되었다고하자.
// shape: 2203, shape_alpha: ab라고 하면
// pants_2203a, pants_2203b 이미지팩 각각의 ActorPartSpriteData 2개를 뭉뚱그려서 VisualData라 명명하였다.
using VisualData = ActorPartSpriteData[Const::Visual::MaxShapeCount];
using VisualInfo = jc::Vector<ActorPartSpriteData>;

//////////////////////////////////////////////////////////////////////////////////////////
struct CharBaseInfo : SDescBase
{
	~CharBaseInfo() override = default;

	// 캐릭터 기본 정보
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

//////////////////////////////////////////////////////////////////////////////////////////
struct AccountData
{
	AccountData();

	void Clear();

	jc::StaticString<Const::StringLen::AccountId> id_;
	jc::StaticString<Const::StringLen::AccountPass> pass_;
	jc::DateTime lastLoginTime_;
	GameServerType_t lastServer_;
};

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AuthenticationState)
		Initialized,
		LobbyWait,
		Lobby,
		GameWait,
		Game
JC_SENUM_MIDDLE(AuthenticationState)
static constexpr const char* Name[Game + 1]{
	"Initialized",
	"LobbyWait",
	"Lobby",
	"GameWait",
	"Game"
};
JC_SENUM_MIDDLE_END(AuthenticationState)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(LoginResult)
	LoginSuccess, // 로그인 성공
	RegisterSuccess, // 회원가입 성공\n 다시 입력 후 로그인 해주세요.
	IdAlreadyExist, // 회원가입 실패\n 이미 입력하신 ID로 회원가입된 계정이 존재합니다.
	IdPasswordMismatch, // 아이디 또는 비밀번호가 일치하지 않음.
	Banned, // 정지된 계정입니다.\n정지일: yyyy-MM-dd HH:mm:ss\n해제일: yyyy-MM-dd HH:mm:ss
	Logined, // 현재 접속중인 계정입니다.
	QueryFailed, // 서버에서 쿼리 수행중 오류가 발생하였습니다.
	Max
JC_SENUM_END(LoginResult)

//////////////////////////////////////////////////////////////////////////////////////////
using AuthenticationSerial_t = _s32;
struct AuthenticationData
{
	AuthenticationState_t state_;
	AuthenticationSerial_t serial_; // 토큰 데이터
	jc::DateTime timeId_; // 최신화된 시각 및 고유 시각ID
	AccountData accountData_; // 처음 계정 접속시 DB로부터 불러왔던 계정 정보
};
