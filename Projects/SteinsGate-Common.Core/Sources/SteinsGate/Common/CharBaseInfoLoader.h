/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CharBaseInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct CharBaseInfoLoader : ConfigFileLoaderAbstract
{
public:
	CharBaseInfoLoader(DataManagerAbstract* manager);

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Char_Base; }
	
	bool load() override;

	static void readCharBaseInfo(Json::Value& charBaseRoot, CharBaseInfo* baseInfo);
	static void readGunnerInfo(Json::Value& gunnerBaseRoot, GunnerInfo* baseInfo);
};