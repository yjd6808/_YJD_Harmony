/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGCharBaseInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGCharBaseInfo.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_CC;
USING_NS_JS;

#define JsonFileName "char_base.json"

bool SGCharBaseInfoLoader::LoadCharBaseInfo(SGCharBaseInfo*(&characterInfoMap)[CharType::Max]) {
	SGGlobal* pGlobal = SGGlobal::get();
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}

	Json::Value gunnerRoot = root["gunner"];



	// 다른 캐릭이 만약 추가되면 코드 변경 필요

	SGGunnerBaseInfo* pGunnerInfo = new SGGunnerBaseInfo();
	SGGunnerBaseInfo& info = *pGunnerInfo;
	{
		characterInfoMap[CharType::Gunner] = pGunnerInfo;

		for (int i = 0; i < VisualType::Max; ++i) {
			info.DefaultVisualImgIndex[i] = InvalidValue_v;
			info.DefaultVisualNpkIndex[i] = InvalidValue_v;
		}

		info.Type = CharType::Gunner;
		info.HP = gunnerRoot["hp"].asInt();
		info.MP = gunnerRoot["mp"].asInt();
		info.Strength = gunnerRoot["str"].asInt();
		info.Dexterity = gunnerRoot["dex"].asInt();
		info.Vitality = gunnerRoot["vit"].asInt();
		info.Intelligence = gunnerRoot["int"].asInt();
		info.HPLevelUp = gunnerRoot["level_hp"].asInt();
		info.MPLevelUp = gunnerRoot["level_mp"].asInt();
		info.LevelUpStat = gunnerRoot["level_up_stat"].asInt();
		info.ShotCount[WeaponType::Auto] = gunnerRoot["auto_shot_count"].asInt();
		info.ShotCount[WeaponType::Bowgun] = gunnerRoot["bowgun_shot_count"].asInt();
		info.ShotCount[WeaponType::Revolver] = gunnerRoot["rev_shot_count"].asInt();
		info.ShotCount[WeaponType::HandCannon] = gunnerRoot["hcan_shot_count"].asInt();
		info.ShotCount[WeaponType::Musket] = gunnerRoot["musket_shot_count"].asInt();

		info.JumpShotCount[WeaponType::Auto] = gunnerRoot["auto_jump_shot_count"].asInt();
		info.JumpShotCount[WeaponType::Bowgun] = gunnerRoot["bowgun_jump_shot_count"].asInt();
		info.JumpShotCount[WeaponType::Revolver] = gunnerRoot["rev_jump_shot_count"].asInt();
		info.JumpShotCount[WeaponType::HandCannon] = gunnerRoot["hcan_jump_shot_count"].asInt();
		info.JumpShotCount[WeaponType::Musket] = gunnerRoot["musket_jump_shot_count"].asInt();

		info.AttackSpeed[WeaponType::Auto] = gunnerRoot["auto_attack_speed"].asFloat();
		info.AttackSpeed[WeaponType::Bowgun] = gunnerRoot["bowgun_attack_speed"].asFloat();
		info.AttackSpeed[WeaponType::Revolver] = gunnerRoot["rev_attack_speed"].asFloat();
		info.AttackSpeed[WeaponType::HandCannon] = gunnerRoot["hcan_attack_speed"].asFloat();
		info.AttackSpeed[WeaponType::Musket] = gunnerRoot["musket_attack_speed"].asFloat();

		
		info.JumpShotForceX[WeaponType::Auto] = gunnerRoot["auto_jump_shot_force_x"].asFloat();
		info.JumpShotForceX[WeaponType::Bowgun] = gunnerRoot["bowgun_jump_shot_force_x"].asFloat();
		info.JumpShotForceX[WeaponType::Revolver] = gunnerRoot["rev_jump_shot_force_x"].asFloat();
		info.JumpShotForceX[WeaponType::HandCannon] = gunnerRoot["musket_jump_shot_force_x"].asFloat();
		info.JumpShotForceX[WeaponType::Musket] = gunnerRoot["hcan_jump_shot_force_x"].asFloat();

		info.JumpShotForceY[WeaponType::Auto] = gunnerRoot["auto_jump_shot_force_y"].asFloat();
		info.JumpShotForceY[WeaponType::Bowgun] = gunnerRoot["bowgun_jump_shot_force_y"].asFloat();
		info.JumpShotForceY[WeaponType::Revolver] = gunnerRoot["rev_jump_shot_force_y"].asFloat();
		info.JumpShotForceY[WeaponType::HandCannon] = gunnerRoot["musket_jump_shot_force_y"].asFloat();
		info.JumpShotForceY[WeaponType::Musket] = gunnerRoot["hcan_jump_shot_force_y"].asFloat();

		info.JumpShotMoveSpeedX[WeaponType::Auto] = gunnerRoot["auto_jump_shot_move_speed_x"].asFloat();
		info.JumpShotMoveSpeedX[WeaponType::Bowgun] = gunnerRoot["bowgun_jump_shot_move_speed_x"].asFloat();
		info.JumpShotMoveSpeedX[WeaponType::Revolver] = gunnerRoot["rev_jump_shot_move_speed_x"].asFloat();
		info.JumpShotMoveSpeedX[WeaponType::HandCannon] = gunnerRoot["musket_jump_shot_move_speed_x"].asFloat();
		info.JumpShotMoveSpeedX[WeaponType::Musket] = gunnerRoot["hcan_jump_shot_move_speed_x"].asFloat();
		info.DownRecoverTime = gunnerRoot["down_recover_time"].asFloat();

		info.JumpForce = gunnerRoot["jump_force"].asFloat();
		info.SlidingForce = gunnerRoot["sliding_force"].asFloat();
		SGJson::parseThicknessInfo(gunnerRoot["thickness_box"], info.ThicknessBox);

		{
			Json::Value& zOrderRoot = gunnerRoot["visual_z_order"];
			info.DefaultVisualZOrder[VisualType::Skin] = zOrderRoot["skin"].asInt();
			info.DefaultVisualZOrder[VisualType::Shoes] = zOrderRoot["shoes"].asInt();
			info.DefaultVisualZOrder[VisualType::Pants] = zOrderRoot["pants"].asInt();
			info.DefaultVisualZOrder[VisualType::Neck] = zOrderRoot["neck"].asInt();
			info.DefaultVisualZOrder[VisualType::Hair] = zOrderRoot["hair"].asInt();
			info.DefaultVisualZOrder[VisualType::Face] = zOrderRoot["face"].asInt();
			info.DefaultVisualZOrder[VisualType::Coat] = zOrderRoot["coat"].asInt();
			info.DefaultVisualZOrder[VisualType::Cap] = zOrderRoot["cap"].asInt();
			info.DefaultVisualZOrder[VisualType::Belt] = zOrderRoot["belt"].asInt();
			info.DefaultVisualZOrder[VisualType::WeaponLeft] = zOrderRoot["weapon_left"].asInt();
			info.DefaultVisualZOrder[VisualType::WeaponRight] = zOrderRoot["weapon_right"].asInt();
		}

		{
			Json::Value& defaultAvatarImgRoot = gunnerRoot["default_visual_img"];

			info.DefaultWeaponType = WeaponType::Auto;

			SGString defaultAvatarPartImgName[VisualType::Max];

			defaultAvatarPartImgName[VisualType::Skin] =   SGJson::getString(defaultAvatarImgRoot["skin"]);
			defaultAvatarPartImgName[VisualType::Shoes] =  SGJson::getString(defaultAvatarImgRoot["shoes"]);
			defaultAvatarPartImgName[VisualType::Pants] =  SGJson::getString(defaultAvatarImgRoot["pants"]);
			defaultAvatarPartImgName[VisualType::Neck] =   SGJson::getString(defaultAvatarImgRoot["neck"]);
			defaultAvatarPartImgName[VisualType::Hair] =   SGJson::getString(defaultAvatarImgRoot["hair"]);
			defaultAvatarPartImgName[VisualType::Face] =   SGJson::getString(defaultAvatarImgRoot["face"]);
			defaultAvatarPartImgName[VisualType::Coat] =   SGJson::getString(defaultAvatarImgRoot["coat"]);
			defaultAvatarPartImgName[VisualType::Cap] =    SGJson::getString(defaultAvatarImgRoot["cap"]);
			defaultAvatarPartImgName[VisualType::Belt] =   SGJson::getString(defaultAvatarImgRoot["belt"]);
			defaultAvatarPartImgName[VisualType::WeaponLeft] = SGJson::getString(defaultAvatarImgRoot["weapon_left"]);
			defaultAvatarPartImgName[VisualType::WeaponRight] = SGJson::getString(defaultAvatarImgRoot["weapon_right"]);
			

			for (int iVisualType = AvatarType::Begin; iVisualType < AvatarType::Max; ++iVisualType) {
				const SGString& npkName = pGlobal->getAvatarNpkName(CharType::Gunner, iVisualType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);

				info.DefaultVisualNpkIndex[iVisualType] = pImgPack->getPackIndex();
				if (pImgPack->hasImgIndex(defaultAvatarPartImgName[iVisualType]))
					info.DefaultVisualImgIndex[iVisualType] = pImgPack->getImgIndex(defaultAvatarPartImgName[iVisualType]);
			}

			bool bDefaultWeaponImgFound = false;
			for (int iWeaponType = WeaponType::Begin; iWeaponType < WeaponType::Max; ++iWeaponType) {
				const SGString& npkName = pGlobal->getWeaponNpkName(CharType::Gunner, iWeaponType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);

				if (pImgPack->hasImgIndex(defaultAvatarPartImgName[VisualType::WeaponLeft])) {
					info.DefaultVisualImgIndex[VisualType::WeaponLeft] = pImgPack->getImgIndex(defaultAvatarPartImgName[VisualType::WeaponLeft]);
					info.DefaultVisualNpkIndex[VisualType::WeaponLeft] = pImgPack->getPackIndex();
				}

				if (pImgPack->hasImgIndex(defaultAvatarPartImgName[VisualType::WeaponRight])) {
					info.DefaultVisualImgIndex[VisualType::WeaponRight] = pImgPack->getImgIndex(defaultAvatarPartImgName[VisualType::WeaponRight]);
					info.DefaultVisualNpkIndex[VisualType::WeaponRight] = pImgPack->getPackIndex();
					bDefaultWeaponImgFound = true;
					break;
				}
			}

			DebugAssertMessage(bDefaultWeaponImgFound, "기본 무기 이미지가 없습니다.");

		}
		 
	}


	Log("SGCharBaseInfoLoader :: 로딩완료\n");
	return true;
}
