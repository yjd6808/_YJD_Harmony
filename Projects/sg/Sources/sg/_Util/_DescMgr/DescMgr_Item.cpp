/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "DescMgr_Item.h"

#include "sg/_Core/AppConfig.h"
#include "sg/_Util/JsonUtil.h"


USING_NS_JC;
USING_NS_JS;

ItemInfoLoader::ItemInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ItemInfoLoader::Load()
{
	const jc::String& srcDataPath = g_cAppConfig.srcDataPath_;
	jc::String itemRootPath = jc::Path::Combine(srcDataPath, GetConfigFileName());

	if (!jc::Directory::Exist(itemRootPath))
		return false;

	try
	{
		for (int i = 0; i < ItemType::Max; ++i)
		{
			if (ItemType::IsEquip[i])
			{
				jc::String equipItemPath = jc::Path::Combine(itemRootPath, "equip");
				LoadEquip(equipItemPath, i);
			}
		}

		// TODO: loadConsume
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::ReadCommonInfo(
	Json::Value& _itemRoot,
	ItemType_t _itemType,
	int _detail1,
	int _detail2,
	ItemInfo* _pItemInfo)
{
	int codeValue = InvalidValue_v;

	codeValue = _itemRoot["code"].asInt();
	_pItemInfo->code_ = ItemCode(codeValue, _detail1, _detail2, _itemType).Code;
	_pItemInfo->name_ = JsonUtil::GetString(_itemRoot["name_kor"]);
	_pItemInfo->nameEng_ = JsonUtil::GetStringOrNull(_itemRoot["name_eng"]);
	_pItemInfo->rarity_ = (RarityType_t)_itemRoot["rarity"].asInt();
	_pItemInfo->icon_ = _itemRoot["icon"].asInt();
	_pItemInfo->sellPrice_ = _itemRoot["sell_price"].asInt();

	ItemCode itemCode(_pItemInfo->code_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::ReadArmorInfo(Json::Value& _armorRoot, ItemArmorInfo* _pItemArmorInfo)
{
	_pItemArmorInfo->armorPhysical_ = _armorRoot["armor_physical"].asInt();
	_pItemArmorInfo->armorMagical_ = _armorRoot["armor_magic"].asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::ReadVisualInfo(Json::Value& _visualRoot, ItemVisualInfo* _pVisual)
{
	_pVisual->shape_ = JsonUtil::GetString(_visualRoot["shape"]);
	_pVisual->shapeAlpha_ = JsonUtil::GetString(_visualRoot["shape_alpha"]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::ReadWeaponInfo(Json::Value& _weaponRoot, ItemWeaponInfo* _pWeapon)
{
	_pWeapon->attackPhysical_ = _weaponRoot["attack_physical"].asInt();
	_pWeapon->attackMagic_ = _weaponRoot["attack_magic"].asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadEquip(const jc::String& _equipItemPath, int _equipItemType)
{
	if (ItemType::HasDetailType[_equipItemType])
	{
		LoadDetailedEquip(_equipItemPath, _equipItemType);
		return;
	}

	LoadAccessory(jc::Path::Combine(_equipItemPath, ItemType::Name[_equipItemType]) + ".json", _equipItemType);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadDetailedEquip(const jc::String& _equipItemPath, int _equipItemType)
{
	const char* equipName = ItemType::Name[_equipItemType];

	switch (_equipItemType)
	{
	case (int)ItemType::Avatar:
		for (int charTypeIndex = 0; charTypeIndex < CharType::Max; ++charTypeIndex)
		{
			LoadAvatar(jc::Path::Combine(_equipItemPath, CharType::Name[charTypeIndex], equipName), charTypeIndex,
			           _equipItemType);
		}
		break;
	case (int)ItemType::Weapon:
		for (int charTypeIndex = 0; charTypeIndex < CharType::Max; ++charTypeIndex)
		{
			LoadWeapon(jc::Path::Combine(_equipItemPath, CharType::Name[charTypeIndex], equipName), charTypeIndex,
			           _equipItemType);
		}
		break;
	default:
		for (int equipArmorTypeIndex = 0; equipArmorTypeIndex < EquipArmorType::Max; ++equipArmorTypeIndex)
		{
			jc::String armorPath = jc::Path::Combine(_equipItemPath, equipName, EquipArmorType::Name[equipArmorTypeIndex]);
			LoadArmor(armorPath + ".json", equipArmorTypeIndex, _equipItemType);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadAccessory(const jc::String& _equipAccessoryPath, int _equipItemType)
{
	if (!File::Exist(_equipAccessoryPath))
		throw std::exception(StringUtil::Format("%s 파일을 찾지 못했습니다.\n", _equipAccessoryPath.Source()).Source());

	Json::Value root;
	JsonUtil::LoadThrow(_equipAccessoryPath, root);

	Json::Value& accessoryListRoot = root[ItemType::Name[_equipItemType]];

	for (ArrayIndex i = 0; i < accessoryListRoot.size(); ++i)
	{
		Json::Value& accessoryRoot = accessoryListRoot[i];
		ItemArmorInfo* pAccessoryInfo = dbg_new ItemArmorInfo;
		ReadCommonInfo(accessoryRoot, (ItemType_t)_equipItemType, InvalidValue_v, InvalidValue_v, pAccessoryInfo);
		ReadArmorInfo(accessoryRoot, pAccessoryInfo);
		AddData(pAccessoryInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadArmor(const jc::String& _equipItemArmorPath, int _armorType, int _equipItemType)
{
	if (!File::Exist(_equipItemArmorPath))
		throw std::exception(StringUtil::Format("%s 파일을 찾지 못했습니다.\n", _equipItemArmorPath.Source()).Source());

	Json::Value root;
	JsonUtil::LoadThrow(_equipItemArmorPath, root);

	const char* armorTypeName = EquipArmorType::Name[_armorType];
	Json::Value& armorListRoot = root[armorTypeName];

	for (ArrayIndex i = 0; i < armorListRoot.size(); ++i)
	{
		Json::Value& armorRoot = armorListRoot[i];
		ItemArmorInfo* pArmorInfo = dbg_new ItemArmorInfo;
		ReadCommonInfo(armorRoot, (ItemType_t)_equipItemType, _armorType, InvalidValue_v, pArmorInfo);
		ReadArmorInfo(armorRoot, pArmorInfo);
		AddData(pArmorInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadAvatar(const jc::String& _equipItemAvatarPath, int _charType, int _equipItemType)
{
	for (int avatarTypeIndex = 0; avatarTypeIndex < AvatarType::Max; ++avatarTypeIndex)
	{
		jc::String avatarPath = jc::Path::Combine(_equipItemAvatarPath, AvatarType::Name[avatarTypeIndex]) + ".json";

		Json::Value root;
		JsonUtil::LoadThrow(avatarPath, root);

		const char* avatarPartName = AvatarType::Name[avatarTypeIndex];
		Json::Value& avatarPartListRoot = root[avatarPartName];

		for (ArrayIndex i = 0; i < avatarPartListRoot.size(); ++i)
		{
			Json::Value& armorRoot = avatarPartListRoot[i];
			ItemAvatarInfo* pAvatarInfo = dbg_new ItemAvatarInfo;
			ReadCommonInfo(armorRoot, (ItemType_t)_equipItemType, avatarTypeIndex, _charType, pAvatarInfo);
			ReadVisualInfo(armorRoot, pAvatarInfo);
			AddData(pAvatarInfo);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemInfoLoader::LoadWeapon(const jc::String& _equipItemWeaponPath, int _charType, int _equipItemType)
{
	for (int weaponTypeIndex = 0; weaponTypeIndex < WeaponType::Max; ++weaponTypeIndex)
	{
		jc::String weaponPath = jc::Path::Combine(_equipItemWeaponPath, WeaponType::Name[weaponTypeIndex]) + ".json";

		Json::Value root;
		JsonUtil::LoadThrow(weaponPath, root);

		const char* weaponName = WeaponType::Name[weaponTypeIndex];
		Json::Value& weaponListRoot = root[weaponName];

		for (ArrayIndex i = 0; i < weaponListRoot.size(); ++i)
		{
			Json::Value& weaponRoot = weaponListRoot[i];
			ItemWeaponInfo* pWeaponInfo = dbg_new ItemWeaponInfo;
			ReadCommonInfo(weaponRoot, (ItemType_t)_equipItemType, weaponTypeIndex, _charType, pWeaponInfo);
			ReadVisualInfo(weaponRoot, pWeaponInfo);
			ReadWeaponInfo(weaponRoot, pWeaponInfo);
			AddData(pWeaponInfo);
		}
	}
}
