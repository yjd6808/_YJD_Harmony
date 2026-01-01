/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>

struct ItemInfo : SDescBase
{
	~ItemInfo() override = default;

	jc::String name_;
	jc::String nameEng_;
	RarityType_t rarity_;

	int icon_;
	int sellPrice_;

	ItemType_t GetItemType()
	{
		return ItemCode(code_).commonUn_.type_;
	}
};

struct ItemStackableInfo : ItemInfo
{
	~ItemStackableInfo() override = default;

	int maxStack_;
};

struct ItemConsumeInfo : ItemStackableInfo
{
	~ItemConsumeInfo() override = default;

	int reqLv_;
	int buff_;
};

struct ItemEtcInfo : ItemStackableInfo
{
	~ItemEtcInfo() override = default;
};

struct ItemQuestInfo : ItemStackableInfo
{
	~ItemQuestInfo() override = default;
};

struct ItemEquipInfo : ItemInfo
{
	ItemEquipInfo()
	: reqLv_(1)
	, setCode_(InvalidValue_v)
	{
	}

	~ItemEquipInfo() override = default;

	int reqLv_;
	int setCode_;
};

struct ItemArmorInfo : ItemEquipInfo
{
	~ItemArmorInfo() override = default;

	int armorPhysical_;
	int armorMagical_;

	EquipArmorType_t GetArmorType()
	{
		return ItemCode(code_).armorUn_.armorType_;
	}
};

struct ItemVisualInfo : ItemEquipInfo
{
	~ItemVisualInfo() override = default;

	jc::String shape_;
	jc::String shapeAlpha_;

	CharType_t GetCharType()
	{
		return ItemCode(code_).avatarUn_.charType_;
	}

	AvatarType_t GetAvatarType()
	{
		return ItemCode(code_).avatarUn_.partType_;
	}

	WeaponType_t GetWeaponType()
	{
		return ItemCode(code_).weaponUn_.weaponType_;
	}

	VisualType_t GetVisualType()
	{
		ItemCode itemCode(code_);

		if (itemCode.commonUn_.type_ == ItemType::Avatar)
		{
			return (VisualType_t)itemCode.avatarUn_.partType_;
		}

		if (itemCode.commonUn_.type_ == ItemType::Weapon)
		{
			return VisualType::Weapon;
		}

		jc_assert_msg(false, "비주얼 타입이 아닌 녀석이 비주얼 타입으로 설정되어있습니다.");
		return (VisualType_t)-1;
	}
};

struct ItemAvatarInfo : ItemVisualInfo
{
	~ItemAvatarInfo() override = default;
};

struct ItemWeaponInfo : ItemVisualInfo
{
	~ItemWeaponInfo() override = default;

	int attackPhysical_;
	int attackMagic_;
};
