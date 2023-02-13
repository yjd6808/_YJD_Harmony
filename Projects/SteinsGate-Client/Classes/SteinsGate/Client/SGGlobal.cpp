/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:56:13 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGGlobal.h"

#include <SteinsGate/Client/SGFrameTexture.h>

SGGlobal::SGGlobal()
	: m_pDefaultFrameTexture(nullptr)
	, m_bDrawThicknessBox(false)
	, m_bDrawBodyBoundingBox(false)
	, m_bDrawAttackBox(false)
	, m_bDrawEffect(true)
{}

SGGlobal::~SGGlobal() {
	CC_SAFE_RELEASE(m_pDefaultFrameTexture);
}

int SGGlobal::convertAvatarPartNameToType(const SGString& str) {
	DebugAssertMsg(m_AvatarPartNameToTypeMap.Exist(str), "존재하지 않는 아바타 부위 이름입니다.");
	return m_AvatarPartNameToTypeMap[str];
}

void SGGlobal::toggleDrawThicknessBox() {
	m_bDrawThicknessBox = !m_bDrawThicknessBox;
}

void SGGlobal::toggleDrawBodyBoundingBox() {
	m_bDrawBodyBoundingBox = !m_bDrawBodyBoundingBox;
}

void SGGlobal::toggleDrawAttackBox() {
	m_bDrawAttackBox = !m_bDrawAttackBox;
}

void SGGlobal::toggleDrawEffect() {
	m_bDrawEffect = !m_bDrawEffect;
}

void SGGlobal::initialize() {

	{
		m_AvatarPartNameToTypeMap.Insert("skin", VisualType::Skin);
		m_AvatarPartNameToTypeMap.Insert("shoes", VisualType::Shoes);
		m_AvatarPartNameToTypeMap.Insert("pants", VisualType::Pants);
		m_AvatarPartNameToTypeMap.Insert("neck", VisualType::Neck);
		m_AvatarPartNameToTypeMap.Insert("hair", VisualType::Hair);
		m_AvatarPartNameToTypeMap.Insert("face", VisualType::Face);
		m_AvatarPartNameToTypeMap.Insert("coat", VisualType::Coat);
		m_AvatarPartNameToTypeMap.Insert("cap", VisualType::Cap);
		m_AvatarPartNameToTypeMap.Insert("belt", VisualType::Belt);
		m_AvatarPartNameToTypeMap.Insert("weapon_left", VisualType::WeaponLeft);
		m_AvatarPartNameToTypeMap.Insert("weapon_right", VisualType::WeaponRight);
	}


	{
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Skin, "gn_body");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Shoes, "gn_shoes");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Pants, "gn_pants");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Neck, "gn_neck");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Hair, "gn_hair");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Face, "gn_face");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Coat, "gn_coat");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Cap, "gn_cap");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::Belt, "gn_belt");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::WeaponLeft, "__");
		m_AvatarPrefix[CharType::Gunner].Insert(VisualType::WeaponRight, "__");
	}

	{
		m_AvatarPackName[CharType::Gunner][VisualType::Skin] = "sprite_character_gunner_equipment_avatar_skin.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Shoes] = "sprite_character_gunner_equipment_avatar_shoes.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Pants] = "sprite_character_gunner_equipment_avatar_pants.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Neck] = "sprite_character_gunner_equipment_avatar_neck.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Hair] = "sprite_character_gunner_equipment_avatar_hair.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Face] = "sprite_character_gunner_equipment_avatar_face.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Coat] = "sprite_character_gunner_equipment_avatar_coat.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Cap] = "sprite_character_gunner_equipment_avatar_cap.NPK";
		m_AvatarPackName[CharType::Gunner][VisualType::Belt] = "sprite_character_gunner_equipment_avatar_belt.NPK";

		m_WeaponPackName[CharType::Gunner][GunnerWeaponType::Auto] = "sprite_character_gunner_equipment_weapon_auto.NPK";
		m_WeaponPackName[CharType::Gunner][GunnerWeaponType::Bowgun] = "sprite_character_gunner_equipment_weapon_bowgun.NPK";
		m_WeaponPackName[CharType::Gunner][GunnerWeaponType::HandCannon] = "sprite_character_gunner_equipment_weapon_hcan.NPK";
		m_WeaponPackName[CharType::Gunner][GunnerWeaponType::Musket] = "sprite_character_gunner_equipment_weapon_musket.NPK";
		m_WeaponPackName[CharType::Gunner][GunnerWeaponType::Revolver] = "sprite_character_gunner_equipment_weapon_rev.NPK";
	}
}



const SGString& SGGlobal::getAvatarNpkName(int charType, int visualAvatarType) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(visualAvatarType >= AvatarType::Begin && visualAvatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_AvatarPackName[charType][visualAvatarType];
}

const SGString& SGGlobal::getWeaponNpkName(int charType, int weaponType) {
	DebugAssertMsg(charType >= CharType::Begin && charType >= CharType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMsg(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_WeaponPackName[charType][weaponType];
}

SGSpriteFrameTexture* SGGlobal::getDefaultFrameTexture() {

	if (m_pDefaultFrameTexture != nullptr) {
		return m_pDefaultFrameTexture;
	}

	static unsigned char White2x2TextureData_v[] = {
		// RGBA8888
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF
	};

	
	SGTexture* pDefaultTexture = dbg_new SGTexture;
	pDefaultTexture->initWithData(White2x2TextureData_v, sizeof(White2x2TextureData_v), SGTexture::PixelFormat::RGB888, 2, 2, { 2, 2 });
	m_pDefaultFrameTexture = dbg_new SGSpriteFrameTexture(pDefaultTexture, { 0, 0, 2, 2, 2, 2 }, 0, true);
	m_pDefaultFrameTexture->autorelease();
	m_pDefaultFrameTexture->retain();

	return m_pDefaultFrameTexture;
}
