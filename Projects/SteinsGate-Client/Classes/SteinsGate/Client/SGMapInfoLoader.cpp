/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:20 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGMapInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_JC;
USING_NS_JS;

bool SGMapInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value mapListRoot = root["map"];

		for (int i = 0; i < mapListRoot.size(); ++i) {
			Value& mapRoot = mapListRoot[i];

			Value& areaRoot = mapRoot["area"];
			Value& npcListRoot = mapRoot["npc"];
			Value& obstacleListRoot = mapRoot["obstacle"];

			// TODO: NPC 추가시 수정 필요
			SGMapInfo* pInfo = dbg_new SGMapInfo((int)areaRoot.size(), /* (int)npcRoot.size() */ 1, (int)obstacleListRoot.size());
			readMapInfo(mapRoot, pInfo);
			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}
	
	return true;
}

void SGMapInfoLoader::readMapInfo(Json::Value& mapRoot, Out_ SGMapInfo* mapInfo) {

	Value& areaRoot = mapRoot["area"];
	Value& npcListRoot = mapRoot["npc"];
	Value& obstacleListRoot = mapRoot["obstacle"];

	mapInfo->Code = mapRoot["code"].asInt();
	mapInfo->Name = SGJsonEx::getString(mapRoot["name"]);
	mapInfo->MapWidth = mapRoot["map_width"].asInt();
	mapInfo->MapHeight = mapRoot["map_height"].asInt();

	// 보이는 그대로 블록 반영해주기위해
	for (int j = areaRoot.size() - 1; j >= 0; --j) {
		mapInfo->Area.PushBack(Move(SGJsonEx::getString(areaRoot[j])));
	}

	for (int j = 0; j < npcListRoot.size(); ++j) {
		// TODO: NPC 추가시 구현 필요
	}

	for (int j = 0; j < obstacleListRoot.size(); ++j) {
		Value& obstacleRoot = obstacleListRoot[j];
		SGMapObjectInfo objectInfo;
		SGJsonEx::parseIntNumber3(obstacleRoot, objectInfo.Code, objectInfo.X, objectInfo.Y);
		mapInfo->ObstacleList.PushBack(objectInfo);
	}

	mapInfo->TileWidth = mapRoot["tile_width"].asInt();
	mapInfo->TileHeight = mapRoot["tile_height"].asInt();

	Value& tileListRoot = mapRoot["tile"];

	for (int j = 0; j < mapInfo->TileHeight; ++j) {
		Value& tileRoot = tileListRoot[j];
		SGJsonEx::parseIntNumberN(tileRoot, mapInfo->TileArray[j], mapInfo->TileWidth);
	}

	mapInfo->Gravity = mapRoot["gravity"].asFloat();
	mapInfo->Friction = mapRoot["friction"].asFloat();
	mapInfo->ElasticityDividedForce = mapRoot["elasticity_divided_force"].asFloat();
}
