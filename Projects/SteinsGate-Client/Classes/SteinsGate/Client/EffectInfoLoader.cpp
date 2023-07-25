/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "EffectInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

EffectInfoLoader::EffectInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool EffectInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Value& effectListRoot = root["effect"];

		for (int i = 0; i < effectListRoot.size(); ++i) {
			Value& effectRoot = effectListRoot[i];
			EffectInfo* pInfo = dbg_new EffectInfo();
			readEffectInfo(effectRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void EffectInfoLoader::readEffectInfo(Json::Value& effectRoot, EffectInfo* effectInfo) {
	effectInfo->Code = effectRoot["code"].asInt();
	effectInfo->Name = JsonUtilEx::getString(effectRoot["name"]);
	JsonUtilEx::parseActorSpriteData(effectRoot["actor_sprite_data"], &effectInfo->SpriteData);
}
