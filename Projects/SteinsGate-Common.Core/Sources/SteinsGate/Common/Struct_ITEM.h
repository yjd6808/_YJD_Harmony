/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:57:42 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Bit.h>

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>


// 31                               0
// +-------+-------+-------+--------+
// |  Type |Detail2|Detail1|  Code  |
// +-------+-------+-------+--------+

union ItemCode
{
	inline static constexpr int BIT_CODE = 16; // 65535
	inline static constexpr int BIT_DETAIL1 = 6; // 64
	inline static constexpr int BIT_DETAIL2 = 4; // 16
	inline static constexpr int BIT_TYPE = 6; // 64

	inline static constexpr int BIT_CODE_SHIFT = 0;
	inline static constexpr int BIT_DETAIL1_SHIFT = 16;
	inline static constexpr int BIT_DETAIL2_SHIFT = 22;
	inline static constexpr int BIT_TYPE_SHIFT = 26;

	inline static constexpr int BIT_CODE_MASK = 0x0000ffff;
	inline static constexpr int BIT_DETAIL1_MASK = JCore::FillBitRight32<22>() & ~0x0000ffff;
	inline static constexpr int BIT_DETAIL2_MASK = JCore::FillBitRight32<26>() & ~JCore::FillBitRight32<22>();
	inline static constexpr int BIT_TYPE_MASK = 0xffffffff & JCore::FillBitRight32<26>();


	ItemCode()
	: Code(InvalidValue_v)
	{
	}

	ItemCode(int _total); // 얘는 전체 값
	ItemCode(int _code, ItemType_t _type);
	ItemCode(int _code, int _detail1, ItemType_t _type);
	ItemCode(int _code, int _detail1, int _detail2, ItemType_t _type);

	void InitAvatarCode(CharType_t _charType, AvatarType_t _avatarType, int _code);
	void InitWeaponCode(CharType_t _charType, WeaponType_t _weaponType, int _code);
	void InitArmorCode(ItemType_t _itemType, EquipArmorType_t _armorType, int _code);

	// 이렇게 유니온 많이 달아도 일반 유니온 1개짜리랑 성능차이가 있을려나?
	// TODO: 어셈코드 확인해볼 것
	//		TODO: 코드가 다르면 벤치 테스트 해볼 것

	struct
	{
		int code_ : BIT_CODE;
		int detail1_ : BIT_DETAIL1; // 아바타 부위 || 갑옷 종류 || 무기 종류
		int detail2_ : BIT_DETAIL2; // 캐릭터 타입
		ItemType_t type_ : BIT_TYPE; // 소모품, 아바타, 무기
	} commonUn_;

	struct
	{
		int code_ : BIT_CODE;
		AvatarType_t partType_ : BIT_DETAIL1;
		CharType_t charType_ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} avatarUn_;

	struct
	{
		int code_ : BIT_CODE;
		WeaponType_t weaponType_ : BIT_DETAIL1;
		CharType_t charType_ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} weaponUn_;

	struct
	{
		int code_ : BIT_CODE;
		EquipArmorType_t armorType_ : BIT_DETAIL1;
		int _ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} armorUn_;


	int Code;
};

struct ItemOpt
{
	ItemOpt()
	: code_(InvalidValue_v)
	{
	}

	int code_{};
};

struct ItemOptVal : ItemOpt
{
	int value_{};
};

struct ItemOptRangeVal : ItemOpt
{
	int minValue_{};
	int maxValue_{};
};


struct InvenItem
{
	ItemCode code_;
	int quantity_;
};

struct InvenItemEquip : InvenItem
{
	int armorPhisical_;
	int armorMagic_;
	int attackPhysical_;
	int attackMagic_;

	int optCount_;
	ItemOptVal opt_[Const::Item::MaxOptCount];
};
