/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>
#include <SteinsGate/Client/ClientInfo.h>

struct ClientInfoLoader : ConfigFileLoaderAbstract
{
public:
	ClientInfoLoader(DataManagerAbstract* _pManager);
	~ClientInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override;
	bool Load() override;

	static void ReadClientInfo(Json::Value& _clientRoot, JCORE_OUT ClientInfo* _pClientInfo);
};
