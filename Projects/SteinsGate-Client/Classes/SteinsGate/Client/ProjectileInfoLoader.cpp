/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "ProjectileInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JC;
USING_NS_JS;

ProjectileInfoLoader::ProjectileInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ProjectileInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value projectiles = root["projectile"];

		for (Json::Value::ArrayIndex i = 0; i < projectiles.size(); i++) {
			Value& projectile = projectiles[i];
			int iOverride = JsonUtilEx::getIntDefault(projectile["override_code"]);

			// 오버라이드 읽기
			if (iOverride != 0) {
				DebugAssertMsg(m_hConfigDataAbstract.Exist(iOverride), "오버라이딩할 프로젝틸 데이터가 없습니다. 문서 똑바로 안만들어!!?");
				const ProjectileInfo& ref = static_cast<ProjectileInfo&>( *m_hConfigDataAbstract[iOverride] );
				ProjectileInfo* pInfo = dbg_new ProjectileInfo(ref);
				pInfo->AnimationRef = true;
				readOverridedProjectileInfo(projectile, pInfo);
				addData(pInfo);
				continue;
			}

			// 원본 읽기
			Value& animationListRoot = projectile["animation"];
			DebugAssertMsg(animationListRoot.size() > 0, "애니메이션이 없는 프로젝틸입니다.");
			ProjectileInfo* pInfo = dbg_new ProjectileInfo;
			pInfo->AnimationRef = false;
			readProjectileInfo(projectile, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void ProjectileInfoLoader::readOverridedProjectileInfo(Json::Value& projectileRoot, Out_ ProjectileInfo* projectileInfo) {

	ImagePackManager* pPackManager = ImagePackManager::Get();
	DataManager* pDataManager = DataManager::Get();

	projectileInfo->Code = projectileRoot["code"].asInt();
	projectileInfo->Name = JsonUtilEx::getString(projectileRoot["name"]);

	int iAttackDataCode = JsonUtilEx::getIntDefault(projectileRoot["attakdata_code"], 0);
	if (iAttackDataCode != 0)
		projectileInfo->AttackData = pDataManager->getAttackDataInfo(iAttackDataCode);

	int iProjectileListenerCode = JsonUtilEx::getIntDefault(projectileRoot["listener_code"], 0);
	if (iProjectileListenerCode != 0)
		projectileInfo->ProjectileListenerCode = iProjectileListenerCode;

	const SGString& sgaName = JsonUtilEx::getStringOrNull(projectileRoot["sga"]);
	if (!sgaName.IsNull()) {
		projectileInfo->SgaIndex = pPackManager->getPackIndex(sgaName);
	}

	const SGString& imgName = JsonUtilEx::getStringOrNull(projectileRoot["img"]);
	if (!imgName.IsNull()) {
		projectileInfo->ImgIndex = pPackManager->getPack(projectileInfo->SgaIndex)->getImgIndex(imgName);
	}

	float fSpawnOffsetX = JsonUtilEx::getFloatDefault(projectileRoot["spawn_offset_x"], 0);
	if ((int)fSpawnOffsetX != 0)
		projectileInfo->SpawnOffsetX = fSpawnOffsetX;

	float fSpawnOffsetY = JsonUtilEx::getFloatDefault(projectileRoot["spawn_offset_y"], 0);
	if ((int)fSpawnOffsetY != 0)
		projectileInfo->SpawnOffsetY = fSpawnOffsetY;

	int iSpawnEffectCode = JsonUtilEx::getIntDefault(projectileRoot["spawn_effect_code"], 0);
	if (iSpawnEffectCode != 0)
		projectileInfo->SpawnEffect = pDataManager->getEffectInfo(iSpawnEffectCode);


	float fSpawnEffectOffsetX = JsonUtilEx::getFloatDefault(projectileRoot["spawn_effect_offset_x"], 0);
	if ((int)fSpawnEffectOffsetX != 0)
		projectileInfo->SpawnEffectOffsetX = fSpawnEffectOffsetX;

	float fSpawnEffectOffsetY = JsonUtilEx::getFloatDefault(projectileRoot["spawn_effect_offset_y"], 0);
	if ((int)fSpawnEffectOffsetY != 0)
		projectileInfo->SpawnEffectOffsetY = fSpawnEffectOffsetY;

	int iHitEffectCode = JsonUtilEx::getIntDefault(projectileRoot["hit_effect_code"], 0);
	if (iHitEffectCode != 0)
		projectileInfo->HitEffect = pDataManager->getEffectInfo(iHitEffectCode);

	float fRotation = JsonUtilEx::getFloatDefault(projectileRoot["rotation"], 400);
	if (fRotation < 360)
		projectileInfo->Rotation = fRotation;

	if (projectileRoot.isMember("random_rotation_range")) {
		JsonUtilEx::parseFloatNumber2(projectileRoot["random_rotation_range"], projectileInfo->RamdomRotationRangeMin, projectileInfo->RamdomRotationRangeMax);
	}

	float fDistance = JsonUtilEx::getFloatDefault(projectileRoot["distance"], 0);
	if ((int)fDistance != 0)
		projectileInfo->Distance = fDistance;

	float fMoveSpeed = JsonUtilEx::getFloatDefault(projectileRoot["move_speed"], 0);
	if ((int)fMoveSpeed != 0)
		projectileInfo->MoveSpeed = fMoveSpeed;

	float fLifeTime = JsonUtilEx::getFloatDefault(projectileRoot["life_time"], 0);
	if ((int)fLifeTime != 0)
		projectileInfo->LifeTime = fLifeTime;

	float fRehitDelay = JsonUtilEx::getFloatDefault(projectileRoot["rehit_delay"], -1);
	if (fRehitDelay >= 0)
		projectileInfo->RehitDelay = fRehitDelay;

	if (projectileRoot.isMember("thickness_box")) {
		JsonUtilEx::parseThicknessInfo(projectileRoot["thickness_box"], projectileInfo->ThicknessBox);
	}

	// 애니메이션 없으면 종료
	if (!projectileRoot.isMember("animation")) {
		return;
	}

	projectileInfo->AnimationRef = false;
	Value& animationRoot = projectileRoot["animation"];
	AnimationInfo* pAnimationInfo = dbg_new AnimationInfo(animationRoot["frames"].size());
	JsonUtilEx::parseAnimationInfo(animationRoot, *pAnimationInfo);
	projectileInfo->Animation = pAnimationInfo;
}

void ProjectileInfoLoader::readProjectileInfo(Json::Value& projectileRoot, Out_ ProjectileInfo* projectileInfo) {
	// 초기화 안된 변수가 없어야함 
	ImagePackManager* pPackManager = ImagePackManager::Get();
	DataManager* pDataManager = DataManager::Get();


	projectileInfo->Code = projectileRoot["code"].asInt();
	projectileInfo->ProjectileListenerCode = projectileRoot["listener_code"].asInt();
	projectileInfo->AttackData = pDataManager->getAttackDataInfo(projectileRoot["attakdata_code"].asInt());
	projectileInfo->Name = JsonUtilEx::getString(projectileRoot["name"]);
	projectileInfo->SgaIndex = pPackManager->getPackIndex(JsonUtilEx::getString(projectileRoot["sga"]));
	projectileInfo->ImgIndex = pPackManager->getPack(projectileInfo->SgaIndex)->getImgIndex(JsonUtilEx::getString(projectileRoot["img"]));
	projectileInfo->SpawnOffsetX = projectileRoot["spawn_offset_x"].asFloat();
	projectileInfo->SpawnOffsetY = projectileRoot["spawn_offset_y"].asFloat();
	projectileInfo->SpawnEffect = pDataManager->getEffectInfo(projectileRoot["spawn_effect_code"].asInt());
	projectileInfo->SpawnEffectOffsetX = projectileRoot["spawn_effect_offset_x"].asFloat();
	projectileInfo->SpawnEffectOffsetY = projectileRoot["spawn_effect_offset_y"].asFloat();
	projectileInfo->HitEffect = pDataManager->getEffectInfo(projectileRoot["hit_effect_code"].asInt());
	projectileInfo->Rotation = projectileRoot["rotation"].asFloat();
	JsonUtilEx::parseFloatNumber2(projectileRoot["random_rotation_range"], projectileInfo->RamdomRotationRangeMin, projectileInfo->RamdomRotationRangeMax);
	projectileInfo->Distance = projectileRoot["distance"].asFloat();
	projectileInfo->MoveSpeed = projectileRoot["move_speed"].asFloat();
	projectileInfo->LifeTime = projectileRoot["life_time"].asFloat();
	projectileInfo->RehitDelay = projectileRoot["rehit_delay"].asFloat();
	JsonUtilEx::parseThicknessInfo(projectileRoot["thickness_box"], projectileInfo->ThicknessBox);

	Value& animationRoot = projectileRoot["animation"];
	AnimationInfo* pAnimationInfo = dbg_new AnimationInfo(animationRoot["frames"].size());
	JsonUtilEx::parseAnimationInfo(animationRoot, *pAnimationInfo);
	projectileInfo->Animation = pAnimationInfo;
}
