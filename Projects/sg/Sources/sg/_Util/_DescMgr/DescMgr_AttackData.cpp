/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_AttackData.h"

#include <sgcl/ImagePackManager.h>
#include <sgcl/Global.h>
#include <sgcl/JsonUtilEx.h>

#include <jc/FileSystem/Path.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
AttackDataInfoLoader::AttackDataInfoLoader(DescLoaderMgr* _pManager, ActorType_t _actorType)
: DescLoaderAbstract(_pManager)
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

		for (size_t i = 0; i < attackDataListRoot.size(); ++i)
		{
			Value& attackDataRoot = attackDataListRoot[(ArrayIndex)i];
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

	Loaded();
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
