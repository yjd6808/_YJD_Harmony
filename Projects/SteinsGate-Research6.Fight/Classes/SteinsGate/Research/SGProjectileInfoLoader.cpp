/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */



#include "SGProjectileInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>




USING_NS_JC;
USING_NS_JS;

#define JsonFileName "projectile.json"

bool SGProjectileInfoLoader::LoadProjectileInfo(SGHashMap<int, SGProjectileInfo>& projectileInfoMap) {
	
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
		Json::Value projectiles = root["projectile"];

		for (Json::Value::ArrayIndex i = 0; i < projectiles.size(); i++) {
			Value& projectile = projectiles[i];

			int iOverride = SGJson::getIntDefault(projectile["override_code"]);

			if (iOverride != 0) {
				DebugAssertMessage(projectileInfoMap.Exist(iOverride), "오버라이딩할 프로젝틸 데이터가 없습니다. 문서 똑바로 안만들어!!?");
				SGProjectileInfo& ref = projectileInfoMap[iOverride];
				SGProjectileInfo info(ref);	// 복사 수행
				info.AnimationRef = true;
				WriteOverridedProjectileInfo(projectile, info);
				projectileInfoMap.Insert(info.Code, Move(info));
				continue;
			}

			Value& animationListRoot = projectile["animation"];
			DebugAssertMessage(animationListRoot.size() > 0, "애니메이션이 없는 프로젝틸입니다.");
			SGProjectileInfo info(animationListRoot.size());
			info.AnimationRef = false;
			WriteProjectileInfo(projectile, info);

			DebugAssertMessage(!projectileInfoMap.Exist(info.Code), "projectile에 해당 코드값이 이미 존재합니다.");
			projectileInfoMap.Insert(info.Code, Move(info));
		}
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}
	Log("SGProjectileInfoLoader :: 로딩완료\n");
	return true;
}


void SGProjectileInfoLoader::WriteOverridedProjectileInfo(Json::Value& projectile, SGProjectileInfo& info) {

	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGDataManager* pDataManager = SGDataManager::getInstance();

	info.Code = projectile["code"].asInt();
	info.Name = SGJson::getString(projectile["name"]);

	int iAttackDataCode = SGJson::getIntDefault(projectile["attakdata_code"], 0);
	if (iAttackDataCode != 0)
		info.AttackData = pDataManager->getAttackDataInfo(iAttackDataCode);

	int iProjectileListenerCode = SGJson::getIntDefault(projectile["listener_code"], 0);
	if (iProjectileListenerCode != 0)
		info.ProjectileListenerCode = iProjectileListenerCode;

	const SGString& npkName = SGJson::getStringDefault(projectile["npk"]);
	if (!npkName.IsNull()) {
		info.NpkIndex = pPackManager->getPackIndex(npkName);
	}

	const SGString& imgName = SGJson::getStringDefault(projectile["img"]);
	if (!imgName.IsNull()) {
		info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getImgIndex(imgName);
	}

	float fSpawnOffsetX = SGJson::getFloatDefault(projectile["spawn_offset_x"], 0);
	if (fSpawnOffsetX != 0)
		info.SpawnOffsetX = fSpawnOffsetX;

	float fSpawnOffsetY = SGJson::getFloatDefault(projectile["spawn_offset_y"], 0);
	if (fSpawnOffsetY != 0)
		info.SpawnOffsetY = fSpawnOffsetY;

	int iSpawnEffectCode = SGJson::getIntDefault(projectile["spawn_effect_code"], 0);
	if (iSpawnEffectCode != 0)
		info.SpawnEffectCode = iSpawnEffectCode;

	int iHitEffectCode = SGJson::getIntDefault(projectile["hit_effect_code"], 0);
	if (iHitEffectCode != 0)
		info.HitEffectCode = iHitEffectCode;

	float fRotation = SGJson::getFloatDefault(projectile["rotation"], 400);
	if (fRotation < 360)
		info.Rotation = fRotation;

	if (projectile.isMember("random_rotation_range")) {
		SGJson::parseFloatNumber2(projectile["random_rotation_range"], info.RamdomRotationRangeMin, info.RamdomRotationRangeMax);
	}

	float fDistance = SGJson::getFloatDefault(projectile["distance"], 0);
	if (fDistance != 0)
		info.Distance = fDistance;

	float fMoveSpeed = SGJson::getFloatDefault(projectile["move_speed"], 0);
	if (fMoveSpeed != 0)
		info.MoveSpeed = fMoveSpeed;

	float fLifeTime = SGJson::getFloatDefault(projectile["life_time"], 0);
	if (fLifeTime != 0)
		info.LifeTime = fLifeTime;

	float fRehitDelay = SGJson::getFloatDefault(projectile["rehit_delay"], -1);
	if (fRehitDelay >= 0)
		info.RehitDelay = fRehitDelay;

	if (projectile.isMember("thickness_box")) {
		SGJson::parseThicknessInfo(projectile["thickness_box"], info.ThicknessBox);
	}

	// 애니메이션 없으면 종료
	if (!projectile.isMember("animation")) {
		return;
	}

	Value& animationListRoot = projectile["animation"];
	info.AnimationRef = false;

	for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
		Value& animationRoot = animationListRoot[j];
		SGAnimationInfo* pAnimationInfo = new SGAnimationInfo(animationRoot["frames"].size());
		SGJson::parseAnimationInfo(animationRoot, *pAnimationInfo);
		info.AnimationList.PushBack(pAnimationInfo);
	}
}

void SGProjectileInfoLoader::WriteProjectileInfo(Json::Value& projectile, SGProjectileInfo& info) {
	// 초기화 안된 변수가 없어야함 
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGDataManager* pDataManager = SGDataManager::getInstance();


	Value& animationListRoot = projectile["animation"];
	info.Code = projectile["code"].asInt();
	info.ProjectileListenerCode = projectile["listener_code"].asInt();
	info.AttackData = pDataManager->getAttackDataInfo(projectile["attakdata_code"].asInt());
	info.Name = SGJson::getString(projectile["name"]);
	info.NpkIndex = pPackManager->getPackIndex(SGJson::getString(projectile["npk"]));
	info.ImgIndex = pPackManager->getPack(info.NpkIndex)->getImgIndex(SGJson::getString(projectile["img"]));
	info.SpawnOffsetX = projectile["spawn_offset_x"].asFloat();
	info.SpawnOffsetY = projectile["spawn_offset_y"].asFloat();
	info.SpawnEffectCode = projectile["spawn_effect_code"].asInt();
	info.HitEffectCode = projectile["hit_effect_code"].asInt();
	info.Rotation = projectile["rotation"].asFloat();
	SGJson::parseFloatNumber2(projectile["random_rotation_range"], info.RamdomRotationRangeMin, info.RamdomRotationRangeMax);
	info.Distance = projectile["distance"].asFloat();
	info.MoveSpeed = projectile["move_speed"].asFloat();
	info.LifeTime = projectile["life_time"].asFloat();
	info.RehitDelay = projectile["rehit_delay"].asFloat();
	SGJson::parseThicknessInfo(projectile["thickness_box"], info.ThicknessBox);

	for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
		Value& animationRoot = animationListRoot[j];
		SGAnimationInfo* pAnimationInfo = new SGAnimationInfo(animationRoot["frames"].size());
		SGJson::parseAnimationInfo(animationRoot, *pAnimationInfo);
		info.AnimationList.PushBack(pAnimationInfo);
	}
}
