/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include "sg/_Struct/SteinsGate_MonsterBase.h"
#include "sg/_Util/DescLoaderAbstract.h"

struct MonsterBaseInfoLoader : DescLoaderAbstract
{
public:
	MonsterBaseInfoLoader();
	~MonsterBaseInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Monster;
	}

	bool Load() override;

	static void ReadMonsterBaseInfo(Json::Value& _monsterRoot, OUT MonsterBaseInfo* _pMobInfo);
};
