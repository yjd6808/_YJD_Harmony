/*
 * 
 * 
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/MapObjectInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MapObjectInfoLoader : ConfigFileLoaderAbstract
{
public:
	MapObjectInfoLoader(DataManagerAbstract* _pManager);
	~MapObjectInfoLoader() override = default;

	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::MapObject; }

	static void ReadObstacleInfo(Json::Value& _mapObjectRoot, JCORE_OUT MapObjectObstacleInfo* _pInfo);
	static void ReadGateInfo(Json::Value& _mapObjectRoot, JCORE_OUT MapObjectGateInfo* _pInfo);
};
