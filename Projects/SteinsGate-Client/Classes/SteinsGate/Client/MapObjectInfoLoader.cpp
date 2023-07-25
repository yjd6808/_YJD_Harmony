/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:31 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "MapObjectInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JC;
USING_NS_JS;

MapObjectInfoLoader::MapObjectInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool MapObjectInfoLoader::load() {

	Json::Value root;

	if (!loadJson(root))
		return false;
	try {

		Json::Value mapObjectListRoot = root["map_object"];
		for (int i = 0; i < mapObjectListRoot.size(); ++i) {
			Value& mapObjectRoot = mapObjectListRoot[i];

			const int eType = mapObjectRoot["type"].asInt();
			MapObjectInfo* pInfo;

			switch (eType) {
				case MapObjectType::Obstacle: {
					MapObjectObstacleInfo* pObstacleInfo = dbg_new MapObjectObstacleInfo();
					readObstacleInfo(mapObjectRoot, pObstacleInfo);
					pInfo = pObstacleInfo;
					break;
				}
				case MapObjectType::Gate: {
					MapObjectGateInfo* pGateInfo = dbg_new MapObjectGateInfo();
					readGateInfo(mapObjectRoot, pGateInfo);
					pInfo = pGateInfo;
					break;
				}
				default: {
					DebugAssertMsg(false, "올바른 맵 오브젝트 타입이 아닙니다.");
					continue;
				}
			}

			pInfo->Code = mapObjectRoot["code"].asInt();
			pInfo->Name = JsonUtilEx::getString(mapObjectRoot["name"]);
			pInfo->Hitable = JsonUtilEx::getBooleanDefault(mapObjectRoot["hitable"], false);
			pInfo->Colliadalble = JsonUtilEx::getBooleanDefault(mapObjectRoot["collidable"], false);
			pInfo->ZOrederable = JsonUtilEx::getBooleanDefault(mapObjectRoot["z_orderable"], false);
			JsonUtilEx::parseThicknessInfo(mapObjectRoot["thickness_box"], pInfo->ThicknessBox);
			JsonUtilEx::parseActorSpriteData(mapObjectRoot["actor_sprite_data"], &pInfo->SpriteData);


			addData(pInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void MapObjectInfoLoader::readObstacleInfo(Json::Value& mapObjectRoot, MapObjectObstacleInfo* info) {
	
}

void MapObjectInfoLoader::readGateInfo(Json::Value& mapObjectRoot, MapObjectGateInfo* info) {
}