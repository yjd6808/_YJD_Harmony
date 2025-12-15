/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ItemInfo : ConfigDataAbstract
{
	~ItemInfo() override = default;

	SGString name_;
	SGString nameEng_;
	RarityType_t rarity_;

	int icon_;
	int sellPrice_;

	ItemType_t GetItemType()
	{
		return ItemCode(code_).CommonUn.Type;
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
		return ItemCode(code_).ArmorUn.ArmorType;
	}
};

struct ItemVisualInfo : ItemEquipInfo
{
	~ItemVisualInfo() override = default;

	SGString shape_;
	SGString shapeAlpha_;

	CharType_t GetCharType()
	{
		return ItemCode(code_).AvatarUn.CharType;
	}

	AvatarType_t GetAvatarType()
	{
		return ItemCode(code_).AvatarUn.PartType;
	}

	WeaponType_t GetWeaponType()
	{
		return ItemCode(code_).WeaponUn.WeaponType;
	}

	VisualType_t GetVisualType()
	{
		ItemCode itemCode(code_);

		if (itemCode.CommonUn.Type == ItemType::Avatar)
		{
			return (VisualType_t)itemCode.AvatarUn.PartType;
		}

		if (itemCode.CommonUn.Type == ItemType::Weapon)
		{
			return VisualType::Weapon;
		}

		DebugAssertMsg(false, "비주얼 타입이 아닌 녀석이 비주얼 타입으로 설정되어있습니다.");
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
