/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:07 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/TileInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>


struct TileInfoLoader : ConfigFileLoaderAbstract
{
public:
	TileInfoLoader(DataManagerAbstract* manager);
	~TileInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Tile; }
	static void readTileInfo(Json::Value& tileRoot, JCORE_OUT TileInfo* tileInfo);
};


