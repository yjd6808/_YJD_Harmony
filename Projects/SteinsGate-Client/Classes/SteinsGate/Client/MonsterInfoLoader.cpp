/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

MonsterInfoLoader::MonsterInfoLoader(DataManagerAbstract* manager)
	: MonsterBaseInfoLoader(manager)
{}

bool MonsterInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {

		Json::Value monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i) {
			Value& monterRoot = monsterListRoot[i];
			MonsterInfo* monsterInfo = dbg_new MonsterInfo();
			readMonsterBaseInfo(monterRoot, monsterInfo);
			readMonsterInfo(monterRoot, monsterInfo);
			addData(monsterInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void MonsterInfoLoader::readMonsterInfo(Json::Value& mobRoot, MonsterInfo* mobInfo) {
	JsonUtilEx::parseActorSpriteData(mobRoot["actor_sprite_data"], &mobInfo->SpriteData);
}
