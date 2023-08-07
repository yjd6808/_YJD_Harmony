

 /*
  * 작성자: 윤정도
  * 생성일: 1/20/2023 1:57:14 PM
  * =====================
  *
  */

#include "Core.h"
#include "MonsterBaseInfoLoader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

MonsterBaseInfoLoader::MonsterBaseInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool MonsterBaseInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monsterRoot = monsterListRoot[i];
			MonsterBaseInfo* pMobInfo = dbg_new MonsterBaseInfo;
			readMonsterBaseInfo(monsterRoot, pMobInfo);
			addData(pMobInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void MonsterBaseInfoLoader::readMonsterBaseInfo(Json::Value& monsterRoot, JCORE_OUT MonsterBaseInfo* mobInfo) {

	mobInfo->Code = monsterRoot["code"].asInt();
	mobInfo->Name = JsonUtil::getString(monsterRoot["kor_name"]);
	JsonUtil::parseThicknessInfo(monsterRoot["thickness_box"], mobInfo->ThicknessBox);
}
