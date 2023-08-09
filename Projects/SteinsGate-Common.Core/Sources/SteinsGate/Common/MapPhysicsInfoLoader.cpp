/*
 * 작성자: 윤정도
 * 생성일: 8/7/2023 11:32:58 AM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "MapPhysicsInfoLoader.h"

#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

MapPhysicsInfoLoader::MapPhysicsInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

MapPhysicsInfoLoader::~MapPhysicsInfoLoader() {
	
}

bool MapPhysicsInfoLoader::load() {
	Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& physicsListRoot = root["physics"];

		for (int i = 0; i < physicsListRoot.size(); ++i) {
			Value& physicsRoot = physicsListRoot[i];
			MapPhysicsInfo* pInfo = dbg_new MapPhysicsInfo;
			readMapPhysicsInfo(physicsRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void MapPhysicsInfoLoader::readMapPhysicsInfo(Value& mapRoot, MapPhysicsInfo* mapPhysicsInfo) {
	mapPhysicsInfo->Code = mapRoot["code"].asInt();
	mapPhysicsInfo->Gravity = mapRoot["gravity"].asFloat();
	mapPhysicsInfo->Friction = mapRoot["friction"].asFloat();
	mapPhysicsInfo->ElasticityDividedForce = mapRoot["elasticity_divided_force"].asFloat();
}
