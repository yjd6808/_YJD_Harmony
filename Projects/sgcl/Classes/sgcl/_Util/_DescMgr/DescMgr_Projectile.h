/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:55 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Struct/SteinsGate_Projectile.h>
#include <sg/_Util/DescLoaderAbstract.h>

struct ProjectileInfoLoader : DescLoaderAbstract
{
public:
	ProjectileInfoLoader(ActorType_t _actorType);
	~ProjectileInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		switch (actorType_)
		{
		case ActorType::Character: return ConfigFileType::Char_Projectile;
		case ActorType::Monster: return ConfigFileType::Monster_Projectile;
		default: jc_assert(false);
		}

		return ConfigFileType::Max;
	}

	bool Load() override;

private:
	void ReadOverridedProjectileInfo(Json::Value& _projectileRoot, OUT ProjectileInfo* _pProjectileInfo);
	void ReadProjectileInfo(Json::Value& _projectileRoot, OUT ProjectileInfo* _pProjectileInfo);

	ActorType_t actorType_;
};
