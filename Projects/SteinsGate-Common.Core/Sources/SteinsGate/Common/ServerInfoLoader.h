/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:42 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ServerProcessInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ServerInfoLoader : ConfigFileLoaderAbstract
{
public:
	ServerInfoLoader(DataManagerAbstract* manager);
	virtual ~ServerInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Server; }
	bool load() override;

	static void readCenterInfo(Json::Value& serverRoot, JCORE_OUT ServerProcessInfo* serverInfo);
	static void readAuthInfo(Json::Value& serverRoot, JCORE_OUT ServerProcessInfo* serverInfo);
	static void readLobbyInfo(Json::Value& serverRoot, JCORE_OUT ServerProcessInfo* serverInfo);
	static void readGameInfo(Json::Value& serverRoot, JCORE_OUT ServerProcessInfo* serverInfo);
};
