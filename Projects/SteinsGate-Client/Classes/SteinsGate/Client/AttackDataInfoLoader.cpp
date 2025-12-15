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
bool AttackDataInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;
	try
	{
		Json::Value attackDataListRoot = root["attackdata"];

		for (int i = 0; i < attackDataListRoot.size(); ++i)
		{
			Value& attackDataRoot = attackDataListRoot[i];
			AttackDataInfo* pInfo = dbg_new AttackDataInfo;
			ReadAttackDataInfo(attackDataRoot, pInfo);
			AddData(pInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackDataInfoLoader::ReadAttackDataInfo(Json::Value& _attackDataRoot, AttackDataInfo* _pAttackDataInfo)
{
	_pAttackDataInfo->code_ = _attackDataRoot["code"].asInt();
	_pAttackDataInfo->name_ = JsonUtilEx::GetString(_attackDataRoot["name"]);
	_pAttackDataInfo->attackDamageType_ = (AttackDamageType_t)_attackDataRoot["attack_damage_type"].asInt();
	_pAttackDataInfo->attackDamageRatio_ = _attackDataRoot["attack_damage_ratio"].asFloat();
	_pAttackDataInfo->attackXForceDir_ = (AttackXForceDirection_t)_attackDataRoot["attack_x_force_dir"].asInt();
	_pAttackDataInfo->attackXForce_ = _attackDataRoot["attack_x_force"].asFloat();
	_pAttackDataInfo->attackYForce_ = _attackDataRoot["attack_y_force"].asFloat();
	_pAttackDataInfo->isFallDownAttack_ = _attackDataRoot["is_fall_down_attack"].asBool();
}
