/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/MobBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct MobBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	virtual ~MobBaseInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Monster; }
	bool load() override;

	static void readMobBaseInfo(Json::Value& monsterRoot, Out_ MobBaseInfo* mobInfo);
};
