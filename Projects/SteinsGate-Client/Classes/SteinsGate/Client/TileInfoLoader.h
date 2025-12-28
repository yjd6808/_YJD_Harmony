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
	TileInfoLoader(DataManagerAbstract* _pManager);
	~TileInfoLoader() override = default;

	bool Load() override;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Tile;
	}

	static void ReadTileInfo(Json::Value& _tileRoot, OUT TileInfo* _pTileInfo);
};
