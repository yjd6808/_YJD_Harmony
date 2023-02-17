/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:55 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGProjectileInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGProjectileInfoLoader : ConfigFileLoaderAbstract
{
public:
	~SGProjectileInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return  ConfigFileType::Projectile; }
	bool load() override;
private:
	static void readOverridedProjectileInfo(Json::Value& projectileRoot, Out_ SGProjectileInfo* projectileInfo);
	static void readProjectileInfo(Json::Value& projectileRoot, Out_ SGProjectileInfo* projectileInfo);
};
