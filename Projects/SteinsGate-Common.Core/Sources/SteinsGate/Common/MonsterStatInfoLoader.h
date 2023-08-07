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
	MonsterStatInfoLoader(DataManagerAbstract* manager);
	virtual ~MonsterStatInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Monster_Stat; }
	bool load() override;

	static void readMobStatInfo(Json::Value& monsterRoot, JCORE_OUT MonsterStatInfo* mobStatInfo);
};
