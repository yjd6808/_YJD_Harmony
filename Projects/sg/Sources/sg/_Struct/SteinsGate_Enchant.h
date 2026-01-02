/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:28 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Struct/SteinsGate_ItemOpt.h>
#include <sg/_Util/DescBase.h>

//////////////////////////////////////////////////////////////////////////////////////////
struct EnchantBonusOptInfo
{
	EnchantBonusOptInfo()
	: opt_(nullptr)
	, value_{}
	{}

	EnchantBonusOptInfo(const EnchantBonusOptInfo& _other);
	EnchantBonusOptInfo& operator=(const EnchantBonusOptInfo& _other);
	ItemOptInfo* opt_;
	int value_[Const::Item::MaxEnchantLevel];
};

//////////////////////////////////////////////////////////////////////////////////////////
struct EnchantInfo : SDescBase
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
