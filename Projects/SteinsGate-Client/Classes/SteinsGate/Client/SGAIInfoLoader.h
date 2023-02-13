/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:56 PM
 * =====================
 *
 */


#pragma once



#include "Tutturu.h"
#include <SteinsGate/Client/SGAIInfo.h>


struct SGAIInfoLoader
{
public:
	static bool LoadAIInfo(SGHashMap<int, SGAIInfo>& aiInfoMap);
};


