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
: SgaIndex(InvalidValue_v)
, ImgIndex(InvalidValue_v)
, ZOrder(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSpriteData::ActorPartSpriteData(int _zOrder, int _sga, int _img)
: SgaIndex(_sga)
, ImgIndex(_img)
, ZOrder(_zOrder)
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
: Value(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String SgaResourceIndex::ToString() const
{
	return StringUtil::Format("Sga(%d) Img(%d) Sprite(%d)", Un.SgaIndex, Un.ImgIndex, Un.FrameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
AccountData::AccountData()
: Id{ "" }
, Pass{ "" }
, LastLogin(0)
, LastServer(GameServerType::Max)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AccountData::Clear()
{
	Id = "";
	Pass = "";
	LastLogin.Tick = 0;
	LastServer = GameServerType::Max;
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
	CommonUn.Code = _code;
	CommonUn.Detail1 = InvalidValue_v;
	CommonUn.Detail2 = InvalidValue_v;
	CommonUn.Type = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _code, int _detail, ItemType_t _type)
{
	CommonUn.Code = _code;
	CommonUn.Detail1 = _detail;
	CommonUn.Detail2 = InvalidValue_v;
	CommonUn.Type = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemCode::ItemCode(int _code, int _detail1, int _detail2, ItemType_t _type)
{
	CommonUn.Code = _code;
	CommonUn.Detail1 = _detail1;
	CommonUn.Detail2 = _detail2;
	CommonUn.Type = _type;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::initAvatarCode(CharType_t _charType, AvatarType_t _avatarType, int _code)
{
	AvatarUn.CharType = _charType;
	AvatarUn.ItemType = ItemType::Avatar;
	AvatarUn.PartType = _avatarType;
	AvatarUn.Code = _code;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::initWeaponCode(CharType_t _charType, WeaponType_t _weaponType, int _code)
{
	WeaponUn.ItemType = ItemType::Weapon;
	WeaponUn.WeaponType = _weaponType;
	WeaponUn.CharType = _charType;
	WeaponUn.Code = _code;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ItemCode::initArmorCode(ItemType_t _itemType, EquipArmorType_t _armorType, int _code)
{
	ArmorUn.Code = _code;
	ArmorUn.ItemType = _itemType;
	ArmorUn.ArmorType = _armorType;
	ArmorUn._ = InvalidValue_v;
}
