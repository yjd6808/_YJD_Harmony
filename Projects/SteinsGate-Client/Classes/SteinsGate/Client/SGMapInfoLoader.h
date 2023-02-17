/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:33:11 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGMapInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>


struct SGMapInfoLoader : ConfigFileLoaderAbstract
{
public:
	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Map; }

	static void readMapInfo(Json::Value& mapRoot, Out_ SGMapInfo* mapInfo);
};


