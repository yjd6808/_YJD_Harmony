/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 4:15:22 PM
 * =====================
 *
 */

#include "Core.h"
#include "Struct.h"

#include <SteinsGate/Common/Const.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSpriteData::ActorPartSpriteData()
: sgaIndex_(InvalidValue_v)
, imgIndex_(InvalidValue_v)
, zOrder_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSpriteData::ActorPartSpriteData(int _zOrder, int _sga, int _img)
: sgaIndex_(_sga)
, imgIndex_(_img)
, zOrder_(_zOrder)
{
}

//ActorPartSpriteDataCustom::ActorPartSpriteDataCustom()
//	: ActorPartSpriteData()
//{}
//
//ActorPartSpriteDataCustom::ActorPartSpriteDataCustom(int zorder, int sga, int img)
//	: ActorPartSpriteData(zorder, sga, img)
//{}

//////////////////////////////////////////////////////////////////////////////////////////
SgaResourceIndex::SgaResourceIndex()
: value_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String SgaResourceIndex::ToString() const
{
	return StringUtil::Format("Sga(%d) Img(%d) Sprite(%d)", un_.sgaIndex_, un_.imgIndex_, un_.frameIndex_);
}

//////////////////////////////////////////////////////////////////////////////////////////
AccountData::AccountData()
: id_{ "" }
, pass_{ "" }
, lastLoginTime_(0)
, lastServer_(GameServerType::Max)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AccountData::Clear()
{
	id_ = "";
	pass_ = "";
	lastLoginTime_.Tick = 0;
	lastServer_ = GameServerType::Max;
}

//////////////////////////////////////////////////////////////////////////////////////////
PlayerData::PlayerData()
: CharId(InvalidValue_v)
, Name{ "" }
, CharType(CharType::Gunner)
, Life(0)
, MaxLife(0)
, Mana(0)
, MaxMana(0)
, Level(0)
, Gold(0)
, Str(0)
, Dex(0)
, Int(0)
, Vit(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void PlayerData::clear()
{
	CharId = InvalidValue_v;
	Name = "";
	CharType = CharType::Gunner;
	Life = 0;
	MaxLife = 0;
	Mana = 0;
	MaxMana = 0;
	Level = 0;

	Gold = 0;
	Str = 0;
	Dex = 0;
	Int = 0;
	Vit = 0;
}

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
