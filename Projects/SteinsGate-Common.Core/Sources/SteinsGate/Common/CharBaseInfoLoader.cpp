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

//////////////////////////////////////////////////////////////////////////////////////////
CharBaseInfoLoader::CharBaseInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CharBaseInfoLoader::Load()
{
	Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Value& gunnerRoot = root["gunner"];

		// 다른 캐릭이 만약 추가되면 코드 변경 필요
		GunnerInfo* pGunnerInfo = dbg_new GunnerInfo();
		ReadCharBaseInfo(gunnerRoot, pGunnerInfo);
		ReadGunnerInfo(gunnerRoot, pGunnerInfo);
		AddData(pGunnerInfo);
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CharBaseInfoLoader::ReadCharBaseInfo(Json::Value& _charBaseRoot, CharBaseInfo* _pBaseInfo)
{
	_pBaseInfo->code_ = CharType::Gunner;
	_pBaseInfo->hp_ = _charBaseRoot["hp"].asInt();
	_pBaseInfo->mp_ = _charBaseRoot["mp"].asInt();
	_pBaseInfo->strength_ = _charBaseRoot["str"].asInt();
	_pBaseInfo->dexterity_ = _charBaseRoot["dex"].asInt();
	_pBaseInfo->vitality_ = _charBaseRoot["vit"].asInt();
	_pBaseInfo->intelligence_ = _charBaseRoot["int"].asInt();
	_pBaseInfo->hpLevelUp_ = _charBaseRoot["level_hp"].asInt();
	_pBaseInfo->mpLevelUp_ = _charBaseRoot["level_mp"].asInt();
	_pBaseInfo->levelUpStat_ = _charBaseRoot["level_up_stat"].asInt();
	_pBaseInfo->jumpForce_ = _charBaseRoot["jump_force"].asFloat();
	_pBaseInfo->slidingForce_ = _charBaseRoot["sliding_force"].asFloat();
	JsonUtil::ParseThicknessInfo(_charBaseRoot["thickness_box"], _pBaseInfo->thicknessBox_);
	_pBaseInfo->downRecoverTime_ = _charBaseRoot["down_recover_time"].asFloat();

	SGString weaponType = JsonUtil::GetString(_charBaseRoot["default_weapon_type"]);
	_pBaseInfo->defaultWeaponType_ = WeaponType::getType(weaponType);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CharBaseInfoLoader::ReadGunnerInfo(Json::Value& _gunnerBaseRoot, GunnerInfo* _pBaseInfo)
{
	_pBaseInfo->shotCount_[WeaponType::Automatic] = _gunnerBaseRoot["auto_shot_count"].asInt();
	_pBaseInfo->shotCount_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_shot_count"].asInt();
	_pBaseInfo->shotCount_[WeaponType::Revolver] = _gunnerBaseRoot["rev_shot_count"].asInt();
	_pBaseInfo->shotCount_[WeaponType::HandCannon] = _gunnerBaseRoot["hcan_shot_count"].asInt();
	_pBaseInfo->shotCount_[WeaponType::Musket] = _gunnerBaseRoot["musket_shot_count"].asInt();

	_pBaseInfo->jumpShotCount_[WeaponType::Automatic] = _gunnerBaseRoot["auto_jump_shot_count"].asInt();
	_pBaseInfo->jumpShotCount_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_jump_shot_count"].asInt();
	_pBaseInfo->jumpShotCount_[WeaponType::Revolver] = _gunnerBaseRoot["rev_jump_shot_count"].asInt();
	_pBaseInfo->jumpShotCount_[WeaponType::HandCannon] = _gunnerBaseRoot["hcan_jump_shot_count"].asInt();
	_pBaseInfo->jumpShotCount_[WeaponType::Musket] = _gunnerBaseRoot["musket_jump_shot_count"].asInt();

	_pBaseInfo->attackSpeed_[WeaponType::Automatic] = _gunnerBaseRoot["auto_attack_speed"].asFloat();
	_pBaseInfo->attackSpeed_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_attack_speed"].asFloat();
	_pBaseInfo->attackSpeed_[WeaponType::Revolver] = _gunnerBaseRoot["rev_attack_speed"].asFloat();
	_pBaseInfo->attackSpeed_[WeaponType::HandCannon] = _gunnerBaseRoot["hcan_attack_speed"].asFloat();
	_pBaseInfo->attackSpeed_[WeaponType::Musket] = _gunnerBaseRoot["musket_attack_speed"].asFloat();

	_pBaseInfo->jumpShotForceX_[WeaponType::Automatic] = _gunnerBaseRoot["auto_jump_shot_force_x"].asFloat();
	_pBaseInfo->jumpShotForceX_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_jump_shot_force_x"].asFloat();
	_pBaseInfo->jumpShotForceX_[WeaponType::Revolver] = _gunnerBaseRoot["rev_jump_shot_force_x"].asFloat();
	_pBaseInfo->jumpShotForceX_[WeaponType::HandCannon] = _gunnerBaseRoot["musket_jump_shot_force_x"].asFloat();
	_pBaseInfo->jumpShotForceX_[WeaponType::Musket] = _gunnerBaseRoot["hcan_jump_shot_force_x"].asFloat();

	_pBaseInfo->jumpShotForceY_[WeaponType::Automatic] = _gunnerBaseRoot["auto_jump_shot_force_y"].asFloat();
	_pBaseInfo->jumpShotForceY_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_jump_shot_force_y"].asFloat();
	_pBaseInfo->jumpShotForceY_[WeaponType::Revolver] = _gunnerBaseRoot["rev_jump_shot_force_y"].asFloat();
	_pBaseInfo->jumpShotForceY_[WeaponType::HandCannon] = _gunnerBaseRoot["musket_jump_shot_force_y"].asFloat();
	_pBaseInfo->jumpShotForceY_[WeaponType::Musket] = _gunnerBaseRoot["hcan_jump_shot_force_y"].asFloat();

	_pBaseInfo->jumpShotMoveSpeedX_[WeaponType::Automatic] = _gunnerBaseRoot["auto_jump_shot_move_speed_x"].asFloat();
	_pBaseInfo->jumpShotMoveSpeedX_[WeaponType::Bowgun] = _gunnerBaseRoot["bowgun_jump_shot_move_speed_x"].asFloat();
	_pBaseInfo->jumpShotMoveSpeedX_[WeaponType::Revolver] = _gunnerBaseRoot["rev_jump_shot_move_speed_x"].asFloat();
	_pBaseInfo->jumpShotMoveSpeedX_[WeaponType::HandCannon] = _gunnerBaseRoot["musket_jump_shot_move_speed_x"].asFloat();
	_pBaseInfo->jumpShotMoveSpeedX_[WeaponType::Musket] = _gunnerBaseRoot["hcan_jump_shot_move_speed_x"].asFloat();
}
