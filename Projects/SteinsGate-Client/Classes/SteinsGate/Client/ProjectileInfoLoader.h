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
	ProjectileInfoLoader(DataManagerAbstract* _pManager, ActorType_t _actorType);
	~ProjectileInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		switch (actorType_)
		{
		case ActorType::Character: return ConfigFileType::Char_Projectile;
		case ActorType::Monster: return ConfigFileType::Monster_Projectile;
		default: DebugAssert(false);
		}

		return ConfigFileType::Max;
	}

	bool Load() override;

private:
	void ReadOverridedProjectileInfo(Json::Value& _projectileRoot, JCORE_OUT ProjectileInfo* _pProjectileInfo);
	void ReadProjectileInfo(Json::Value& _projectileRoot, JCORE_OUT ProjectileInfo* _pProjectileInfo);

	ActorType_t actorType_;
};
