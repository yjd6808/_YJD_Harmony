/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:20 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGMapInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

#define JsonFileName "map.json"

bool SGMapInfoLoader::LoadMapInfo(SGHashMap<int, SGMapInfo>& mapInfoMap) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGString path = JCore::Path::Combine(ConfigDirectory_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}
	Json::Value mapListRoot = root["map"];

	for (int i = 0; i < mapListRoot.size(); ++i) {
		Value& mapRoot = mapListRoot[i];

		Value& areaRoot = mapRoot["area"];
		Value& npcListRoot = mapRoot["npc"];
		Value& obstacleListRoot = mapRoot["obstacle"];

		// TODO: NPC 추가시 수정 필요
		SGMapInfo info(areaRoot.size(), /* npcRoot.size() */ 1, obstacleListRoot.size());

		info.Code = mapRoot["code"].asInt();
		info.Name = SGJson::getString(mapRoot["name"]);
		info.MapWidth = mapRoot["map_width"].asInt();
		info.MapHeight = mapRoot["map_height"].asInt();

		// 보이는 그대로 블록 반영해주기위해
		for (int j = areaRoot.size() - 1; j >= 0; --j) {
			info.Area.PushBack(Move(SGJson::getString(areaRoot[j])));
		}

		for (int j = 0; j < npcListRoot.size(); ++j) {
			// TODO: NPC 추가시 구현 필요
		}

		for (int j = 0; j < obstacleListRoot.size(); ++j) {
			Value& obstacleRoot = obstacleListRoot[j];
			SGMapObjectInfo objectInfo;
			SGJson::parseIntNumber3(obstacleRoot, objectInfo.Code, objectInfo.X, objectInfo.Y);
			info.ObstacleList.PushBack(objectInfo);
		}

		info.TileWidth = mapRoot["tile_width"].asInt();
		info.TileHeight = mapRoot["tile_height"].asInt();

		Value& tileListRoot = mapRoot["tile"];

		for (int j = 0; j < info.TileHeight; ++j) {
			Value& tileRoot = tileListRoot[j];
			SGJson::parseIntNumberN(tileRoot, info.TileArray[j], info.TileWidth);
		}

		info.Gravity = mapRoot["gravity"].asFloat();
		info.Friction = mapRoot["friction"].asFloat();
		info.ElasticityDividedForce = mapRoot["elasticity_divided_force"].asFloat();

		mapInfoMap.Insert(info.Code, Move(info));
	}


	Log("SGMapInfoLoader :: 로딩완료\n");
	return true;
}