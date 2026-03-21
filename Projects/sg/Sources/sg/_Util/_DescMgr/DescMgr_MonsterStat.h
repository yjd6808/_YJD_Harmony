/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include "sg/_Struct/SteinsGate_MonsterStat.h"
#include "sg/_Util/DescLoaderAbstract.h"

struct MonsterStatInfoLoader : DescLoaderAbstract
{
public:
	MonsterStatInfoLoader();
	~MonsterStatInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Monster_Stat; }
	bool Load() override;

	static void ReadMobStatInfo(Json::Value& _monsterRoot, OUT MonsterStatInfo* _pMonsterStatInfo);
};
