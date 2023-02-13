/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:08:07 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGTileInfo.h>


struct SGTileInfoLoader
{
public:
	static bool LoadTileInfo(SGHashMap<int, SGTileInfo>& tileInfoMap);
};


