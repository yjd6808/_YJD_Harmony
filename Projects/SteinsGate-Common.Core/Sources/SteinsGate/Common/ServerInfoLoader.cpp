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

ServerInfoLoader::ServerInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ServerInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& serverInfoRootList = root["server"];

		for (int i = 0; i < serverInfoRootList.size(); ++i) {
			Value& serverInfoRoot = serverInfoRootList[i];
			Value& gameServerInfoListRoot = serverInfoRoot["game"];
			int gameServerCount = (int)gameServerInfoListRoot.size();

			DebugAssertMsg(gameServerCount > 0, "게임 서버 카운트가 0입니다.");
			ServerInfo* pServerInfo = dbg_new ServerInfo(gameServerCount);
			pServerInfo->Code = serverInfoRoot["code"].asInt();
			pServerInfo->Name = SGJson::getString(serverInfoRoot["name"]);
			readCenterInfo(serverInfoRoot["center"], pServerInfo);
			readAuthInfo(serverInfoRoot["auth"], pServerInfo);
			readLobbyInfo(serverInfoRoot["lobby"], pServerInfo);
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
	SGString szRemoteCenter = SGJson::getString(serverRoot["remote_center"]);

	serverInfo->Center.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Center.BindCenterTcp = szBindCenterTcp;
	serverInfo->Center.RemoteCenter = szRemoteCenter;
	serverInfo->Center.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(serverInfo->Center.MaxSessionCount > 0, "센터 맥스 세션 수는 0보다 커야합니다.");
}

void ServerInfoLoader::readAuthInfo(Json::Value& serverRoot, ServerInfo* serverInfo) {
	SGString szBindAuthTcp = SGJson::getString(serverRoot["bind_auth_tcp"]);
	SGString szRemoteAuth = SGJson::getString(serverRoot["remote_auth"]);
	SGString szBindInterServerUdp = SGJson::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindCenterTcp = SGJson::getString(serverRoot["bind_center_tcp"]);

	serverInfo->Auth.BindAuthTcp = szBindAuthTcp;
	serverInfo->Auth.RemoteAuth = szRemoteAuth;
	serverInfo->Auth.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Auth.BindCenterTcp = szBindCenterTcp;
	serverInfo->Auth.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(serverInfo->Auth.MaxSessionCount > 0, "오쓰 맥스 세션 수는 0보다 커야합니다.");
}

void ServerInfoLoader::readLobbyInfo(Json::Value& serverRoot, ServerInfo* serverInfo) {
	SGString szBindLobbyTcp = SGJson::getString(serverRoot["bind_lobby_tcp"]);
	SGString szRemoteLobby = SGJson::getString(serverRoot["remote_lobby"]);
	SGString szBindInterServerUdp = SGJson::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindCenterTcp = SGJson::getString(serverRoot["bind_center_tcp"]);

	serverInfo->Lobby.BindLobbyTcp = szBindLobbyTcp;
	serverInfo->Lobby.RemoteLobby = szRemoteLobby;
	serverInfo->Lobby.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Lobby.BindCenterTcp = szBindCenterTcp;
	serverInfo->Lobby.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(serverInfo->Lobby.MaxSessionCount > 0, "로비 맥스 세션 수는 0보다 커야합니다.");
}


void ServerInfoLoader::readGameInfo(Json::Value& gameServerLostRoot, ServerInfo* serverInfo) {

	int iGameServerCount = (int)gameServerLostRoot.size();

	for (int i = 0; i < iGameServerCount; ++i) {
		Value& gameServerRoot = gameServerLostRoot[i];

		bool bActive = gameServerRoot["active"].asBool();

		if (!bActive)
			continue;

		Value& gameChannelListRoot = gameServerRoot["active_channel"];
		int iGameChannelCount = int(gameChannelListRoot.size());
		DebugAssertMsg(iGameChannelCount > 0, "액티브 게임서버인데 활성화된 채널 정보가 아무것도 없습니다.");
		GameServerInfo info(iGameChannelCount);

		info.Code = (ServerType_t)gameServerRoot["code"].asInt();
		info.Name = SGJson::getString(gameServerRoot["name"]);
		info.Active = true;

		SGString szBindGameTcp = SGJson::getStringOrNull(gameServerRoot["bind_game_tcp"]);
		SGString szBindGameUdp = SGJson::getStringOrNull(gameServerRoot["bind_game_udp"]);
		SGString szRemoteGame = SGJson::getStringOrNull(gameServerRoot["remote_game"]);

		SGString szBindTownTcp = SGJson::getStringOrNull(gameServerRoot["bind_town_tcp"]);
		SGString szRemoteTown = SGJson::getStringOrNull(gameServerRoot["remote_town"]);

		SGString szBindChatTcp = SGJson::getStringOrNull(gameServerRoot["bind_chat_tcp"]);
		SGString szRemoteChat = SGJson::getStringOrNull(gameServerRoot["remote_chat"]);

		SGString szBindInterServerUdp = SGJson::getStringOrNull(gameServerRoot["bind_interserver_udp"]);
		SGString szBindCenterTcp = SGJson::getStringOrNull(gameServerRoot["bind_center_tcp"]);

		DebugAssertMsg(
			!szBindGameTcp.IsNull() &&
			!szBindGameUdp.IsNull() &&
			!szRemoteGame.IsNull() &&

			!szBindTownTcp.IsNull() &&
			!szRemoteTown.IsNull() &&

			!szBindChatTcp.IsNull() &&
			!szRemoteChat.IsNull() &&

			!szBindInterServerUdp.IsNull() &&
			!szBindCenterTcp.IsNull()
			,
			"액티브 게임서버 정보인데 비어있는 설정이 있습니다."
		);

		
		info.BindGameTcp = szBindGameTcp;
		info.BindGameUdp = szBindGameUdp;
		info.RemoteGame = szRemoteGame;
		
		info.BindTownTcp = szBindTownTcp;
		info.RemoteTown = szRemoteTown;

		info.BindChatTcp = szBindChatTcp;
		info.RemoteChat = szRemoteChat;

		info.BindInterServerUdp = szBindInterServerUdp;
		info.BindCenterTcp = szBindCenterTcp;

		for (int j = 0; j < gameChannelListRoot.size(); ++j) {
			int iChannelNumber;
			int iChannelType;
			int iMaxPlayerCount;

			SGJson::parseIntNumber3(gameChannelListRoot[j], iChannelNumber, iChannelType, iMaxPlayerCount);

			info.MaxSessionCount += iMaxPlayerCount;
			info.GameChannelInfoList.PushBack({
				iChannelNumber,
				iChannelType,
				iMaxPlayerCount
			});
		}

		serverInfo->GameList.PushBack(Move(info));
	}
}

