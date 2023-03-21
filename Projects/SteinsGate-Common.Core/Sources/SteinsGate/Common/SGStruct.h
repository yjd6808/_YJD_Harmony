/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGEnums.h>
#include <SteinsGate/Common/SGConfig.h>

#include <JCore/Time.h>
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

struct ActorPartSpriteData
{
	ActorPartSpriteData();
	ActorPartSpriteData(int zorder, int sga, int img);

	int ZOrder;
	int SgaIndex;
	int ImgIndex;
};

using VisualData = ActorPartSpriteData[MaxVisualCount_v];
using VisualInfo = JCore::Vector<ActorPartSpriteData>;


union SgaResourceIndex
{
	SgaResourceIndex();
	SgaResourceIndex(int sgaIndex, int imgIndex, int frameIndex)
		: Un{sgaIndex, imgIndex, frameIndex}
	{}

	struct
	{
		int SgaIndex	: 9;	// 512
		int ImgIndex	: 11;	// 2048
		int FrameIndex  : 12;	// 4096
		
	} Un;

	SGString ToString() const;
	Int32U Value;
};

struct AccountData
{
	AccountData();

	int AccountId;	// 계정 고유 아이디
	SGStaticString<AccountIdLen_v> Name;
	SGStaticString<AccountPassLen_v> Pass;
	SGDateTime LastLogin;
	int LastServer;
};

struct PlayerData
{
	PlayerData();

	int CharId;	// 캐릭터 고유 아이디
	SGStaticString<CharNameLen_v> Name;
	CharType_t CharType;
	int Gold;
	int Str;
	int Dex;
	int Int;
	int Vit;
	int Life;
	int Mana;
	int Level;
};


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
	inline static constexpr int BitDetail1Mask = JCore::FillBitRightInt32<22>() & ~0x0000ffff;
	inline static constexpr int BitDetail2Mask = JCore::FillBitRightInt32<26>() & ~JCore::FillBitRightInt32<22>();
	inline static constexpr int BitTypeMask = 0xffffffff & JCore::FillBitRightInt32<26>();


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
		int Code		 : BitCode;
		int Detail1		 : BitDetail1;		// 아바타 부위 || 갑옷 종류 || 무기 종류
		int Detail2		 : BitDetail2;		// 캐릭터 타입
		ItemType_t Type	 : BitType;			// 소모품, 아바타, 무기
	} CommonUn;

	struct
	{
		int Code				: BitCode;
		AvatarType_t PartType	: BitDetail1;	
		CharType_t CharType		: BitDetail2;	
		ItemType_t ItemType		: BitType;		
	} AvatarUn;

	struct
	{
		int Code					: BitCode;
		WeaponType_t WeaponType		: BitDetail1;	
		CharType_t CharType			: BitDetail2;	
		ItemType_t ItemType			: BitType;
	} WeaponUn;

	struct
	{
		int Code					: BitCode;
		EquipArmorType_t ArmorType	: BitDetail1;	
		int _						: BitDetail2;	
		ItemType_t ItemType			: BitType;		
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
	ItemOptVal Opt[MaxOptCount_v];
};



