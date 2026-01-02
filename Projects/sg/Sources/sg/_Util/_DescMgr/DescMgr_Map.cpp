/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:20 AM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_Map.h"

#include <sg/_Util/JsonUtil.h>

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
MapInfoLoader::MapInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfoLoader::~MapInfoLoader()
{
	mapAreaInfoHash_.ForEachValue([](MapAreaInfo* pMapArea)
	{
		delete pMapArea;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapInfoLoader::Load()
{
	DirectoryTree directoryTree;

	if (!LoadDirectory(directoryTree))
	{
		return false;
	}

	directoryTree.SetCallback("map/map_of_dungeon", JC_CALLBACK_3(MapInfoLoader::OnDungeonMapRootLoaded, this));
	directoryTree.SetCallback("map/map_of_dungeon_area", JC_CALLBACK_3(MapInfoLoader::OnAreaRootLoaded, this));
	directoryTree.SetCallback("map/map_of_private", JC_CALLBACK_3(MapInfoLoader::OnPrivateMapRootLoaded, this));
	directoryTree.SetCallback("map/map_of_private_area", JC_CALLBACK_3(MapInfoLoader::OnAreaRootLoaded, this));
	directoryTree.SetCallback("map/map_of_town", JC_CALLBACK_3(MapInfoLoader::OnTownMapRootLoaded, this));
	directoryTree.SetCallback("map/map_of_town_area", JC_CALLBACK_3(MapInfoLoader::OnAreaRootLoaded, this));

	directoryTree.SetCallbackCommon(JC_CALLBACK_3(MapInfoLoader::TestOnCommon, this));
	directoryTree.Load();

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* MapInfoLoader::CreateMapInfo(Value& _mapRoot, MapType_t _mapType)
{
	Value& npcListRoot = _mapRoot["npc"];
	Value& mapObjectListRoot = _mapRoot["map_object"];

	// TODO: NPC 추가시 수정 필요
	return dbg_new MapInfo(/* (int)npcRoot.size() */ 1, static_cast<int>(mapObjectListRoot.size()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::OnDungeonMapRootLoaded(Value& _mapRoot, const jc::String& _fileName, DirectoryTreeNode* _pCurNode)
{
	MapInfo* pMapInfo = CreateMapInfo(_mapRoot, MapType::Dungeon);
	ReadMapCommonInfo(_mapRoot, pMapInfo);
	AddData(pMapInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::OnPrivateMapRootLoaded(Value& _mapRoot, const jc::String& _fileName, DirectoryTreeNode* _pCurNode)
{
	MapInfo* pMapInfo = CreateMapInfo(_mapRoot, MapType::Private);
	ReadMapCommonInfo(_mapRoot, pMapInfo);
	AddData(pMapInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::OnTownMapRootLoaded(Value& _mapRoot, const jc::String& _fileName, DirectoryTreeNode* _pCurNode)
{
	MapInfo* pMapInfo = CreateMapInfo(_mapRoot, MapType::Town);
	ReadMapCommonInfo(_mapRoot, pMapInfo);
	AddData(pMapInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::OnAreaRootLoaded(Value& _areaRoot, const jc::String& _fileName, DirectoryTreeNode* _pCurNode)
{
	int mapCode = _areaRoot["code"].asInt();
	int height = _areaRoot["area"].size();

	if (mapCode < 1)
	{
		_LogWarn_("%s 올바르지 않은 area 데이터", _fileName.Source());
		return;
	}

	if (height == 0)
	{
		_LogWarn_("%s area 데이터가 없음", _fileName.Source());
		return;
	}

	MapAreaInfo* pMapAreaInfo = dbg_new MapAreaInfo(height);
	ReadMapAreaInfo(_areaRoot, pMapAreaInfo);
	const bool added = mapAreaInfoHash_.Insert(mapCode, pMapAreaInfo);
	_LogWarnIf_(!added, "이미 %d 맵의 area 데이터가 존재함", mapCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::TestOnCommon(Value& _mapRoot, const jc::String& _fileName, DirectoryTreeNode* _pCurNode)
{
	JC_PASS;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::ReadMapCommonInfo(Value& _mapRoot, OUT MapInfo* _pMapInfo)
{
	Value& areaRoot = _mapRoot["area"];
	Value& npcListRoot = _mapRoot["npc"];
	Value& mapObjectListRoot = _mapRoot["map_object"];

	_pMapInfo->code_ = _mapRoot["code"].asInt();
	_pMapInfo->name_ = JsonUtil::GetString(_mapRoot["name"]);
	_pMapInfo->physicsCode_ = _mapRoot["physics"].asInt();

	for (int j = 0; j < npcListRoot.size(); ++j)
	{
		// TODO: NPC 추가시 구현 필요
	}

	for (int j = 0; j < mapObjectListRoot.size(); ++j)
	{
		Value& mapObjectRoot = mapObjectListRoot[j];
		MapObjectPositionInfo mapObjectInfo;
		JsonUtil::ParseIntNumber3(mapObjectRoot, mapObjectInfo.code_, mapObjectInfo.x_, mapObjectInfo.y_);
		_pMapInfo->mapObjectList_.PushBack(mapObjectInfo);
	}

	_pMapInfo->tileWidth_ = _mapRoot["tile_width"].asInt();
	_pMapInfo->tileHeight_ = _mapRoot["tile_height"].asInt();

	Value& tileListRoot = _mapRoot["tile"];

	for (int j = 0; j < _pMapInfo->tileHeight_; ++j)
	{
		Value& tileRoot = tileListRoot[j];
		JsonUtil::ParseIntNumberN(tileRoot, _pMapInfo->tileArray_[j], _pMapInfo->tileWidth_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapInfoLoader::ReadMapAreaInfo(Value& _areaRoot, OUT MapAreaInfo* _pMapAreaInfo)
{
	Value& areaDataRoot = _areaRoot["area"];

	for (int j = areaDataRoot.size() - 1; j >= 0; --j)
	{
		_pMapAreaInfo->area_.PushBack(JsonUtil::GetString(areaDataRoot[j]));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* MapInfoLoader::GetMapAreaInfo(int _mapCode)
{
	MapAreaInfo** ppMapAreaInfo = mapAreaInfoHash_.Find(_mapCode);

	if (ppMapAreaInfo == nullptr)
	{
		return nullptr;
	}

	return *ppMapAreaInfo;
}
