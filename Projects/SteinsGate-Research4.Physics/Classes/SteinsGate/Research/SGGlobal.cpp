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
		AvatarPartNameToTypeMap.Insert("skin", AvatarType::Skin);
		AvatarPartNameToTypeMap.Insert("shoes", AvatarType::Shoes);
		AvatarPartNameToTypeMap.Insert("pants", AvatarType::Pants);
		AvatarPartNameToTypeMap.Insert("neck", AvatarType::Neck);
		AvatarPartNameToTypeMap.Insert("hair", AvatarType::Hair);
		AvatarPartNameToTypeMap.Insert("face", AvatarType::Face);
		AvatarPartNameToTypeMap.Insert("coat", AvatarType::Coat);
		AvatarPartNameToTypeMap.Insert("cap", AvatarType::Cap);
		AvatarPartNameToTypeMap.Insert("belt", AvatarType::Belt);
		AvatarPartNameToTypeMap.Insert("weapon", AvatarType::Weapon);
	}


	{
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Skin, "gn_body");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Shoes, "gn_shoes");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Pants, "gn_pants");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Neck, "gn_neck");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Hair, "gn_hair");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Face, "gn_face");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Coat, "gn_coat");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Cap, "gn_cap");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Belt, "gn_belt");
		AvatarPrefix[CharacterType::Gunner].Insert(AvatarType::Weapon, "gn_weapon");
	}

	{
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Skin] = "sprite_character_gunner_equipment_avatar_skin.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Shoes] = "sprite_character_gunner_equipment_avatar_shoes.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Pants] = "sprite_character_gunner_equipment_avatar_pants.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Neck] = "sprite_character_gunner_equipment_avatar_neck.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Hair] = "sprite_character_gunner_equipment_avatar_hair.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Face] = "sprite_character_gunner_equipment_avatar_face.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Coat] = "sprite_character_gunner_equipment_avatar_coat.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Cap] = "sprite_character_gunner_equipment_avatar_cap.NPK";
		m_AvatarPackName[CharacterType::Gunner][AvatarType::Belt] = "sprite_character_gunner_equipment_avatar_belt.NPK";

		m_WeaponPackName[CharacterType::Gunner][WeaponType::Auto] = "sprite_character_gunner_equipment_weapon_auto.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Bowgun] = "sprite_character_gunner_equipment_weapon_bowgun.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::HandCannon] = "sprite_character_gunner_equipment_weapon_hcan.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Musket] = "sprite_character_gunner_equipment_weapon_musket.NPK";
		m_WeaponPackName[CharacterType::Gunner][WeaponType::Revolver] = "sprite_character_gunner_equipment_weapon_rev.NPK";
	}
}



const SGString& SGGlobal::getAvatarNpkName(int characterType, int visualAvatarType) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(visualAvatarType >= AvatarType::Begin && visualAvatarType < AvatarType::VisualMax, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_AvatarPackName[characterType][visualAvatarType];
}

const SGString& SGGlobal::getWeaponNpkName(int characterType, int visualAvatarType) {
	DebugAssertMessage(characterType >= CharacterType::Begin && characterType >= CharacterType::End, "올바르지 않은 캐릭터 타입입니다.");
	DebugAssertMessage(visualAvatarType >= AvatarType::Begin && visualAvatarType < AvatarType::VisualMax, "올바르지 않은 비주얼 아바타 타입입니다.");
	return m_WeaponPackName[characterType][visualAvatarType];
}
