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
	ItemInfoLoader(DataManagerAbstract* _pManager);

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Item; }
	void LoadArmor(const JCore::String& _equipItemArmorPath, int _armorType, int _equipItemType);

	void LoadAvatar(const JCore::String& _equipItemAvatarPath, int _charType, int _equipItemType);
	void LoadWeapon(const JCore::String& _equipItemWeaponPath, int _charType, int _equipItemType);
	void LoadDetailedEquip(const SGString& _equipItemPath, int _equipItemType);

	void LoadAccessory(const SGString& _equipAccessoryPath, int _equipItemType);
	void LoadEquip(const SGString& _equipItemPath, int _equipItemType);
	bool Load() override;

	static void ReadVisualInfo(Json::Value& _visualRoot, ItemVisualInfo* _pVisual);
	static void ReadWeaponInfo(Json::Value& _weaponRoot, ItemWeaponInfo* _pWeapon);
	static void ReadArmorInfo(Json::Value& _armorRoot, ItemArmorInfo* _pItemArmorInfo);
	static void ReadCommonInfo(Json::Value& _itemRoot, ItemType_t _itemType, int _detail1, int _detail2, ItemInfo* _pItemInfo);
};
