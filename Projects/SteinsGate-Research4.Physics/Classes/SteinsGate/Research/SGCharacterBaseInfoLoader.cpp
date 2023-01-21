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

		for (int i = 0; i < VisualType::Max; ++i) {
			gunnerBaseInfo.DefaultVisualImgIndex[i] = InvalidIndex_v;
			gunnerBaseInfo.DefaultVisualNpkIndex[i] = InvalidIndex_v;
		}

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
			Json::Value& zOrderRoot = gunnerRoot["visual_z_order"];
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Skin] = zOrderRoot["skin"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Shoes] = zOrderRoot["shoes"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Pants] = zOrderRoot["pants"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Neck] = zOrderRoot["neck"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Hair] = zOrderRoot["hair"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Face] = zOrderRoot["face"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Coat] = zOrderRoot["coat"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Cap] = zOrderRoot["cap"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Belt] = zOrderRoot["belt"].asInt();
			gunnerBaseInfo.DefaultVisualZOrder[VisualType::Weapon] = zOrderRoot["weapon"].asInt();
		}

		{
			Json::Value& defaultAvatarImgRoot = gunnerRoot["default_visual_img"];

			gunnerBaseInfo.DefaultWeaponType = WeaponType::Auto;

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
			defaultAvatarPartImgName[VisualType::Weapon] = SGJson::getString(defaultAvatarImgRoot["weapon"]);

			for (int iVisualType = AvatarType::Begin; iVisualType < AvatarType::Max; ++iVisualType) {
				const SGString& npkName = pGlobal->getAvatarNpkName(CharacterType::Gunner, iVisualType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);
				gunnerBaseInfo.DefaultVisualNpkIndex[iVisualType] = pImgPack->getPackIndex();
				if (pImgPack->hasImgIndex(defaultAvatarPartImgName[iVisualType]))
					gunnerBaseInfo.DefaultVisualImgIndex[iVisualType] = pImgPack->getImgIndex(defaultAvatarPartImgName[iVisualType]);
			}

			bool bDefaultWeaponImgFound = false;
			for (int iWeaponType = WeaponType::Begin; iWeaponType < WeaponType::Max; ++iWeaponType) {
				const SGString& npkName = pGlobal->getWeaponNpkName(CharacterType::Gunner, iWeaponType);
				SGImagePack* pImgPack = pPackManager->getPack(npkName);

				if (pImgPack->hasImgIndex(defaultAvatarPartImgName[VisualType::Weapon])) {
					gunnerBaseInfo.DefaultVisualImgIndex[VisualType::Weapon] = pImgPack->getImgIndex(defaultAvatarPartImgName[VisualType::Weapon]);
					gunnerBaseInfo.DefaultVisualNpkIndex[VisualType::Weapon] = pImgPack->getPackIndex();
					bDefaultWeaponImgFound = true;
					break;
				}
			}

			DebugAssertMessage(bDefaultWeaponImgFound, "기본 무기 이미지가 없습니다.");

		}
		 
	}


	Log("SGCharacterBaseInfoLoader :: 로딩완료\n");
}
