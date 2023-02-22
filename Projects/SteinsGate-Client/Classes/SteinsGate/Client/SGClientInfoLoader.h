/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGClientInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGClientInfoLoader : ConfigFileLoaderAbstract
{
public:
	SGClientInfoLoader(DataManagerAbstract* manager);
	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Client; }

	static void readClientInfo(Json::Value& clientRoot, Out_ SGClientInfo* clientInfo);
};


