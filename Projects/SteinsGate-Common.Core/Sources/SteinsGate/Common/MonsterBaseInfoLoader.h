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
	MonsterBaseInfoLoader(DataManagerAbstract* manager);
	virtual ~MonsterBaseInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Monster; }
	bool load() override;

	static void readMonsterBaseInfo(Json::Value& monsterRoot, JCORE_OUT MonsterBaseInfo* mobInfo);
};
