/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:26 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/MapObjectInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>


struct MapObjectInfoLoader : ConfigFileLoaderAbstract
{
public:
	MapObjectInfoLoader(DataManagerAbstract* manager);
	~MapObjectInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::MapObject; }

	static void readObstacleInfo(Json::Value& mapObjectRoot, JCORE_OUT MapObjectObstacleInfo* info);
	static void readGateInfo(Json::Value& mapObjectRoot, JCORE_OUT MapObjectGateInfo* info);
};


