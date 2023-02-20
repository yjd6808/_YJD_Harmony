

 /*
  * 작성자: 윤정도
  * 생성일: 1/20/2023 1:57:14 PM
  * =====================
  *
  */

#include "Core.h"
#include "MobBaseInfoLoader.h"

#include <SteinsGate/Common/SGJson.h>

USING_NS_JS;
USING_NS_JC;

bool MobBaseInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monsterRoot = monsterListRoot[i];
			MobBaseInfo* pMobInfo = dbg_new MobBaseInfo;
			readMobBaseInfo(monsterRoot, pMobInfo);
			addData(pMobInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void MobBaseInfoLoader::readMobBaseInfo(Json::Value& monsterRoot, Out_ MobBaseInfo* mobInfo) {

	mobInfo->Code = monsterRoot["code"].asInt();
	mobInfo->Name = SGJson::getString(monsterRoot["kor_name"]);
	mobInfo->HP = monsterRoot["hp"].asInt();
	mobInfo->MP = monsterRoot.get("mp", 0).asInt();
	mobInfo->PhysicalDamage = monsterRoot["physical_damage"].asInt();
	mobInfo->MagicDamage = monsterRoot["Magic_damage"].asInt();
	mobInfo->Armor = monsterRoot.get("armor", 0).asInt();
	mobInfo->MagicArmor = monsterRoot.get("magic_armor", 0).asInt();
	mobInfo->EnhancePerLevel = monsterRoot["enhance_per_level"].asFloat();
	mobInfo->MoveSpeedX = monsterRoot.get("move_speed_x", 0).asFloat();
	mobInfo->MoveSpeedY = monsterRoot["move_speed_y"].asFloat();
	mobInfo->DownRecoverTime = monsterRoot.get("down_recover_time", 1.0).asFloat();
	mobInfo->Weight = monsterRoot["weight"].asFloat();
	SGJson::parseThicknessInfo(monsterRoot["thickness_box"], mobInfo->ThicknessBox);
}
