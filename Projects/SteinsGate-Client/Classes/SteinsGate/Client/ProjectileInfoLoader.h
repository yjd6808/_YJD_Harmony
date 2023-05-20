/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:55 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ProjectileInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ProjectileInfoLoader : ConfigFileLoaderAbstract
{
public:
	ProjectileInfoLoader(DataManagerAbstract* manager);
	~ProjectileInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return  ConfigFileType::Projectile; }
	bool load() override;
private:
	static void readOverridedProjectileInfo(Json::Value& projectileRoot, JCORE_OUT ProjectileInfo* projectileInfo);
	static void readProjectileInfo(Json::Value& projectileRoot, JCORE_OUT ProjectileInfo* projectileInfo);
};
