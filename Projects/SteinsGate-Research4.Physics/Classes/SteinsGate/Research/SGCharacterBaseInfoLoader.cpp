/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */



#include "SGCharacterBaseInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGCharacterBaseInfo.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>



USING_NS_JS;

void SGCharacterBaseInfoLoader::LoadCharacterBaseInfo(SGCharacterBaseInfo(&characterInfoMap)[CharacterType::Max]) {
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGGlobal* pGlobal = SGGlobal::getInstance();

	SGString path = JCore::Path::Combine(ConfigDirectory_v, "character_base.json");
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "character_base.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	reader >> root;
	Json::Value gunnerRoot = root["gunner"];
	
	{
		SGCharacterBaseInfo& gunnerBaseInfo = characterInfoMap[CharacterType::Gunner];

		gunnerBaseInfo.HP = gunnerRoot["hp"].asInt();
		gunnerBaseInfo.MP = gunnerRoot["mp"].asInt();
		gunnerBaseInfo.Strength = gunnerRoot["str"].asInt();
		gunnerBaseInfo.Dexterity = gunnerRoot["dex"].asInt();
		gunnerBaseInfo.Vitality = gunnerRoot["vit"].asInt();
		gunnerBaseInfo.Intelligence = gunnerRoot["int"].asInt();
		gunnerBaseInfo.HPLevelUp = gunnerRoot["level_hp"].asInt();
		gunnerBaseInfo.MPLevelUp = gunnerRoot["level_mp"].asInt();
		gunnerBaseInfo.LevelUpStat = gunnerRoot["level_up_stat"].asInt();
		gunnerBaseInfo.ShotCount[WeaponType::Auto] = gunnerRoot["auto_shot_count"].asInt();
		gunnerBaseInfo.AttackSpeed[WeaponType::Auto] = gunnerRoot["auto_attack_speed"].asFloat();
		gunnerBaseInfo.ShotCount[WeaponType::Bowgun] = gunnerRoot["bowgun_shot_count"].asInt();
		gunnerBaseInfo.AttackSpeed[WeaponType::Bowgun] = gunnerRoot["bowgun_attack_speed"].asFloat();
		gunnerBaseInfo.ShotCount[WeaponType::Revolver] = gunnerRoot["rev_shot_count"].asInt();
		gunnerBaseInfo.AttackSpeed[WeaponType::Revolver] = gunnerRoot["rev_attack_speed"].asFloat();
		gunnerBaseInfo.ShotCount[WeaponType::HandCannon] = gunnerRoot["hcan_shot_count"].asInt();
		gunnerBaseInfo.AttackSpeed[WeaponType::HandCannon] = gunnerRoot["hcan_attack_speed"].asFloat();
		gunnerBaseInfo.ShotCount[WeaponType::Musket] = gunnerRoot["musket_shot_count"].asInt();
		gunnerBaseInfo.AttackSpeed[WeaponType::Musket] = gunnerRoot["musket_attack_speed"].asFloat();
		gunnerBaseInfo.JumpForce = gunnerRoot["jump_force"].asFloat();
		gunnerBaseInfo.SlidingForce = gunnerRoot["sliding_force"].asFloat();
		gunnerBaseInfo.ThicknessBoxWidth = gunnerRoot["thickness_box_width"].asFloat();
		gunnerBaseInfo.ThicknessBoxHeight = gunnerRoot["thickness_box_height"].asFloat();
		gunnerBaseInfo.ThicknessBoxRelativeY = gunnerRoot["thickness_box_relative_y"].asFloat();

		{
			Json::Value& zOrderRoot = gunnerRoot["avatar_z_order"];
			gunnerBaseInfo.AvatarZOrder[AvatarType::Skin] = zOrderRoot["skin"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Shoes] = zOrderRoot["shoes"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Pants] = zOrderRoot["pants"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Neck] = zOrderRoot["neck"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Hair] = zOrderRoot["hair"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Face] = zOrderRoot["face"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Coat] = zOrderRoot["coat"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Cap] = zOrderRoot["cap"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Belt] = zOrderRoot["belt"].asInt();
			gunnerBaseInfo.AvatarZOrder[AvatarType::Weapon] = zOrderRoot["weapon"].asInt();
		}

		{
			Json::Value& defaultAvatarImgRoot = gunnerRoot["default_avatar_img"];

			gunnerBaseInfo.DefaultWeaponType = WeaponType::Auto;

			SGString defaultAvatarPartImgName[AvatarType::Max];

			defaultAvatarPartImgName[AvatarType::Skin] =   SGJson::getString(defaultAvatarImgRoot["skin"]);
			defaultAvatarPartImgName[AvatarType::Shoes] =  SGJson::getString(defaultAvatarImgRoot["shoes"]);
			defaultAvatarPartImgName[AvatarType::Pants] =  SGJson::getString(defaultAvatarImgRoot["pants"]);
			defaultAvatarPartImgName[AvatarType::Neck] =   SGJson::getString(defaultAvatarImgRoot["neck"]);
			defaultAvatarPartImgName[AvatarType::Hair] =   SGJson::getString(defaultAvatarImgRoot["hair"]);
			defaultAvatarPartImgName[AvatarType::Face] =   SGJson::getString(defaultAvatarImgRoot["face"]);
			defaultAvatarPartImgName[AvatarType::Coat] =   SGJson::getString(defaultAvatarImgRoot["coat"]);
			defaultAvatarPartImgName[AvatarType::Cap] =    SGJson::getString(defaultAvatarImgRoot["cap"]);
			defaultAvatarPartImgName[AvatarType::Belt] =   SGJson::getString(defaultAvatarImgRoot["belt"]);
			defaultAvatarPartImgName[AvatarType::Weapon] = SGJson::getString(defaultAvatarImgRoot["weapon"]);

			for (int iVisualType = AvatarType::Begin; iVisualType < AvatarType::VisualMax; ++iVisualType) {
				const SGString& npkName = pGlobal->getAvatarNpkName(CharacterType::Gunner, iVisualType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);
				if (pImgPack->hasIndex(defaultAvatarPartImgName[iVisualType]))
					gunnerBaseInfo.DefaultAvatarImgIndex[iVisualType] = pImgPack->getIndex(defaultAvatarPartImgName[iVisualType]);
			}

			bool bDefaultWeaponImgFound = false;
			for (int iWeaponType = WeaponType::Begin; iWeaponType < WeaponType::Max; ++iWeaponType) {
				const SGString& npkName = pGlobal->getWeaponNpkName(CharacterType::Gunner, iWeaponType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);

				if (pImgPack->hasIndex(defaultAvatarPartImgName[AvatarType::Weapon])) {
					gunnerBaseInfo.DefaultAvatarImgIndex[AvatarType::Weapon] = pImgPack->getIndex(defaultAvatarPartImgName[AvatarType::Weapon]);
					bDefaultWeaponImgFound = true;
					break;
				}
			}

			DebugAssertMessage(bDefaultWeaponImgFound, "기본 무기 이미지가 없습니다.");

		}
		 
	}


	Log("SGCharacterBaseInfoLoader :: 로딩완료\n");
}
