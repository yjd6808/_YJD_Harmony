/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:55 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGProjectileInfo.h>
#include <JCore/Container/HashMap.h>

namespace Json { class Value; }

struct SGProjectileInfoLoader
{
public:
	static bool LoadProjectileInfo(SGHashMap<int, SGProjectileInfo>& projectileInfoMap);
private:
	static void WriteOverridedProjectileInfo(Json::Value& projectile, SGProjectileInfo& info);
	static void WriteProjectileInfo(Json::Value& projectile, SGProjectileInfo& info);
};
