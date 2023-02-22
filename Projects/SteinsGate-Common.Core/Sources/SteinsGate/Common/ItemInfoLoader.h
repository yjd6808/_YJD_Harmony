/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ItemInfoLoader : ConfigFileLoaderAbstract
{
public:
	ItemInfoLoader(DataManagerAbstract* manager);

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Item; }
	void loadArmor(const JCore::String& equipItemArmorPath, int armorType, int equipItemType);
	
	void loadAvatar(const JCore::String& equipItemAvatarPath, int charType, int equipItemType);
	void loadWeapon(const JCore::String& equipItemWeaponPath, int charType, int equipItemType);
	void loadDetailedEquip(const SGString& equipItemPath, int equipItemType);
	
	void loadAccessory(const SGString& equipAccessoryPath, int equipItemType);
	void loadEquip(const SGString& equipItemPath, int equipItemType);
	bool load() override;

	static void readVisualInfo(Json::Value& visualRoot, ItemVisualInfo* pVisual);
	static void readWeaponInfo(Json::Value& weaponRoot, ItemWeaponInfo* pWeapon);
	static void readArmorInfo(Json::Value& armorRoot, ItemArmorInfo* itemArmorInfo);
	static void readCommonInfo(Json::Value& itemRoot, ItemType_t itemType, int detail1, int detail2, ItemInfo* itemInfo);
};