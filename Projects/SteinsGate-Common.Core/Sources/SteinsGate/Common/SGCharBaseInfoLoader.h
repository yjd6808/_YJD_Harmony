/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGCharBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGCharBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	SGCharBaseInfoLoader(DataManagerAbstract* manager);

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Char_Base; }
	
	bool load() override;

	static void readCharBaseInfo(Json::Value& charBaseRoot, SGCharBaseInfo* baseInfo);
	static void readGunnerInfo(Json::Value& gunnerBaseRoot, SGGunnerInfo* baseInfo);
};