/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:11 AM
 * =====================
 *
 */


#pragma once



#include "Tutturu.h"
#include <SteinsGate/Research/SGMapInfo.h>


struct SGMapInfoLoader
{
public:
	static bool LoadMapInfo(SGHashMap<int, SGMapInfo>& mapInfoMap);
};


