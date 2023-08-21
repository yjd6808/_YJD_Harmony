/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:28 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ItemOptInfo.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct EnchantBonusOptInfo
{
	EnchantBonusOptInfo() : Opt(nullptr), Value{} {}
	EnchantBonusOptInfo(const EnchantBonusOptInfo& other) { this->operator=(other); }
	
	EnchantBonusOptInfo& operator=(const EnchantBonusOptInfo& other) {
		DebugAssertMsg(this != &other, "셀프 어사인 하지마!");
		Opt = other.Opt;
		SGMemory::CopyUnsafe(Value, other.Value, sizeof(int) * Const::Item::MaxEnchantLevel);
		return *this;
	}


	ItemOptInfo* Opt;
	int Value[Const::Item::MaxEnchantLevel];
};

struct EnchantInfo : ConfigDataAbstract
{
	~EnchantInfo() override = default;

	int ArmorBonusOptCount;
	int AccessoryBonusOptCount;
	EnchantBonusOptInfo ArmorBonusOptList[Const::Item::ArmorEnchantBonusOptCount];
	EnchantBonusOptInfo AccessoryBonusOptList[Const::Item::ArmorEnchantBonusOptCount];

	float WeaponAttackPhysicalMultiplyValue[Const::Item::MaxEnchantLevel];
	float ArmorRarityConstant[RarityType::Max];
	float AccesorryRarityConstant[RarityType::Max];
	float WeaponRarityConstant[RarityType::Max];
	float EnchangeProbs[Const::Item::MaxEnchantLevel];
	float SellBonus[Const::Item::MaxEnchantLevel];
};

