/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:45:23 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SGMobBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGMobBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	virtual ~SGMobBaseInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Monster; }
	bool load() override;

	static void readMobBaseInfo(Json::Value& monsterRoot, Out_ SGMobBaseInfo* mobInfo);
};
