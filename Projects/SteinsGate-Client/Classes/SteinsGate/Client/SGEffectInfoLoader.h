/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 4:43:09 PM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGEffectInfo.h>

struct SGEffectInfoLoader
{
public:
	static bool LoadEffectInfo(SGHashMap<int, SGEffectInfo>& effectInfoMap);
};


