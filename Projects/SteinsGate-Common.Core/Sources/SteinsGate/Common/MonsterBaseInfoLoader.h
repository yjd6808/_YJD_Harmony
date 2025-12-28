/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/MonsterBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MonsterBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	MonsterBaseInfoLoader(DataManagerAbstract* _pManager);
	~MonsterBaseInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override
	{
		return ConfigFileType::Monster;
	}

	bool Load() override;

	static void ReadMonsterBaseInfo(Json::Value& _monsterRoot, OUT MonsterBaseInfo* _pMobInfo);
};
