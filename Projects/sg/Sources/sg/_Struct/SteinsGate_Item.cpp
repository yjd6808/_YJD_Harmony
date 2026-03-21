/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#include "SteinsGate_Item.h"

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _total)
: Code(_total)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _code, ItemType_t _type)
{
	commonUn_.code_ = _code;
	commonUn_.detail1_ = InvalidValue_v;
	commonUn_.detail2_ = InvalidValue_v;
	commonUn_.type_ = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _code, int _detail, ItemType_t _type)
{
	commonUn_.code_ = _code;
	commonUn_.detail1_ = _detail;
	commonUn_.detail2_ = InvalidValue_v;
	commonUn_.type_ = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _code, int _detail1, int _detail2, ItemType_t _type)
{
	commonUn_.code_ = _code;
	commonUn_.detail1_ = _detail1;
	commonUn_.detail2_ = _detail2;
	commonUn_.type_ = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::InitAvatarCode(CharType_t _charType, AvatarType_t _avatarType, int _code)
{
	avatarUn_.charType_ = _charType;
	avatarUn_.itemType_ = ItemType::Avatar;
	avatarUn_.partType_ = _avatarType;
	avatarUn_.code_ = _code;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::InitWeaponCode(CharType_t _charType, WeaponType_t _weaponType, int _code)
{
	weaponUn_.itemType_ = ItemType::Weapon;
	weaponUn_.weaponType_ = _weaponType;
	weaponUn_.charType_ = _charType;
	weaponUn_.code_ = _code;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::InitArmorCode(ItemType_t _itemType, EquipArmorType_t _armorType, int _code)
{
	armorUn_.code_ = _code;
	armorUn_.itemType_ = _itemType;
	armorUn_.armorType_ = _armorType;
	armorUn_._ = InvalidValue_v;
}
