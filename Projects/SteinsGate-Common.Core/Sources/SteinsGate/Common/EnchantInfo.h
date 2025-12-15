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
	EnchantBonusOptInfo()
	: opt_(nullptr)
	, value_{}
	{
	}

	EnchantBonusOptInfo(const EnchantBonusOptInfo& _other)
	{
		this->operator=(_other);
	}

	EnchantBonusOptInfo& operator=(const EnchantBonusOptInfo& _other)
	{
		DebugAssertMsg(this != &_other, "셀프 어사인 하지마!");
		opt_ = _other.opt_;
		SGMemory::CopyUnsafe(value_, _other.value_, sizeof(int) * Const::Item::MaxEnchantLevel);
		return *this;
	}

	ItemOptInfo* opt_;
	int value_[Const::Item::MaxEnchantLevel];
};

struct EnchantInfo : ConfigDataAbstract
{
	~EnchantInfo() override = default;

	int armorBonusOptCount_;
	int accessoryBonusOptCount_;
	EnchantBonusOptInfo armorBonusOptList_[Const::Item::ArmorEnchantBonusOptCount];
	EnchantBonusOptInfo accessoryBonusOptList_[Const::Item::ArmorEnchantBonusOptCount];

	float weaponAttackPhysicalMultiplyValue_[Const::Item::MaxEnchantLevel];
	float armorRarityConstant_[RarityType::Max];
	float accesorryRarityConstant_[RarityType::Max];
	float weaponRarityConstant_[RarityType::Max];
	float enchangeProbs_[Const::Item::MaxEnchantLevel];
	float sellBonus_[Const::Item::MaxEnchantLevel];
};
