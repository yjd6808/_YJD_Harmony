/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:42 AM
 * =====================
 *
 */


#pragma once

#include "sg/Struct/SteinsGate_Server.h"
#include "sg/Util/DescLoaderAbstract.h"

struct ServerInfoLoader : DescLoaderAbstract
{
public:
	ServerInfoLoader();
	~ServerInfoLoader() override = default;

	virtual ConfigFileType_t GetConfigFileType() override { return ConfigFileType::ServerInfo; }
	virtual bool Load() override;
	virtual bool UseSrcDataPath() const override { return false; }

	static void ReadCommonInfo(Json::Value& _serverRoot, OUT ServerProcessInfo* _pServerInfo);
	static void ReadInterServInfo(Json::Value& _serverRoot, OUT NetInterServerInfo& _interServerInfo);
	static void ReadDbInfo(Json::Value& _databaseRoot, OUT NetDBInfo_MySQL& _dbInfo);
	static void ReadDbInfo(Json::Value& _databaseRoot, OUT NetDBInfo_SQLServer& _dbInfo);
};
