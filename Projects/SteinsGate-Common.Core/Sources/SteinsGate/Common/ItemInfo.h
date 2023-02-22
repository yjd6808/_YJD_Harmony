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

	SGString Name;
	SGString NameEng;
	RarityType_t Rarity;

	int Icon;
	int SellPrice;

	ItemType_t getItemType() {
		return ItemCode(Code).CommonUn.Type;
	}
};

struct ItemStackableInfo : ItemInfo
{
	~ItemStackableInfo() override = default;

	int MaxStack;
};

struct ItemConsumeInfo : ItemStackableInfo
{
	~ItemConsumeInfo() override = default;

	int ReqLv;
	int Buff;
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
	ItemEquipInfo() : ReqLv(1), SetCode(InvalidValue_v) {}
	~ItemEquipInfo() override = default;

	int ReqLv;
	int SetCode;

	
};

struct ItemArmorInfo : ItemEquipInfo
{
	~ItemArmorInfo() override = default;

	int ArmorPhysical;
	int ArmorMagical;

	EquipArmorType_t getArmorType() {
		return ItemCode(Code).ArmorUn.ArmorType;
	}

};

struct ItemVisualInfo : ItemEquipInfo
{
	~ItemVisualInfo() override = default;

	SGString Shape;
	SGString ShapeAlpha;

	CharType_t getCharType() {
		return ItemCode(Code).AvatarUn.CharType;
	}

	AvatarType_t getAvatarType() {
		return ItemCode(Code).AvatarUn.PartType;
	}

	WeaponType_t getWeaponType() {
		return ItemCode(Code).WeaponUn.WeaponType;
	}

	VisualType_t getVisualType() {

		ItemCode c(Code);

		if (c.CommonUn.Type == ItemType::Avatar) {
			return (VisualType_t)c.AvatarUn.PartType;
		}

		if (c.CommonUn.Type == ItemType::Weapon) {
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

	int AttackPhysical;
	int AttackMagic;
};




