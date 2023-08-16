/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Core.h"
#include "ItemInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

ItemInfoLoader::ItemInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ItemInfoLoader::load() {

	SGString itemRootPath = SGPath::Combine(CoreCommonInfo_v->ConfigPath, getConfigFileName());

	if (!JCore::Directory::Exist(itemRootPath))
		return false;

	try {

		for (int i = 0; i < ItemType::Max; ++i) {
			if (ItemType::IsEquip[i]) {

				SGString equipItemPath = SGPath::Combine(itemRootPath, "equip");
				loadEquip(equipItemPath, i);
			} 
		}

		// TODO: loadConsume
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void ItemInfoLoader::readCommonInfo(
	Json::Value& itemRoot, 
	ItemType_t itemType,
	int detail1,
	int detail2,
	ItemInfo* itemInfo)
{
	int iCode = InvalidValue_v;

	iCode = itemRoot["code"].asInt();
	itemInfo->Code = ItemCode(iCode, detail1, detail2, itemType).Code;
	itemInfo->Name = JsonUtil::getString(itemRoot["name_kor"]);
	itemInfo->NameEng = JsonUtil::getStringOrNull(itemRoot["name_eng"]);
	itemInfo->Rarity = (RarityType_t)itemRoot["rarity"].asInt();
	itemInfo->Icon = itemRoot["icon"].asInt();
	itemInfo->SellPrice = itemRoot["sell_price"].asInt();

	ItemCode code(itemInfo->Code);
	int a = 40;
}

void ItemInfoLoader::readArmorInfo(Json::Value& armorRoot, ItemArmorInfo* itemArmorInfo) {
	itemArmorInfo->ArmorPhysical = armorRoot["armor_physical"].asInt();
	itemArmorInfo->ArmorMagical = armorRoot["armor_magic"].asInt();
}


void ItemInfoLoader::readVisualInfo(Json::Value& visualRoot, ItemVisualInfo* pVisual) {
	pVisual->Shape = JsonUtil::getString(visualRoot["shape"]);
	pVisual->ShapeAlpha = JsonUtil::getString(visualRoot["shape_alpha"]);
}

void ItemInfoLoader::readWeaponInfo(Json::Value& weaponRoot, ItemWeaponInfo* pWeapon) {
	pWeapon->AttackPhysical = weaponRoot["attack_physical"].asInt();
	pWeapon->AttackMagic = weaponRoot["attack_magic"].asInt();;
}

void ItemInfoLoader::loadEquip(const SGString& equipItemPath, int itemType) {
	if (ItemType::HasDetailType[itemType]) {
		loadDetailedEquip(equipItemPath, itemType);
		return;
	}

	loadAccessory(SGPath::Combine(equipItemPath, ItemType::Name[itemType]) + ".json", itemType);
}




void ItemInfoLoader::loadDetailedEquip(const SGString& equipItemPath, int itemType) {

	const char* equipName = ItemType::Name[itemType];

	switch (itemType) {
	case (int)ItemType::Avatar: 
		for (int eCharType = 0; eCharType < CharType::Max; ++eCharType) {
			loadAvatar(SGPath::Combine(equipItemPath, CharType::Name[eCharType], equipName), eCharType, itemType);
		}
		break;
	case (int)ItemType::Weapon:
		for (int eCharType = 0; eCharType < CharType::Max; ++eCharType) {
			loadWeapon(SGPath::Combine(equipItemPath, CharType::Name[eCharType], equipName), eCharType, itemType);
		}
		break;
	default:
		for (int eEquipArmorType = 0; eEquipArmorType <EquipArmorType::Max; ++eEquipArmorType) {
			SGString armorPath = SGPath::Combine(equipItemPath, equipName, EquipArmorType::Name[eEquipArmorType]);
			loadArmor(armorPath + ".json", eEquipArmorType, itemType);
		}
		break;
	}
}


void ItemInfoLoader::loadAccessory(const SGString& equipAccessoryPath, int itemType) {
	if (!File::Exist(equipAccessoryPath))
		throw std::exception(StringUtil::Format("%s 파일을 찾지 못했습니다.\n", equipAccessoryPath.Source()).Source());

	Json::Value root;
	JsonUtil::loadThrow(equipAccessoryPath, root);

	Json::Value& accessoryListRoot = root[ItemType::Name[itemType]];

	for (int i = 0; i < accessoryListRoot.size(); ++i) {
		Json::Value& accessoryRoot = accessoryListRoot[i];
		ItemArmorInfo* pAccessoryInfo = dbg_new ItemArmorInfo;
		readCommonInfo(accessoryRoot, (ItemType_t)itemType, InvalidValue_v, InvalidValue_v, pAccessoryInfo);
		readArmorInfo(accessoryRoot, pAccessoryInfo);
		addData(pAccessoryInfo);
	}
}

void ItemInfoLoader::loadArmor(const JCore::String& equipItemArmorPath, int armorType, int itemType) {
	if (!File::Exist(equipItemArmorPath))
		throw std::exception(StringUtil::Format("%s 파일을 찾지 못했습니다.\n", equipItemArmorPath.Source()).Source());

	Json::Value root;
	JsonUtil::loadThrow(equipItemArmorPath, root);

	const char* ArmorTypeName = EquipArmorType::Name[armorType];
	Json::Value& armorListRoot = root[ArmorTypeName];

	for (int i = 0; i < armorListRoot.size(); ++i) {
		Json::Value& armorRoot = armorListRoot[i];
		ItemArmorInfo* pArmorInfo = dbg_new ItemArmorInfo;
		readCommonInfo(armorRoot, (ItemType_t)itemType, armorType, InvalidValue_v, pArmorInfo);
		readArmorInfo(armorRoot, pArmorInfo);
		addData(pArmorInfo);
	}
}

void ItemInfoLoader::loadAvatar(const JCore::String& equipItemAvatarPath, int charType, int itemType) {
	for (int eAvatarType = 0; eAvatarType < AvatarType::Max; ++eAvatarType) {

		SGString avatarPath = SGPath::Combine(equipItemAvatarPath, AvatarType::Name[eAvatarType]) + ".json";

		Json::Value root;
		JsonUtil::loadThrow(avatarPath, root);

		const char* avatarPartName = AvatarType::Name[eAvatarType];
		Json::Value& avatarPartListRoot = root[avatarPartName];

		for (int i = 0; i < avatarPartListRoot.size(); ++i) {
			Json::Value& armorRoot = avatarPartListRoot[i];
			ItemAvatarInfo* pAvatarInfo = dbg_new ItemAvatarInfo;
			readCommonInfo(armorRoot, (ItemType_t)itemType, eAvatarType, charType, pAvatarInfo);
			readVisualInfo(armorRoot, pAvatarInfo);
			addData(pAvatarInfo);
		}
	}
}

void ItemInfoLoader::loadWeapon(const JCore::String& equipItemWeaponPath, int charType, int itemType) {
	for (int eWeaponType = 0; eWeaponType < WeaponType::Max; ++eWeaponType) {

		SGString weaponPath = SGPath::Combine(equipItemWeaponPath, WeaponType::Name[eWeaponType]) + ".json";

		Json::Value root;
		JsonUtil::loadThrow(weaponPath, root);

		const char* weaponName = WeaponType::Name[eWeaponType];
		Json::Value& weaponListRoot = root[weaponName];

		for (int i = 0; i < weaponListRoot.size(); ++i) {
			Json::Value& weaponRoot = weaponListRoot[i];
			ItemWeaponInfo* pWeaponInfo = dbg_new ItemWeaponInfo;
			readCommonInfo(weaponRoot, (ItemType_t)itemType, eWeaponType, charType, pWeaponInfo);
			readVisualInfo(weaponRoot, pWeaponInfo);
			readWeaponInfo(weaponRoot, pWeaponInfo);
			addData(pWeaponInfo);
		}
	}
}
