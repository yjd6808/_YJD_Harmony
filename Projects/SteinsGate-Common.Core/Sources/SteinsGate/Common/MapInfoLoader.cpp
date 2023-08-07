/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:20 AM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "MapInfoLoader.h"

#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

MapInfoLoader::MapInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

MapInfoLoader::~MapInfoLoader() {
	m_hMapAreaInfo.ForEachValue([](MapAreaInfo* area) {
		delete area;
	});
}

bool MapInfoLoader::load() {
	DirectoryTree tree;

	if (!loadDirectory(tree)) {
		return false;
	}

	tree.setCallback("map/map_of_dungeon",		JCORE_CALLBACK_3(MapInfoLoader::onDungeonMapRootLoaded, this));
	tree.setCallback("map/map_of_dungeon_area", JCORE_CALLBACK_3(MapInfoLoader::onAreaRootLoaded, this));
	tree.setCallback("map/map_of_private",		JCORE_CALLBACK_3(MapInfoLoader::onPrivateMapRootLoaded, this));
	tree.setCallback("map/map_of_private_area", JCORE_CALLBACK_3(MapInfoLoader::onAreaRootLoaded, this));
	tree.setCallback("map/map_of_town",			JCORE_CALLBACK_3(MapInfoLoader::onTownMapRootLoaded, this));
	tree.setCallback("map/map_of_town_area",	JCORE_CALLBACK_3(MapInfoLoader::onAreaRootLoaded, this));

	tree.setCallbackCommon(JCORE_CALLBACK_3(MapInfoLoader::test_onCommon, this));
	tree.load();

	return true;
}

MapInfo* MapInfoLoader::createMapInfo(Value& mapRoot, MapType_t type) {
	Value& npcListRoot = mapRoot["npc"];
	Value& mapObjectListRoot = mapRoot["map_object"];



	// TODO: NPC 추가시 수정 필요
	return dbg_new MapInfo(/* (int)npcRoot.size() */ 1, (int)mapObjectListRoot.size());
}

void MapInfoLoader::onDungeonMapRootLoaded(Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode) {
	MapInfo* pInfo = createMapInfo(mapRoot, MapType::Dungeon);
	readMapCommonInfo(mapRoot, pInfo);
	addData(pInfo);
}

void MapInfoLoader::onPrivateMapRootLoaded(Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode) {
	MapInfo* pInfo = createMapInfo(mapRoot, MapType::Private);
	readMapCommonInfo(mapRoot, pInfo);
	addData(pInfo);
}

void MapInfoLoader::onTownMapRootLoaded(Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode) {
	MapInfo* pInfo = createMapInfo(mapRoot, MapType::Town);
	readMapCommonInfo(mapRoot, pInfo);
	addData(pInfo);
}

void MapInfoLoader::onAreaRootLoaded(Value& areaRoot, const SGString& fileName, DirectoryTreeNode* curNode) {
	int iMapCode = areaRoot["code"].asInt();
	int iHeight = areaRoot["area"].size();

	if (iMapCode < 1) {
		_LogWarn_("%s 올바르지 않은 area 데이터", fileName.Source());
		return;
	}

	if (iHeight == 0) {
		_LogWarn_("%s area 데이터가 없음", fileName.Source());
		return;
	}

	MapAreaInfo* pInfo = dbg_new MapAreaInfo(iHeight);
	readMapAreaInfo(areaRoot, pInfo);
	const bool bAdded = m_hMapAreaInfo.Insert(iMapCode, pInfo);
	_LogWarnIf_(!bAdded, "이미 %d 맵의 area 데이터가 존재함", iMapCode);
}

void MapInfoLoader::test_onCommon(Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode) {
	JCORE_PASS;
}

void MapInfoLoader::readMapCommonInfo(Value& mapRoot, JCORE_OUT MapInfo* mapInfo) {

	Value& areaRoot = mapRoot["area"];
	Value& npcListRoot = mapRoot["npc"];
	Value& mapObjectListRoot = mapRoot["map_object"];

	mapInfo->Code = mapRoot["code"].asInt();
	mapInfo->Name = JsonUtil::getString(mapRoot["name"]);
	mapInfo->PhysicsCode = mapRoot["physics"].asInt();

	for (int j = 0; j < npcListRoot.size(); ++j) {
		// TODO: NPC 추가시 구현 필요
	}

	for (int j = 0; j < mapObjectListRoot.size(); ++j) {
		Value& mapObjectRoot = mapObjectListRoot[j];
		MapObjectPositionInfo objectInfo;
		JsonUtil::parseIntNumber3(mapObjectRoot, objectInfo.Code, objectInfo.X, objectInfo.Y);
		mapInfo->MapObjectList.PushBack(objectInfo);
	}

	mapInfo->TileWidth = mapRoot["tile_width"].asInt();
	mapInfo->TileHeight = mapRoot["tile_height"].asInt();

	Value& tileListRoot = mapRoot["tile"];

	for (int j = 0; j < mapInfo->TileHeight; ++j) {
		Value& tileRoot = tileListRoot[j];
		JsonUtil::parseIntNumberN(tileRoot, mapInfo->TileArray[j], mapInfo->TileWidth);
	}

}

void MapInfoLoader::readMapAreaInfo(Value& areaRoot, JCORE_OUT MapAreaInfo* mapAreaInfo) {
	Value& areaDataRoot = areaRoot["area"];
	for (int j = areaDataRoot.size() - 1; j >= 0; --j) {
		mapAreaInfo->Area.PushBack(JsonUtil::getString(areaDataRoot[j]));
	}
}

MapAreaInfo* MapInfoLoader::getMapAreaInfo(int mapCode) {
	MapAreaInfo** ppFind = m_hMapAreaInfo.Find(mapCode);

	if (ppFind == nullptr) {
		return nullptr;
	}

	return *ppFind;
}
