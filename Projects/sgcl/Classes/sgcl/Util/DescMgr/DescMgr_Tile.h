/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:07 AM
 * =====================
 *
 */

#pragma once

#include "sg/Struct/SteinsGate_Tile.h"
#include "sg/Util/DescLoaderAbstract.h"

struct TileInfoLoader : DescLoaderAbstract
{
public:
	TileInfoLoader();
	~TileInfoLoader() override = default;

	bool Load() override;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Tile;
	}

	static void ReadTileInfo(Json::Value& _tileRoot, OUT TileInfo* _pTileInfo);
};
