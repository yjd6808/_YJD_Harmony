/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Core.h"
#include "CharBaseInfoLoader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;

CharBaseInfoLoader::CharBaseInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool CharBaseInfoLoader::load() {

	Value root;

	if (!loadJson(root))
		return false;

	try {
		Value& gunnerRoot = root["gunner"];

		// 다른 캐릭이 만약 추가되면 코드 변경 필요
		GunnerInfo* pGunnerInfo = dbg_new GunnerInfo();
		readCharBaseInfo(gunnerRoot, pGunnerInfo);
		readGunnerInfo(gunnerRoot, pGunnerInfo);
		addData(pGunnerInfo);
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}


void CharBaseInfoLoader::readCharBaseInfo(Json::Value& charBaseRoot, CharBaseInfo* baseInfo) {
	baseInfo->Code = CharType::Gunner;
	baseInfo->HP = charBaseRoot["hp"].asInt();
	baseInfo->MP = charBaseRoot["mp"].asInt();
	baseInfo->Strength = charBaseRoot["str"].asInt();
	baseInfo->Dexterity = charBaseRoot["dex"].asInt();
	baseInfo->Vitality = charBaseRoot["vit"].asInt();
	baseInfo->Intelligence = charBaseRoot["int"].asInt();
	baseInfo->HPLevelUp = charBaseRoot["level_hp"].asInt();
	baseInfo->MPLevelUp = charBaseRoot["level_mp"].asInt();
	baseInfo->LevelUpStat = charBaseRoot["level_up_stat"].asInt();
	baseInfo->JumpForce = charBaseRoot["jump_force"].asFloat();
	baseInfo->SlidingForce = charBaseRoot["sliding_force"].asFloat();
	JsonUtil::parseThicknessInfo(charBaseRoot["thickness_box"], baseInfo->ThicknessBox);
	baseInfo->DownRecoverTime = charBaseRoot["down_recover_time"].asFloat();

	SGString weaponType = JsonUtil::getString(charBaseRoot["default_weapon_type"]);
	baseInfo->DefaultWeaponType = WeaponType::getType(weaponType);
}

void CharBaseInfoLoader::readGunnerInfo(Json::Value& gunnerBaseRoot, GunnerInfo* baseInfo) {

	baseInfo->ShotCount[WeaponType::Automatic] = gunnerBaseRoot["auto_shot_count"].asInt();
	baseInfo->ShotCount[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_shot_count"].asInt();
	baseInfo->ShotCount[WeaponType::Revolver] = gunnerBaseRoot["rev_shot_count"].asInt();
	baseInfo->ShotCount[WeaponType::HandCannon] = gunnerBaseRoot["hcan_shot_count"].asInt();
	baseInfo->ShotCount[WeaponType::Musket] = gunnerBaseRoot["musket_shot_count"].asInt();

	baseInfo->JumpShotCount[WeaponType::Automatic] = gunnerBaseRoot["auto_jump_shot_count"].asInt();
	baseInfo->JumpShotCount[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_jump_shot_count"].asInt();
	baseInfo->JumpShotCount[WeaponType::Revolver] = gunnerBaseRoot["rev_jump_shot_count"].asInt();
	baseInfo->JumpShotCount[WeaponType::HandCannon] = gunnerBaseRoot["hcan_jump_shot_count"].asInt();
	baseInfo->JumpShotCount[WeaponType::Musket] = gunnerBaseRoot["musket_jump_shot_count"].asInt();

	baseInfo->AttackSpeed[WeaponType::Automatic] = gunnerBaseRoot["auto_attack_speed"].asFloat();
	baseInfo->AttackSpeed[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_attack_speed"].asFloat();
	baseInfo->AttackSpeed[WeaponType::Revolver] = gunnerBaseRoot["rev_attack_speed"].asFloat();
	baseInfo->AttackSpeed[WeaponType::HandCannon] = gunnerBaseRoot["hcan_attack_speed"].asFloat();
	baseInfo->AttackSpeed[WeaponType::Musket] = gunnerBaseRoot["musket_attack_speed"].asFloat();


	baseInfo->JumpShotForceX[WeaponType::Automatic] = gunnerBaseRoot["auto_jump_shot_force_x"].asFloat();
	baseInfo->JumpShotForceX[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_jump_shot_force_x"].asFloat();
	baseInfo->JumpShotForceX[WeaponType::Revolver] = gunnerBaseRoot["rev_jump_shot_force_x"].asFloat();
	baseInfo->JumpShotForceX[WeaponType::HandCannon] = gunnerBaseRoot["musket_jump_shot_force_x"].asFloat();
	baseInfo->JumpShotForceX[WeaponType::Musket] = gunnerBaseRoot["hcan_jump_shot_force_x"].asFloat();

	baseInfo->JumpShotForceY[WeaponType::Automatic] = gunnerBaseRoot["auto_jump_shot_force_y"].asFloat();
	baseInfo->JumpShotForceY[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_jump_shot_force_y"].asFloat();
	baseInfo->JumpShotForceY[WeaponType::Revolver] = gunnerBaseRoot["rev_jump_shot_force_y"].asFloat();
	baseInfo->JumpShotForceY[WeaponType::HandCannon] = gunnerBaseRoot["musket_jump_shot_force_y"].asFloat();
	baseInfo->JumpShotForceY[WeaponType::Musket] = gunnerBaseRoot["hcan_jump_shot_force_y"].asFloat();

	baseInfo->JumpShotMoveSpeedX[WeaponType::Automatic] = gunnerBaseRoot["auto_jump_shot_move_speed_x"].asFloat();
	baseInfo->JumpShotMoveSpeedX[WeaponType::Bowgun] = gunnerBaseRoot["bowgun_jump_shot_move_speed_x"].asFloat();
	baseInfo->JumpShotMoveSpeedX[WeaponType::Revolver] = gunnerBaseRoot["rev_jump_shot_move_speed_x"].asFloat();
	baseInfo->JumpShotMoveSpeedX[WeaponType::HandCannon] = gunnerBaseRoot["musket_jump_shot_move_speed_x"].asFloat();
	baseInfo->JumpShotMoveSpeedX[WeaponType::Musket] = gunnerBaseRoot["hcan_jump_shot_move_speed_x"].asFloat();


}

