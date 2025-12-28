/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:11 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/MapInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MapInfoLoader : ConfigFileLoaderAbstract
{
public:
	MapInfoLoader(DataManagerAbstract* _pManager);
	~MapInfoLoader() override;

	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Map; }

	void OnDungeonMapRootLoaded(Json::Value& _mapRoot, const SGString& _fileName, DirectoryTreeNode* _pCurNode);
	void OnPrivateMapRootLoaded(Json::Value& _mapRoot, const SGString& _fileName, DirectoryTreeNode* _pCurNode);
	void OnTownMapRootLoaded(Json::Value& _mapRoot, const SGString& _fileName, DirectoryTreeNode* _pCurNode);
	void OnAreaRootLoaded(Json::Value& _areaRoot, const SGString& _fileName, DirectoryTreeNode* _pCurNode);
	void TestOnCommon(Json::Value& _mapRoot, const SGString& _fileName, DirectoryTreeNode* _pCurNode);

	static void ReadMapCommonInfo(Json::Value& _mapRoot, OUT MapInfo* _pMapInfo);
	static void ReadMapAreaInfo(Json::Value& _areaRoot, OUT MapAreaInfo* _pMapAreaInfo);

	MapAreaInfo* GetMapAreaInfo(int _mapCode);

private:
	MapInfo* CreateMapInfo(Json::Value& _mapRoot, MapType_t _mapType);

	SGHashMap<int, MapAreaInfo*> mapAreaInfoHash_;
};
