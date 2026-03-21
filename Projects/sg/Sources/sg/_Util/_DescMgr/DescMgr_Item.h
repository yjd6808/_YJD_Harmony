/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include "sg/_Struct/SteinsGate_Item.h"
#include "sg/_Util/DescLoaderAbstract.h"

struct ItemInfoLoader : DescLoaderAbstract
{
public:
	ItemInfoLoader();

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Item; }
	void LoadArmor(const jc::String& _equipItemArmorPath, int _armorType, int _equipItemType);

	void LoadAvatar(const jc::String& _equipItemAvatarPath, int _charType, int _equipItemType);
	void LoadWeapon(const jc::String& _equipItemWeaponPath, int _charType, int _equipItemType);
	void LoadDetailedEquip(const jc::String& _equipItemPath, int _equipItemType);

	void LoadAccessory(const jc::String& _equipAccessoryPath, int _equipItemType);
	void LoadEquip(const jc::String& _equipItemPath, int _equipItemType);
	bool Load() override;

	static void ReadVisualInfo(Json::Value& _visualRoot, ItemVisualInfo* _pVisual);
	static void ReadWeaponInfo(Json::Value& _weaponRoot, ItemWeaponInfo* _pWeapon);
	static void ReadArmorInfo(Json::Value& _armorRoot, ItemArmorInfo* _pItemArmorInfo);
	static void ReadCommonInfo(Json::Value& _itemRoot, ItemType_t _itemType, int _detail1, int _detail2, ItemInfo* _pItemInfo);
};
