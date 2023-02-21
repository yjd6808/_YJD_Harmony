/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:50 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/SGJson.h>

USING_NS_JS;
USING_NS_JC;

bool ServerInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value serverInfoRootList = root["server"];

		for (int i = 0; i < serverInfoRootList.size(); ++i) {
			Value& serverInfoRoot = serverInfoRootList[i];
			Value& gameServerInfoListRoot = serverInfoRoot["game"];
			int gameServerCount = (int)gameServerInfoListRoot.size();

			DebugAssertMsg(gameServerCount > 0, "게임 서버 카운트가 0입니다.");
			ServerInfo* pServerInfo = dbg_new ServerInfo(gameServerCount);
			pServerInfo->Code = serverInfoRoot["code"].asInt();
			pServerInfo->Name = SGJson::getString(serverInfoRoot["name"]);
			readCenterInfo(serverInfoRoot, pServerInfo);
			readLobbyInfo(serverInfoRoot, pServerInfo);
			readAuthInfo(serverInfoRoot, pServerInfo);
			readGameInfo(gameServerInfoListRoot, pServerInfo);
			addData(pServerInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void ServerInfoLoader::readCenterInfo(Json::Value& serverRoot, ServerInfo* serverInfo) {
	SGString szBindInterServerUdp = SGJson::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindCenterTcp = SGJson::getString(serverRoot["bind_center_tcp"]);

	serverInfo->Center.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Center.BindCenterTcp = szBindCenterTcp;
}

void ServerInfoLoader::readAuthInfo(Json::Value& serverRoot, ServerInfo* serverInfo) {
	SGString szBindAuthTcp = SGJson::getString(serverRoot["bind_auth_tcp"]);
	SGString szBindInterServerUdp = SGJson::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindCenterTcp = SGJson::getString(serverRoot["bind_center_tcp"]);
	SGString szConnectCenterTcp = SGJson::getString(serverRoot["connect_center_tcp"]);

	serverInfo->Auth.BindAuthTcp = szBindAuthTcp;
	serverInfo->Auth.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Auth.BindCenterTcp = szBindCenterTcp;
	serverInfo->Auth.ConnectCenterTcp = szConnectCenterTcp;
}

void ServerInfoLoader::readLobbyInfo(Json::Value& serverRoot, ServerInfo* serverInfo) {
	SGString szBindLobbyTcp = SGJson::getString(serverRoot["bind_lobby_tcp"]);
	SGString szBindInterServerUdp = SGJson::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindCenterTcp = SGJson::getString(serverRoot["bind_center_tcp"]);
	SGString szConnectCenterTcp = SGJson::getString(serverRoot["connect_center_tcp"]);

	serverInfo->Lobby.BindLobbyTcp = szBindLobbyTcp;
	serverInfo->Lobby.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Lobby.BindCenterTcp = szBindCenterTcp;
	serverInfo->Lobby.ConnectCenterTcp = szConnectCenterTcp;
}

void ServerInfoLoader::readGameInfo(Json::Value& gameServerLostRoot, ServerInfo* serverInfo) {

	int iGameServerCount = (int)gameServerLostRoot.size();

	for (int i = 0; i < iGameServerCount; ++i) {
		Value& gameServerRoot = gameServerLostRoot[i];

		GameServerInfo info;

		info.Code = (ServerType_t)gameServerRoot["code"].asInt();
		info.Name = SGJson::getString(gameServerRoot["name"]);
		info.Active = gameServerRoot["active"].asBool();

		if (info.Active == false)
			continue;

		SGString szBindGameTcp = SGJson::getStringOrNull(gameServerRoot["bind_game_tcp"]);
		SGString szBindGameUdp = SGJson::getStringOrNull(gameServerRoot["bind_game_udp"]);
		SGString szBindChatTcp = SGJson::getStringOrNull(gameServerRoot["bind_town_tcp"]);
		SGString szBindTownTcp = SGJson::getStringOrNull(gameServerRoot["bind_chat_tcp"]);
		SGString szBindInterServerUdp = SGJson::getStringOrNull(gameServerRoot["bind_interserver_udp"]);
		SGString szBindCenterTcp = SGJson::getStringOrNull(gameServerRoot["bind_center_tcp"]);
		SGString szConnectCenterTcp = SGJson::getStringOrNull(gameServerRoot["connect_center_tcp"]);

		DebugAssertMsg(
			!szBindGameTcp.IsNull() &&
			!szBindGameUdp.IsNull() &&
			!szBindChatTcp.IsNull() &&
			!szBindTownTcp.IsNull() &&
			!szBindInterServerUdp.IsNull() &&
			!szBindCenterTcp.IsNull() &&
			!szConnectCenterTcp.IsNull()
			,
			"액티브 게임서버 정보인데 비어있는 설정이 있습니다."
		);

		
		info.BindGameTcp = szBindGameTcp;
		info.BindGameUdp = szBindGameUdp;
		info.BindChatTcp = szBindChatTcp;
		info.BindTownTcp = szBindTownTcp;
		info.BindInterServerUdp = szBindInterServerUdp;
		info.BindCenterTcp = szBindCenterTcp;
		info.ConnectCenterTcp = szConnectCenterTcp;

		serverInfo->GameList.PushBack(Move(info));
	}
}

