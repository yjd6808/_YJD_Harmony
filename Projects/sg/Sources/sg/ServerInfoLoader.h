/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:42 AM
 * =====================
 *
 */


#pragma once

#include <sg/ServerProcessInfo.h>
#include <sg/DescLoaderAbstract.h>

struct ServerInfoLoader : DescLoaderAbstract
{
public:
	ServerInfoLoader(DescLoaderMgr* _pManager);
	~ServerInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Server; }
	bool Load() override;

	static void ReadCenterInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadAuthInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadLobbyInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo);
	static void ReadGameInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo);
};
