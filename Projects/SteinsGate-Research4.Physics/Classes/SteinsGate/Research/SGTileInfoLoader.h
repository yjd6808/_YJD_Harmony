/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:07 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGTileInfo.h>


struct SGTileInfoLoader
{
public:
	static bool LoadTileInfo(SGHashMap<int, SGTileInfo>& tileInfoMap);
};

