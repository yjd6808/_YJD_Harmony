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
	MapInfoLoader(DataManagerAbstract* manager);
	~MapInfoLoader() override;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Map; }

	void onDungeonMapRootLoaded(Json::Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode);
	void onPrivateMapRootLoaded(Json::Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode);
	void onTownMapRootLoaded(Json::Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode);
	void onAreaRootLoaded(Json::Value& areaRoot, const SGString& fileName, DirectoryTreeNode* curNode);
	void test_onCommon(Json::Value& mapRoot, const SGString& fileName, DirectoryTreeNode* curNode);

	static void readMapCommonInfo(Json::Value& mapRoot, JCORE_OUT MapInfo* mapInfo);
	static void readMapAreaInfo(Json::Value& areaRoot, JCORE_OUT MapAreaInfo* mapAreaInfo);

	MapAreaInfo* getMapAreaInfo(int mapCode);
private:
	MapInfo* createMapInfo(Json::Value& mapRoot, MapType_t type);

	SGHashMap<int, MapAreaInfo*> m_hMapAreaInfo;
};




