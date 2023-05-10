/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 4:15:22 PM
 * =====================
 *
 */

#include "Core.h"
#include "Struct.h"

#include <SteinsGate/Common/Config.h>

USING_NS_JC;


ActorPartSpriteData::ActorPartSpriteData()
	: SgaIndex(InvalidValue_v)
	, ZOrder(0)
	, ImgIndex(InvalidValue_v)
{}

ActorPartSpriteData::ActorPartSpriteData(int zorder, int sga, int img)
	: SgaIndex(sga)
	, ZOrder(zorder)
	, ImgIndex(img)
{}

SgaResourceIndex::SgaResourceIndex() : Value(0) {}


String SgaResourceIndex::ToString() const {
	return StringUtil::Format("Sga(%d) Img(%d) Sprite(%d)", Un.SgaIndex, Un.ImgIndex, Un.FrameIndex);
}

AccountData::AccountData()
	: UUId(InvalidValue_v)
	, Name{""}
	, Pass{""}
	, LastLogin(0)
	, LastServer(0)
	, Logined(false)
{}

PlayerData::PlayerData()
	: Name{""}
	, CharId(InvalidValue_v)
	, CharType(CharType::Gunner)
	, Gold(0)
	, Str(0)
	, Dex(0)
	, Int(0)
	, Vit(0)
	, Life(0)
	, Mana(0)
	, Level(0)
{}

ItemCode::ItemCode(int Total)
	: Code(Total)
{}



ItemCode::ItemCode(int Code, ItemType_t Type) {
	CommonUn.Code = Code;
	CommonUn.Detail1 = InvalidValue_v;
	CommonUn.Detail2 = InvalidValue_v;
	CommonUn.Type = Type;
}

ItemCode::ItemCode(int Code, int Detail, ItemType_t Type) {
	CommonUn.Code = Code;
	CommonUn.Detail1 = Detail;
	CommonUn.Detail2 = InvalidValue_v;
	CommonUn.Type = Type;
}

ItemCode::ItemCode(int Code, int Detail1, int Detail2, ItemType_t Type) {
	CommonUn.Code = Code;
	CommonUn.Detail1 = Detail1;
	CommonUn.Detail2 = Detail2;
	CommonUn.Type = Type;
}


void ItemCode::initAvatarCode(CharType_t charType, AvatarType_t avatarType, int code) {
	AvatarUn.CharType = charType;
	AvatarUn.ItemType = ItemType::Avatar;
	AvatarUn.PartType = avatarType;
	AvatarUn.Code = code;
}

void ItemCode::initWeaponCode(CharType_t charType, WeaponType_t weaponType, int code) {
	WeaponUn.ItemType = ItemType::Weapon;
	WeaponUn.WeaponType = weaponType;
	WeaponUn.CharType = charType;
	WeaponUn.Code = code;
}

void ItemCode::initArmorCode(ItemType_t itemType, EquipArmorType_t armorType, int code) {
	ArmorUn.Code = code;
	ArmorUn.ItemType = itemType;
	ArmorUn.ArmorType = armorType;
	ArmorUn._ = InvalidValue_v;
}

