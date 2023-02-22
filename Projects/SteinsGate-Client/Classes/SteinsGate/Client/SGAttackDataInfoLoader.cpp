/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAttackDataInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJsonEx.h>

#include <JCore/FileSystem/Path.h>

USING_NS_JS;
USING_NS_JC;

SGAttackDataInfoLoader::SGAttackDataInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGAttackDataInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;
	try {
		Json::Value attackDataListRoot = root["attackdata"];

		for (int i = 0; i < attackDataListRoot.size(); ++i) {
			Value& attackDataRoot = attackDataListRoot[i];
			SGAttackDataInfo* pInfo = dbg_new SGAttackDataInfo;
			readAttackDataInfo(attackDataRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void SGAttackDataInfoLoader::readAttackDataInfo(Json::Value& attackDataRoot, SGAttackDataInfo* attackDataInfo) {
	attackDataInfo->Code = attackDataRoot["code"].asInt();
	attackDataInfo->Name = SGJsonEx::getString(attackDataRoot["name"]);
	attackDataInfo->AttackDamageType = (AttackDamageType_t)attackDataRoot["attack_damage_type"].asInt();
	attackDataInfo->AttackDamageRatio = attackDataRoot["attack_damage_ratio"].asFloat();
	attackDataInfo->AttackXForceDir = (AttackXForceDirection_t)attackDataRoot["attack_x_force_dir"].asInt();
	attackDataInfo->AttackXForce = attackDataRoot["attack_x_force"].asFloat();
	attackDataInfo->AttackYForce = attackDataRoot["attack_y_force"].asFloat();
	attackDataInfo->IsFallDownAttack = attackDataRoot["is_fall_down_attack"].asBool();
}
