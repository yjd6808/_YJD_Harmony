/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:26 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGObstacleInfo.h>


struct SGObstacleInfoLoader
{
public:
	static bool LoadObstacleInfo(SGHashMap<int, SGObstacleInfo>& obstacleInfoMap);
};


