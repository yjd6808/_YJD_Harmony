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
	ProjectileInfoLoader(DataManagerAbstract* manager, ActorType_t actorType);
	~ProjectileInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override {
		switch (m_eActorType) {
		case ActorType::Character:	return ConfigFileType::Char_Projectile;
		case ActorType::Monster:	return ConfigFileType::Monster_Projectile;
		default: DebugAssert(false);
		}
		return ConfigFileType::Max;
	}
	bool load() override;
private:
	void readOverridedProjectileInfo(Json::Value& projectileRoot, JCORE_OUT ProjectileInfo* projectileInfo);
	void readProjectileInfo(Json::Value& projectileRoot, JCORE_OUT ProjectileInfo* projectileInfo);

	ActorType_t m_eActorType;
};
