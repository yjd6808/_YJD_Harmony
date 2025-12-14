/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AttackDataInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

#include <JCore/FileSystem/Path.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
AttackDataInfoLoader::AttackDataInfoLoader(DataManagerAbstract* _pManager, ActorType_t _actorType)
	: ConfigFileLoaderAbstract(_pManager)
	, actorType_(_actorType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AttackDataInfoLoader::load()
{
	Json::Value root;

	if (!loadJson(root))
		return false;

	try
	{
		Json::Value attackDataListRoot = root["attackdata"];

		for (int i = 0; i < attackDataListRoot.size(); ++i)
		{
			Value& attackDataRoot = attackDataListRoot[i];
			AttackDataInfo* pInfo = dbg_new AttackDataInfo;
			readAttackDataInfo(attackDataRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackDataInfoLoader::readAttackDataInfo(Json::Value& _attackDataRoot, AttackDataInfo* _pAttackDataInfo)
{
	_pAttackDataInfo->Code = _attackDataRoot["code"].asInt();
	_pAttackDataInfo->Name = JsonUtilEx::getString(_attackDataRoot["name"]);
	_pAttackDataInfo->AttackDamageType = (AttackDamageType_t)_attackDataRoot["attack_damage_type"].asInt();
	_pAttackDataInfo->AttackDamageRatio = _attackDataRoot["attack_damage_ratio"].asFloat();
	_pAttackDataInfo->AttackXForceDir = (AttackXForceDirection_t)_attackDataRoot["attack_x_force_dir"].asInt();
	_pAttackDataInfo->AttackXForce = _attackDataRoot["attack_x_force"].asFloat();
	_pAttackDataInfo->AttackYForce = _attackDataRoot["attack_y_force"].asFloat();
	_pAttackDataInfo->IsFallDownAttack = _attackDataRoot["is_fall_down_attack"].asBool();
}
