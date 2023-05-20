/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/ClientInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ClientInfoLoader : ConfigFileLoaderAbstract
{
public:
	ClientInfoLoader(DataManagerAbstract* manager);
	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Client; }

	static void readClientInfo(Json::Value& clientRoot, JCORE_OUT ClientInfo* clientInfo);
};


