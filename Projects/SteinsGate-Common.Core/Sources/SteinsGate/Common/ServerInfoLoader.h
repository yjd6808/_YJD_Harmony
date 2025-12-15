/*
 * 작성자: 윤정도
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
	ServerInfoLoader(DataManagerAbstract* _pManager);
	~ServerInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Server; }
	bool Load() override;

	static void ReadCenterInfo(Json::Value& _serverRoot, JCORE_OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadAuthInfo(Json::Value& _serverRoot, JCORE_OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadLobbyInfo(Json::Value& _serverRoot, JCORE_OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadGameInfo(Json::Value& _serverRoot, JCORE_OUT ServerProcessInfoPackage* _pServerInfo);
};
