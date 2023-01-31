/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/SGAttackData.h>


struct SGAttackDataLoader
{
public:
	static bool LoadAttackData(SGHashMap<int, SGAttackData>& attackDataInfoMap);
};


