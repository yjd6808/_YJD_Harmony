/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 2:56:13 AM
 * =====================
 *
 */



#include "SGGlobal.h"

int SGGlobal::convertAvatarPartNameToType(const SGString& str) {
	DebugAssertMessage(AvatarPartNameToTypeMap.Exist(str), "존재하지 않는 아바타 부위 이름입니다.");
	return AvatarPartNameToTypeMap[str];
}

void SGGlobal::initialize() {

	{
		AvatarPartNameToTypeMap.Insert("skin", VisualType::Skin);
		AvatarPartNameToTypeMap.Insert("shoes", VisualType::Shoes);
		AvatarPartNameToTypeMap.Insert("pants", VisualType::Pants);
		AvatarPartNameToTypeMap.Insert("neck", VisualType::Neck);
		AvatarPartNameToTypeMap.Insert("hair", VisualType::Hair);
		AvatarPartNameToTypeMap.Insert("face", VisualType::Face);
		AvatarPartNameToTypeMap.Insert("coat", VisualType::Coat);
		AvatarPartNameToTypeMap.Insert("cap", VisualType::Cap);
		AvatarPartNameToTypeMap.Insert("belt", VisualType::Belt);
		AvatarPartNameToTypeMap.Insert("weapon", VisualType::Weapon);
	}


	{
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Skin, "gn_body");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Shoes, "gn_shoes");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Pants, "gn_pants");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Neck, "gn_neck");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Hair, "gn_hair");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Face, "gn_face");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Coat, "gn_coat");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Cap, "gn_cap");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Belt, "gn_belt");
		AvatarPrefix[CharacterType::Gunner].Insert(VisualType::Weapon, "gn_weapon");
	}

	{
		m_AvatarPackName[CharacterType::Gunner][VisualType::Skin] = "sprite_character_gunner_equipment_avatar_skin.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Shoes] = "sprite_character_gunner_equipment_avatar_shoes.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Pants] = "sprite_character_gunner_equipment_avatar_pants.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Neck] = "sprite_character_gunner_equipment_avatar_neck.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Hair] = "sprite_character_gunner_equipment_avatar_hair.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Face] = "sprite_character_gunner_equipment_avatar_face.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Coat] = "sprite_character_gunner_equipment_avatar_coat.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Cap] = "sprite_character_gunner_equipment_avatar_cap.NPK";
		m_AvatarPackName[CharacterType::Gunner][VisualType::Belt] = "sprite_character_gunner_equipment_avatar_belt.NPK";

		m_WeaponPackName[CharacterType::Gunner][WeaponType::Auto] = "sprite_character_gunner_equipment_weapon_auto.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Bowgun] = "sprite_character_gunner_equipment_weapon_bowgun.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::HandCannon] = "sprite_character_gunner_equipment_weapon_hcan.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Musket] = "sprite_character_gunner_equipment_weapon_musket.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Revolver] = "sprite_character_gunner_equipment_weapon_rev.NPK";
	}
}



const SGString& SGGlobal::getAvatarNpkName(int characterType, int visualAvatarType) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(visualAvatarType >= AvatarType::Begin && visualAvatarType < AvatarType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_AvatarPackName[characterType][visualAvatarType];
}

const SGString& SGGlobal::getWeaponNpkName(int characterType, int weaponType) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(weaponType >= WeaponType::Begin && weaponType < WeaponType::Max, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_WeaponPackName[characterType][weaponType];
}
