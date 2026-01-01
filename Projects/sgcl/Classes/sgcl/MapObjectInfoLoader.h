/*
 * 
 * 
 * =====================
 *
 */

#pragma once

#include <sgcl/MapObjectInfo.h>
#include <sg/DescLoaderAbstract.h>

struct MapObjectInfoLoader : DescLoaderAbstract
{
public:
	MapObjectInfoLoader(DescLoaderMgr* _pManager);
	~MapObjectInfoLoader() override = default;

	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::MapObject; }

	static void ReadObstacleInfo(Json::Value& _mapObjectRoot, OUT MapObjectObstacleInfo* _pInfo);
	static void ReadGateInfo(Json::Value& _mapObjectRoot, OUT MapObjectGateInfo* _pInfo);
};
