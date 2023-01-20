/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:55 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGProjectileInfo.h>
#include <JCore/Container/HashMap.h>


struct SGProjectileInfoLoader
{
public:
	static void LoadProjectileInfo(SGHashMap<int, SGProjectileInfo>& projectileInfoMap);
};
