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

EnchantInfoLoader::EnchantInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool EnchantInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& enchantInfoListRoot = root["enchant"];

		for (int i = 0; i < enchantInfoListRoot.size(); ++i) {
			Value& enchantInfoRoot = enchantInfoListRoot[i];
			EnchantInfo* pEnchantInfo = dbg_new EnchantInfo();
			readEnchantInfo(enchantInfoRoot, pEnchantInfo);
			addData(pEnchantInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void EnchantInfoLoader::readEnchantInfo(Json::Value& enchantRoot, JCORE_OUT EnchantInfo* enchantInfo) {
	enchantInfo->Code = enchantRoot["code"].asInt();

	Value& armorBonusOptListRoot = enchantRoot["armor_bonus_opt"];
	Value& accessoryBonusOptListRoot = enchantRoot["accessory_bonus_opt"];

	int iArmorBonusOptCount = int(armorBonusOptListRoot.size());
	int iAcceossoryBonusOptCount = int(accessoryBonusOptListRoot.size());

	DebugAssertMsg(iArmorBonusOptCount > 0, "아머 보너스 옵트가 없습니다.");
	DebugAssertMsg(iAcceossoryBonusOptCount > 0, "악세 보너스 옵트가 없습니다.");

	enchantInfo->ArmorBonusOptCount = iArmorBonusOptCount;
	enchantInfo->AccessoryBonusOptCount = iAcceossoryBonusOptCount;

	for (int i = 0; i < iArmorBonusOptCount; ++i) {
		Value& armorBonusOptRoot = armorBonusOptListRoot[i];

		SGString szOptName = JsonUtil::getString(armorBonusOptRoot["opt_name"]);
		Value& szOptValListRoot = armorBonusOptRoot["opt_val"];
		DebugAssertMsg(szOptValListRoot.size() == Const::Item::MaxEnchantLevel, "방어구 보너스 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)", szOptValListRoot.size(), Const::Item::MaxEnchantLevel);

		enchantInfo->ArmorBonusOptList[i].Opt = m_pManager->getItemOptInfo(szOptName);

		for (int j = 0; j < Const::Item::MaxEnchantLevel; j++) {
			enchantInfo->ArmorBonusOptList[i].Value[j] = szOptValListRoot[j].asInt();
		}
	}

	for (int i = 0; i < iAcceossoryBonusOptCount; ++i) {
		Value& accessoryBonusOptRoot = accessoryBonusOptListRoot[i];

		SGString szOptName = JsonUtil::getString(accessoryBonusOptRoot["opt_name"]);
		Value& szOptValListRoot = accessoryBonusOptRoot["opt_val"];
		DebugAssertMsg(szOptValListRoot.size() == Const::Item::MaxEnchantLevel, "악세서리 보너스 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)", szOptValListRoot.size(), Const::Item::MaxEnchantLevel);

		enchantInfo->AccessoryBonusOptList[i].Opt = m_pManager->getItemOptInfo(szOptName);

		for (int j = 0; j < Const::Item::MaxEnchantLevel; j++) {
			enchantInfo->AccessoryBonusOptList[i].Value[j] = szOptValListRoot[j].asInt();
		}
	}

	Value& weaponBonusMulitplyValueListRoot = enchantRoot["weapon_bonus_opt_val"];
	Value& probListRoot = enchantRoot["probs"];
	Value& sellBonusListRoot = enchantRoot["sell_bonus"];

	Value& weaponRarityConstantListRoot= enchantRoot["rarity_weapon_constant"];
	Value& armorRarityConstantListRoot = enchantRoot["rarity_armor_constant"];
	Value& accessoryRarityConstantListRoot = enchantRoot["rarity_accessory_constant"];

	int iWeaponBonusOptValueCount = (int)weaponBonusMulitplyValueListRoot.size();
	int iProbCount = (int)probListRoot.size();
	int iSellBonusCount = (int)sellBonusListRoot.size();

	int iWeaponRarityConstantCount = (int)weaponRarityConstantListRoot.size();
	int iArmorRarityConstantCount = (int)armorRarityConstantListRoot.size();
	int iAccessoryRarityConstantCount = (int)accessoryRarityConstantListRoot.size();

	DebugAssertMsg(iWeaponBonusOptValueCount == Const::Item::MaxEnchantLevel, "무기 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)", iWeaponBonusOptValueCount, Const::Item::MaxEnchantLevel);
	DebugAssertMsg(iProbCount == Const::Item::MaxEnchantLevel, "강화 확률 값이 %d개 입니다. (%d개로 맞춰야함)", iProbCount, Const::Item::MaxEnchantLevel);
	DebugAssertMsg(iSellBonusCount == Const::Item::MaxEnchantLevel, "판매 보너스 값이 %d개 입니다. (%d개로 맞춰야함)", iSellBonusCount, Const::Item::MaxEnchantLevel);

	DebugAssertMsg(iWeaponRarityConstantCount == RarityType::Max, "무기 옵트 벨류 값이 %d개 입니다. (%d개로 맞춰야함)", iWeaponBonusOptValueCount, RarityType::Max);
	DebugAssertMsg(iArmorRarityConstantCount == RarityType::Max, "강화 확률 값이 %d개 입니다. (%d개로 맞춰야함)", iProbCount, RarityType::Max);
	DebugAssertMsg(iAccessoryRarityConstantCount == RarityType::Max, "판매 보너스 값이 %d개 입니다. (%d개로 맞춰야함)", iSellBonusCount, RarityType::Max);

	for (int i = 0; i < Const::Item::MaxEnchantLevel; ++i) {
		enchantInfo->WeaponAttackPhysicalMultiplyValue[i] = weaponBonusMulitplyValueListRoot[i].asFloat();
		enchantInfo->EnchangeProbs[i] = probListRoot[i].asFloat();
		enchantInfo->SellBonus[i] = sellBonusListRoot[i].asFloat();
	}

	for (int i = 0; i < RarityType::Max; ++i) {
		enchantInfo->ArmorRarityConstant[i] = weaponRarityConstantListRoot[i].asFloat();
		enchantInfo->AccesorryRarityConstant[i] = armorRarityConstantListRoot[i].asFloat();
		enchantInfo->WeaponRarityConstant[i] = accessoryRarityConstantListRoot[i].asFloat();
	}

}
