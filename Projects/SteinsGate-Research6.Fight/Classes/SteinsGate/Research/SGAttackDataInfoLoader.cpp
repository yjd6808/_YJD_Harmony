/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */


#include "SGAttackDataInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>

USING_NS_JS;
USING_NS_JC;

#define JsonFileName "attackdata.json"

bool SGAttackDataInfoLoader::LoadAttackDataInfo(SGHashMap<int, SGAttackDataInfo>& attackDataInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
		Json::Value attackDataListRoot = root["attackdata"];

		for (int i = 0; i < attackDataListRoot.size(); ++i) {
			Value& attackDataRoot = attackDataListRoot[i];
			SGAttackDataInfo info;

			info.Code = attackDataRoot["code"].asInt();
			info.Name = SGJson::getString(attackDataRoot["name"]);
			info.AttackDamageType = (AttackDamageType_t)attackDataRoot["attack_damage_type"].asInt();
			info.AttackDamageRatio = attackDataRoot["attack_damage_ratio"].asFloat();
			info.AttackXForceDir = (AttackXForceDirection_t)attackDataRoot["attack_x_force_dir"].asInt();
			info.AttackXForce = attackDataRoot["attack_x_force"].asFloat();
			info.AttackYForce = attackDataRoot["attack_y_force"].asFloat();
			info.IsFallDownAttack = attackDataRoot["is_fall_down_attack"].asBool();
			attackDataInfoMap.Insert(info.Code, Move(info));
		}
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}



	Log("SGAttackDataInfoLoader :: 로딩완료\n");

	return true;
}