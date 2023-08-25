/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Common/SgaSpriteRect.h>

#include <JCore/Time.h>
#include <JCore/Container/Vector.h>
#include <JCore/Primitives/StaticString.h>



template <Int32U Size>
using SGStaticString = JCore::StaticString<Size>;
using SGString = JCore::String;
using SGDateTime = JCore::DateTime;

struct ThicknessBox
{
	float RelativeX;
	float RelativeY;
	float Width;
	float Height;

	ThicknessBox() : RelativeX(0), RelativeY(0), Width(0), Height(0) {}
	ThicknessBox(float relaitveX, float relativeY, float width, float height)
		: RelativeX(relaitveX)
		, RelativeY(relativeY)
		, Width(width)
		, Height(height) {}
};


struct FrameInfo
{
	int FrameIndex;
	float Delay;
	int FrameEventCode;
};


struct AnimationInfo : ConfigDataAbstract
{
	AnimationInfo() { DebugAssertMsg(false, "호출 금지"); }
	AnimationInfo(int frameSize)
		: Loop(false)
		, Name{ 0 }
		, Frames(frameSize)
	{}

	bool Loop;
	SGString Name;
	SGVector<FrameInfo> Frames;
};

struct ActorPartSpriteData
{
	ActorPartSpriteData();
	ActorPartSpriteData(int zorder, int sga, int img);

	// 스프라이트 정보(필수)
	int SgaIndex;
	int ImgIndex;

	// 미입력시 순서대로 1씩증가
	int ZOrder;
	SgaSpriteRect CustomSizeInfo;
};

//struct ActorPartSpriteDataCustom : ActorPartSpriteData
//{
//	ActorPartSpriteDataCustom();
//	ActorPartSpriteDataCustom(int zorder, int sga, int img);
//};

struct ActorSpriteData
{
	ActorSpriteData(ActorPartSpritePositioningRule_t positioningRule, int partCount, int animationCount)
		: PositioningRule(positioningRule)
		, Parts(partCount)
		, Animations(animationCount) {}

	ActorPartSpritePositioningRule_t PositioningRule;
	SGVector<ActorPartSpriteData> Parts;
	SGVector<AnimationInfo> Animations;
};

using ActorSpriteDataPtr = JCore::SharedPtr<ActorSpriteData>;

// 캐릭터의 비주얼 아이템(아바타 혹은 무기)는 여러 조합을 만들기위해 하나의 부위가 여러개의 쉐이프(그냥 대충 지은 이름)로 구성된다.
// 그래서 모든 아바타+무기는 각각 최대 3개씩의 파츠 데이터를 가질 수 있도록 구성하였다.
// 예를들어 거너 아바타 하의는 2개의 파츠 데이터로 구성되었다고하자.
// shape: 2203, shape_alpha: ab라고 하면
// pants_2203a, pants_2203b 이미지팩 각각의 ActorPartSpriteData 2개를 뭉뚱그려서 VisualData라 명명하였다.
using VisualData = ActorPartSpriteData[Const::Visual::MaxShapeCount];
using VisualInfo = JCore::Vector<ActorPartSpriteData>;


union SgaResourceIndex
{
	SgaResourceIndex();
	SgaResourceIndex(int sgaIndex, int imgIndex, int frameIndex)
		: Un{ sgaIndex, imgIndex, frameIndex }
	{}

	struct
	{
		int SgaIndex : 9;	// 512
		int ImgIndex : 11;	// 2048
		int FrameIndex : 12;	// 4096

	} Un;

	SGString ToString() const;
	Int32U Value;
};

using AuthenticationSerial_t = Int32;
struct AuthenticationData
{
	AuthenticationState_t State;
	AuthenticationSerial_t Serial;				// 토큰 데이터
	SGStaticString<Const::StringLen::AccountId> AccountId;
	SGDateTime TimeId;							// 최신화된 시각 및 고유 시각ID
};

struct AccountData
{
	AccountData();

	void Clear();

	SGStaticString<Const::StringLen::AccountId> Id;
	SGStaticString<Const::StringLen::AccountPass> Pass;
	SGDateTime LastLogin;
	GameServerType_t LastServer;
};

struct PlayerData
{
	PlayerData();

	void clear();

	int CharId;	// 캐릭터 고유 아이디
	SGStaticString<Const::StringLen::CharacterName> Name;
	CharType_t CharType;
	Int32 Life;
	Int32 MaxLife;
	Int32 Mana;
	Int32 MaxMana;
	Int32 Level;

	// 호스트 플레이어 데이터
	Int64 Gold;
	Int32 Str;
	Int32 Dex;
	Int32 Int;
	Int32 Vit;
};



// 31                               0
// +-------+-------+-------+--------+
// |  Type |Detail2|Detail1|  Code  |
// +-------+-------+-------+--------+

union ItemCode
{
	inline static constexpr int BitCode = 16;	// 65535
	inline static constexpr int BitDetail1 = 6;		// 64
	inline static constexpr int BitDetail2 = 4;		// 16
	inline static constexpr int BitType = 6;		// 64

	inline static constexpr int BitCodeShift = 0;
	inline static constexpr int BitDetail1Shift = 16;
	inline static constexpr int BitDetail2Shift = 22;
	inline static constexpr int BitTypeShift = 26;

	inline static constexpr int BitCodeMask = 0x0000ffff;
	inline static constexpr int BitDetail1Mask = JCore::FillBitRight32<22>() & ~0x0000ffff;
	inline static constexpr int BitDetail2Mask = JCore::FillBitRight32<26>() & ~JCore::FillBitRight32<22>();
	inline static constexpr int BitTypeMask = 0xffffffff & JCore::FillBitRight32<26>();


	ItemCode() : Code(InvalidValue_v) {}
	ItemCode(int Total);	// 얘는 전체 값
	ItemCode(int Code, ItemType_t Type);
	ItemCode(int Code, int Detail1, ItemType_t Type);
	ItemCode(int Code, int Detail1, int Detail2, ItemType_t Type);

	void initAvatarCode(CharType_t charType, AvatarType_t avatarType, int code);
	void initWeaponCode(CharType_t charType, WeaponType_t weaponType, int code);
	void initArmorCode(ItemType_t itemType, EquipArmorType_t armorType, int code);

	// 이렇게 유니온 많이 달아도 일반 유니온 1개짜리랑 성능차이가 있을려나?
	// TODO: 어셈코드 확인해볼 것
	//		TODO: 코드가 다르면 벤치 테스트 해볼 것

	struct
	{
		int Code : BitCode;
		int Detail1 : BitDetail1;		// 아바타 부위 || 갑옷 종류 || 무기 종류
		int Detail2 : BitDetail2;		// 캐릭터 타입
		ItemType_t Type : BitType;			// 소모품, 아바타, 무기
	} CommonUn;

	struct
	{
		int Code : BitCode;
		AvatarType_t PartType : BitDetail1;
		CharType_t CharType : BitDetail2;
		ItemType_t ItemType : BitType;
	} AvatarUn;

	struct
	{
		int Code : BitCode;
		WeaponType_t WeaponType : BitDetail1;
		CharType_t CharType : BitDetail2;
		ItemType_t ItemType : BitType;
	} WeaponUn;

	struct
	{
		int Code : BitCode;
		EquipArmorType_t ArmorType : BitDetail1;
		int _ : BitDetail2;
		ItemType_t ItemType : BitType;
	} ArmorUn;


	int Code;
};

struct ItemOpt
{
	ItemOpt() : Code(InvalidValue_v) {}

	int Code{};
};

struct ItemOptVal : ItemOpt
{
	int Value{};
};

struct ItemOptRangeVal : ItemOpt
{
	int MinValue{};
	int MaxValue{};
};



struct InvenItem
{
	ItemCode Code;
	int Quantity;
};

struct InvenItemEquip : InvenItem
{
	int ArmorPhisical;
	int ArmorMagic;
	int AttackPhysical;
	int AttackMagic;

	int OptCount;
	ItemOptVal Opt[Const::Item::MaxOptCount];
};

struct LobbyChannelInfo
{
	int Type;
	int Number;
	ChannelDensity_t Desity;
};

