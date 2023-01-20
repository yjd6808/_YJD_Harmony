/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */



#include "SGProjectileInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


using namespace JCore;
using namespace Json;

void SGProjectileInfoLoader::LoadProjectileInfo(SGHashMap<int, SGProjectileInfo>& projectileInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();

	SGString path = JCore::Path::Combine(ConfigDirectory_v, "projectile_gunner.json");
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "projectile_gunner.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	reader >> root;
	Json::Value projectiles = root["projectile"];

	for (Json::Value::ArrayIndex i = 0; i < projectiles.size(); i++) {
		Value& projectile = projectiles[i];
		Value& animationListRoot = projectile["animation"];
		DebugAssertMessage(animationListRoot.size() > 0, "애니메이션이 없는 프로젝틸입니다.");
		SGProjectileInfo info(animationListRoot.size());
		info.Code = projectile["code"].asInt();
		info.Name = SGJson::getString(projectile["name"]);
		info.NpkIndex = pPackManager->getPackIndex(SGJson::getString(projectile["npk"]));
		info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getIndex(SGJson::getString(projectile["img"]));
		info.SpawnOffsetX = projectile["spawn_offset_x"].asFloat();
		info.SpawnOffsetY = projectile["spawn_offset_y"].asFloat();
		info.SpawnEffectCode = projectile["spawn_effect_code"].asInt();
		info.HitEffectCode = projectile["hit_effect_code"].asInt();
		info.Rotation = projectile["rotation"].asFloat();
		info.AttackDamageType = projectile["attack_damage_type"].asFloat();
		info.AttackDamageRatio = projectile["attack_damage_ratio"].asFloat();
		info.AttackXForce = projectile["attack_x_force"].asFloat();
		info.AttackYForce = projectile["attack_y_force"].asFloat();
		info.ApplyPhysics = projectile["apply_physics"].asBool();
		info.Weight = projectile["weight"].asFloat();
		info.Distance = projectile["distance"].asFloat();
		info.MoveSpeed = projectile["move_speed"].asFloat();
		info.PenetrationRate = projectile["penetration_rate"].asFloat();
		info.RehitDelay = projectile["rehit_delay"].asFloat();
		info.ThicknessBoxWidth = projectile["thickness_box_width"].asFloat();
		info.ThicknessBoxHeight = projectile["thickness_box_height"].asFloat();
		info.ThicknessBoxRelativeY = projectile["thickness_relative_y"].asFloat();

		for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
			Value& animationRoot = animationListRoot[j];
			SGAnimationInfo animationInfo(animationRoot["frames"].size());
			SGJson::parseAnimationInfo(animationRoot, animationInfo);
			info.Animations.PushBack(Move(animationInfo));
		}

		DebugAssertMessage(!projectileInfoMap.Exist(info.Code), "projectile에 해당 코드값이 이미 존재합니다.");
		projectileInfoMap.Insert(info.Code, info);
	}

	Log("SGProjectileInfoLoader :: 로딩완료\n");
}
