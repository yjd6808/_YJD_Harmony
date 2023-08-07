

 /*
  * 작성자: 윤정도
  * 생성일: 1/20/2023 1:57:14 PM
  * =====================
  *
  */

#include "Core.h"
#include "MonsterStatInfoLoader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

MonsterStatInfoLoader::MonsterStatInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool MonsterStatInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Value& monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monsterRoot = monsterListRoot[i];
			MonsterStatInfo* pInfo = dbg_new MonsterStatInfo;
			readMobStatInfo(monsterRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void MonsterStatInfoLoader::readMobStatInfo(Json::Value& monsterRoot, JCORE_OUT MonsterStatInfo* mobStatInfo) {
	mobStatInfo->Code = monsterRoot["code"].asInt();
	mobStatInfo->HP = monsterRoot["hp"].asInt();
	mobStatInfo->MP = monsterRoot.get("mp", 0).asInt();
	mobStatInfo->PhysicalDamage = monsterRoot["physical_damage"].asInt();
	mobStatInfo->MagicDamage = monsterRoot["magical_damage"].asInt();
	mobStatInfo->Armor = monsterRoot.get("armor", 0).asInt();
	mobStatInfo->MagicArmor = monsterRoot.get("magic_armor", 0).asInt();
	mobStatInfo->MoveSpeedX = monsterRoot.get("move_speed_x", 0).asFloat();
	mobStatInfo->MoveSpeedY = monsterRoot["move_speed_y"].asFloat();
	mobStatInfo->DownRecoverTime = monsterRoot.get("down_recover_time", 1.0).asFloat();
	mobStatInfo->Weight = monsterRoot["weight"].asFloat();
}
