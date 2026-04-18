/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "DescMgr_Projectile.h"

#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sg/Util/DescLoaderMgr.h"
#include "sgcl/Util/JsonUtilEx.h"

USING_NS_JC;
USING_NS_JS;

ProjectileInfoLoader::ProjectileInfoLoader(ActorType_t _actorType)
: actorType_(_actorType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ProjectileInfoLoader::Load()
{
	Json::Value jsonRoot;

	if (!LoadJson(jsonRoot))
	{
		return false;
	}

	try
	{
		Json::Value projectileArray = jsonRoot["projectile"];

		for (Json::Value::ArrayIndex projectileIndex = 0; projectileIndex < projectileArray.size(); ++projectileIndex)
		{
			Json::Value& projectileValue = projectileArray[projectileIndex];
			int overrideCode = JsonUtilEx::GetIntDefault(projectileValue["override_code"]);

			// 오버라이드 읽기
			if (overrideCode != 0)
			{
				jc_assert_msg(configDataMap_.Exist(overrideCode), "오버라이딩할 프로젝틸 데이터가 없습니다. 문서 똑바로 안만들어!!?");

				ProjectileInfo& referenceProjectileInfo = static_cast<ProjectileInfo&>(*configDataMap_[overrideCode]);
				ProjectileInfo* pProjectileInfo = dbg_new ProjectileInfo(referenceProjectileInfo);
				pProjectileInfo->isSpriteDataRef_ = true;
				ReadOverridedProjectileInfo(projectileValue, pProjectileInfo);
				AddData(pProjectileInfo);
				continue;
			}

			// 원본 읽기
			ProjectileInfo* pProjectileInfo = dbg_new ProjectileInfo;
			pProjectileInfo->isSpriteDataRef_ = false;
			ReadProjectileInfo(projectileValue, pProjectileInfo);
			AddData(pProjectileInfo);
		}
	}
	catch (std::exception& exception)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), exception.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileInfoLoader::ReadOverridedProjectileInfo(Json::Value& _projectileRoot,
                                                       OUT ProjectileInfo* _pProjectileInfo)
{
	ImagePackManager* pPackManager = ImagePackManager::Get();
	DescLoaderMgr* pDataManager = DescLoaderMgr::Get();

	(void)pPackManager; // currently only used by commented-out code

	_pProjectileInfo->code_ = _projectileRoot["code"].asInt();
	_pProjectileInfo->name_ = JsonUtilEx::GetString(_projectileRoot["name"]);

	int attackDataCode = JsonUtilEx::GetIntDefault(_projectileRoot["attack_data_code"], 0);
	if (attackDataCode != 0)
	{
		_pProjectileInfo->attackData_ = pDataManager->GetAttackDataInfo(actorType_, attackDataCode);
	}

	int projectileListenerCode = JsonUtilEx::GetIntDefault(_projectileRoot["listener_code"], 0);
	if (projectileListenerCode != 0)
	{
		_pProjectileInfo->listenerCode_ = projectileListenerCode;
	}

	/*const jc::String& sgaName = JsonUtilEx::getStringOrNull(_projectileRoot["sga"]);
	if (!sgaName.IsNull())
	{
	    _pProjectileInfo->SgaIndex = pPackManager->getPackIndex(sgaName);
	}

	const jc::String& imgName = JsonUtilEx::getStringOrNull(_projectileRoot["img"]);
	if (!imgName.IsNull())
	{
	    _pProjectileInfo->ImgIndex = pPackManager->getPack(_pProjectileInfo->SgaIndex)->getImgIndex(imgName);
	}*/

	float spawnOffsetX = JsonUtilEx::GetFloatDefault(_projectileRoot["spawn_offset_x"], 0);
	if (static_cast<int>(spawnOffsetX) != 0)
	{
		_pProjectileInfo->spawnOffsetX_ = spawnOffsetX;
	}

	float spawnOffsetY = JsonUtilEx::GetFloatDefault(_projectileRoot["spawn_offset_y"], 0);
	if (static_cast<int>(spawnOffsetY) != 0)
	{
		_pProjectileInfo->spawnOffsetY_ = spawnOffsetY;
	}

	int spawnEffectCode = JsonUtilEx::GetIntDefault(_projectileRoot["spawn_effect_code"], 0);
	if (spawnEffectCode != 0)
	{
		_pProjectileInfo->spawnEffect_ = pDataManager->GetEffectInfo(spawnEffectCode);
	}

	float spawnEffectOffsetX = JsonUtilEx::GetFloatDefault(_projectileRoot["spawn_effect_offset_x"], 0);
	if (static_cast<int>(spawnEffectOffsetX) != 0)
	{
		_pProjectileInfo->spawnEffectOffsetX_ = spawnEffectOffsetX;
	}

	float spawnEffectOffsetY = JsonUtilEx::GetFloatDefault(_projectileRoot["spawn_effect_offset_y"], 0);
	if (static_cast<int>(spawnEffectOffsetY) != 0)
	{
		_pProjectileInfo->spawnEffectOffsetY_ = spawnEffectOffsetY;
	}

	int hitEffectCode = JsonUtilEx::GetIntDefault(_projectileRoot["hit_effect_code"], 0);
	if (hitEffectCode != 0)
	{
		_pProjectileInfo->hitEffect_ = pDataManager->GetEffectInfo(hitEffectCode);
	}

	float rotation = JsonUtilEx::GetFloatDefault(_projectileRoot["rotation"], 400);
	if (rotation < 360)
	{
		_pProjectileInfo->rotation_ = rotation;
	}

	if (_projectileRoot.isMember("random_rotation_range"))
	{
		JsonUtilEx::ParseFloatNumber2(_projectileRoot["random_rotation_range"],
		                              _pProjectileInfo->ramdomRotationRangeMin_,
		                              _pProjectileInfo->ramdomRotationRangeMax_);
	}

	float distance = JsonUtilEx::GetFloatDefault(_projectileRoot["distance"], 0);
	if (static_cast<int>(distance) != 0)
	{
		_pProjectileInfo->distance_ = distance;
	}

	float moveSpeed = JsonUtilEx::GetFloatDefault(_projectileRoot["move_speed"], 0);
	if (static_cast<int>(moveSpeed) != 0)
	{
		_pProjectileInfo->moveSpeed_ = moveSpeed;
	}

	float lifeTime = JsonUtilEx::GetFloatDefault(_projectileRoot["life_time"], 0);
	if (static_cast<int>(lifeTime) != 0)
	{
		_pProjectileInfo->lifeTime_ = lifeTime;
	}

	float rehitDelay = JsonUtilEx::GetFloatDefault(_projectileRoot["rehit_delay"], -1);
	if (rehitDelay >= 0)
	{
		_pProjectileInfo->rehitDelay_ = rehitDelay;
	}

	if (_projectileRoot.isMember("thickness_box"))
	{
		JsonUtilEx::ParseThicknessInfo(_projectileRoot["thickness_box"], _pProjectileInfo->thicknessBox_);
	}

	// 애니메이션 없으면 종료
	if (!_projectileRoot.isMember("animation"))
	{
		return;
	}

	_pProjectileInfo->isSpriteDataRef_ = false;
	JsonUtilEx::ParseActorSpriteData(_projectileRoot["actor_sprite_data"], &_pProjectileInfo->spriteData_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileInfoLoader::ReadProjectileInfo(Json::Value& _projectileRoot, OUT ProjectileInfo* _pProjectileInfo)
{
	// 초기화 안된 변수가 없어야함
	ImagePackManager* pPackManager = ImagePackManager::Get();
	DescLoaderMgr* pDataManager = DescLoaderMgr::Get();

	(void)pPackManager; // currently only used by commented-out code

	_pProjectileInfo->code_ = _projectileRoot["code"].asInt();
	_pProjectileInfo->listenerCode_ = _projectileRoot["listener_code"].asInt();
	_pProjectileInfo->attackData_ = pDataManager->GetAttackDataInfo(
		actorType_, _projectileRoot["attack_data_code"].asInt());
	_pProjectileInfo->name_ = JsonUtilEx::GetString(_projectileRoot["name"]);
	//_pProjectileInfo->SgaIndex = pPackManager->getPackIndex(JsonUtilEx::getString(_projectileRoot["sga"]));
	//_pProjectileInfo->ImgIndex = pPackManager->getPack(_pProjectileInfo->SgaIndex)->getImgIndex(JsonUtilEx::getString(_projectileRoot["img"]));
	_pProjectileInfo->spawnOffsetX_ = _projectileRoot["spawn_offset_x"].asFloat();
	_pProjectileInfo->spawnOffsetY_ = _projectileRoot["spawn_offset_y"].asFloat();
	_pProjectileInfo->spawnEffect_ = pDataManager->GetEffectInfo(_projectileRoot["spawn_effect_code"].asInt());
	_pProjectileInfo->spawnEffectOffsetX_ = _projectileRoot["spawn_effect_offset_x"].asFloat();
	_pProjectileInfo->spawnEffectOffsetY_ = _projectileRoot["spawn_effect_offset_y"].asFloat();
	_pProjectileInfo->hitEffect_ = pDataManager->GetEffectInfo(_projectileRoot["hit_effect_code"].asInt());
	_pProjectileInfo->rotation_ = _projectileRoot["rotation"].asFloat();
	JsonUtilEx::ParseFloatNumber2(_projectileRoot["random_rotation_range"], _pProjectileInfo->ramdomRotationRangeMin_,
	                              _pProjectileInfo->ramdomRotationRangeMax_);
	_pProjectileInfo->distance_ = _projectileRoot["distance"].asFloat();
	_pProjectileInfo->moveSpeed_ = _projectileRoot["move_speed"].asFloat();
	_pProjectileInfo->lifeTime_ = _projectileRoot["life_time"].asFloat();
	_pProjectileInfo->rehitDelay_ = _projectileRoot["rehit_delay"].asFloat();
	JsonUtilEx::ParseThicknessInfo(_projectileRoot["thickness_box"], _pProjectileInfo->thicknessBox_);
	JsonUtilEx::ParseActorSpriteData(_projectileRoot["actor_sprite_data"], &_pProjectileInfo->spriteData_);
}
