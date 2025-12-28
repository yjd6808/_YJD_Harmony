/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:55 AM
 * =====================
 *
 */


#include "Core.h"
#include "EnchantInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>


USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
EnchantInfoLoader::EnchantInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool EnchantInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Json::Value& enchantInfoListRoot = root["enchant"];

		for (int i = 0; i < enchantInfoListRoot.size(); ++i)
		{
			Value& enchantInfoRoot = enchantInfoListRoot[i];
			EnchantInfo* pEnchantInfo = dbg_new EnchantInfo();
			ReadEnchantInfo(enchantInfoRoot, pEnchantInfo);
			AddData(pEnchantInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void EnchantInfoLoader::ReadEnchantInfo(Json::Value& _enchantRoot, OUT EnchantInfo* _pEnchantInfo)
{
	_pEnchantInfo->code_ = _enchantRoot["code"].asInt();

	Value& armorBonusOptListRoot = _enchantRoot["armor_bonus_opt"];
	Value& accessoryBonusOptListRoot = _enchantRoot["accessory_bonus_opt"];

	int armorBonusOptCount = (int)armorBonusOptListRoot.size();
	int accessoryBonusOptCount = (int)accessoryBonusOptListRoot.size();

	DebugAssertMsg(armorBonusOptCount > 0, "아머 보너스 옵트가 없습니다.");
	DebugAssertMsg(accessoryBonusOptCount > 0, "악세 보너스 옵트가 없습니다.");

	_pEnchantInfo->armorBonusOptCount_ = armorBonusOptCount;
	_pEnchantInfo->accessoryBonusOptCount_ = accessoryBonusOptCount;

	for (int i = 0; i < armorBonusOptCount; ++i)
	{
		Value& armorBonusOptRoot = armorBonusOptListRoot[i];

		SGString optName = JsonUtil::GetString(armorBonusOptRoot["opt_name"]);
		Value& optValListRoot = armorBonusOptRoot["opt_val"];
		DebugAssertMsg(optValListRoot.size() == Const::Item::MaxEnchantLevel, "방어구 보너스 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)",
		               optValListRoot.size(), Const::Item::MaxEnchantLevel);

		_pEnchantInfo->armorBonusOptList_[i].opt_ = pManager_->GetItemOptInfo(optName);

		for (int j = 0; j < Const::Item::MaxEnchantLevel; j++)
		{
			_pEnchantInfo->armorBonusOptList_[i].value_[j] = optValListRoot[j].asInt();
		}
	}

	for (int i = 0; i < accessoryBonusOptCount; ++i)
	{
		Value& accessoryBonusOptRoot = accessoryBonusOptListRoot[i];

		SGString optName = JsonUtil::GetString(accessoryBonusOptRoot["opt_name"]);
		Value& optValListRoot = accessoryBonusOptRoot["opt_val"];
		DebugAssertMsg(optValListRoot.size() == Const::Item::MaxEnchantLevel, "악세서리 보너스 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)",
		               optValListRoot.size(), Const::Item::MaxEnchantLevel);

		_pEnchantInfo->accessoryBonusOptList_[i].opt_ = pManager_->GetItemOptInfo(optName);

		for (int j = 0; j < Const::Item::MaxEnchantLevel; j++)
		{
			_pEnchantInfo->accessoryBonusOptList_[i].value_[j] = optValListRoot[j].asInt();
		}
	}

	Value& weaponBonusMulitplyValueListRoot = _enchantRoot["weapon_bonus_opt_val"];
	Value& probListRoot = _enchantRoot["probs"];
	Value& sellBonusListRoot = _enchantRoot["sell_bonus"];

	Value& weaponRarityConstantListRoot = _enchantRoot["rarity_weapon_constant"];
	Value& armorRarityConstantListRoot = _enchantRoot["rarity_armor_constant"];
	Value& accessoryRarityConstantListRoot = _enchantRoot["rarity_accessory_constant"];

	int weaponBonusOptValueCount = (int)weaponBonusMulitplyValueListRoot.size();
	int probCount = (int)probListRoot.size();
	int sellBonusCount = (int)sellBonusListRoot.size();

	int weaponRarityConstantCount = (int)weaponRarityConstantListRoot.size();
	int armorRarityConstantCount = (int)armorRarityConstantListRoot.size();
	int accessoryRarityConstantCount = (int)accessoryRarityConstantListRoot.size();

	DebugAssertMsg(weaponBonusOptValueCount == Const::Item::MaxEnchantLevel, "무기 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)",weaponBonusOptValueCount, Const::Item::MaxEnchantLevel);
	DebugAssertMsg(probCount == Const::Item::MaxEnchantLevel, "강화 확률 값이 %d개 입니다. (%d개로 맞춰야함)", probCount,Const::Item::MaxEnchantLevel);
	DebugAssertMsg(sellBonusCount == Const::Item::MaxEnchantLevel, "판매 보너스 값이 %d개 입니다. (%d개로 맞춰야함)", sellBonusCount, Const::Item::MaxEnchantLevel);
	DebugAssertMsg(weaponRarityConstantCount == RarityType::Max, "무기 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)", weaponBonusOptValueCount, RarityType::Max);
	DebugAssertMsg(armorRarityConstantCount == RarityType::Max, "강화 확률 값이 %d개 입니다. (%d개로 맞춰야함)", probCount, RarityType::Max);
	DebugAssertMsg(accessoryRarityConstantCount == RarityType::Max, "판매 보너스 값이 %d개 입니다. (%d개로 맞춰야함)", sellBonusCount, RarityType::Max);

	for (int i = 0; i < Const::Item::MaxEnchantLevel; ++i)
	{
		_pEnchantInfo->weaponAttackPhysicalMultiplyValue_[i] = weaponBonusMulitplyValueListRoot[i].asFloat();
		_pEnchantInfo->enchangeProbs_[i] = probListRoot[i].asFloat();
		_pEnchantInfo->sellBonus_[i] = sellBonusListRoot[i].asFloat();
	}

	for (int i = 0; i < RarityType::Max; ++i)
	{
		_pEnchantInfo->armorRarityConstant_[i] = weaponRarityConstantListRoot[i].asFloat();
		_pEnchantInfo->accesorryRarityConstant_[i] = armorRarityConstantListRoot[i].asFloat();
		_pEnchantInfo->weaponRarityConstant_[i] = accessoryRarityConstantListRoot[i].asFloat();
	}
}
