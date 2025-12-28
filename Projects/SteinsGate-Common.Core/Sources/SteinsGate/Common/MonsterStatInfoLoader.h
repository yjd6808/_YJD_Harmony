/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/MonsterStatInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MonsterStatInfoLoader : ConfigFileLoaderAbstract
{
public:
	MonsterStatInfoLoader(DataManagerAbstract* _pManager);
	~MonsterStatInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Monster_Stat; }
	bool Load() override;

	static void ReadMobStatInfo(Json::Value& _monsterRoot, OUT MonsterStatInfo* _pMonsterStatInfo);
};
