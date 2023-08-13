/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:50 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/JsonUtil.h>

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
			ServerProcessInfoPackage* pServerInfo = dbg_new ServerProcessInfoPackage(gameServerCount);
			pServerInfo->Code = serverInfoRoot["code"].asInt();
			pServerInfo->Name = JsonUtil::getString(serverInfoRoot["name"]);
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



void ServerInfoLoader::readCenterInfo(Json::Value& serverRoot, ServerProcessInfoPackage* serverInfo) {
	SGString szBindInterServerUdp = JsonUtil::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindInterServerTcp = JsonUtil::getString(serverRoot["bind_interserver_tcp"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szRemoteCenter = JsonUtil::getString(serverRoot["remote_center"]);
	int iServerId = serverRoot["server_id"].asInt();

	serverInfo->Center.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Center.BindInterServerTcp = szBindInterServerTcp;
	serverInfo->Center.RemoteEP = szRemoteCenter;
	serverInfo->Center.RemoteInterServerEP = szRemoteInterServer;
	serverInfo->Center.MaxSessionCount = serverRoot["max_session_count"].asInt();
	serverInfo->Center.ServerId = iServerId;
	DebugAssertMsg(serverInfo->Center.MaxSessionCount > 0, "센터 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &serverInfo->Center;
}

void ServerInfoLoader::readAuthInfo(Json::Value& serverRoot, ServerProcessInfoPackage* serverInfo) {
	SGString szBindAuthTcp = JsonUtil::getString(serverRoot["bind_auth_tcp"]);
	SGString szRemoteAuth = JsonUtil::getString(serverRoot["remote_auth"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szBindInterServerUdp = JsonUtil::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindInterServerTcp = JsonUtil::getString(serverRoot["bind_interserver_tcp"]);
	int iServerId = serverRoot["server_id"].asInt();

	serverInfo->Auth.BindAuthTcp = szBindAuthTcp;
	serverInfo->Auth.RemoteEP = szRemoteAuth;
	serverInfo->Auth.RemoteInterServerEP = szRemoteInterServer;
	serverInfo->Auth.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Auth.BindInterServerTcp = szBindInterServerTcp;
	serverInfo->Auth.ServerId = iServerId;
	serverInfo->Auth.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(serverInfo->Auth.MaxSessionCount > 0, "오쓰 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &serverInfo->Auth;
}

void ServerInfoLoader::readLobbyInfo(Json::Value& serverRoot, ServerProcessInfoPackage* serverInfo) {
	SGString szBindLobbyTcp = JsonUtil::getString(serverRoot["bind_lobby_tcp"]);
	SGString szRemoteLobby = JsonUtil::getString(serverRoot["remote_lobby"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szBindInterServerUdp = JsonUtil::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindInterServerTcp = JsonUtil::getString(serverRoot["bind_interserver_tcp"]);
	int iServerId = serverRoot["server_id"].asInt();

	serverInfo->Lobby.BindLobbyTcp = szBindLobbyTcp;
	serverInfo->Lobby.RemoteEP = szRemoteLobby;
	serverInfo->Lobby.RemoteInterServerEP = szRemoteInterServer;
	serverInfo->Lobby.BindInterServerUdp = szBindInterServerUdp;
	serverInfo->Lobby.BindInterServerTcp = szBindInterServerTcp;
	serverInfo->Lobby.ServerId = iServerId;
	serverInfo->Lobby.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(serverInfo->Lobby.MaxSessionCount > 0, "로비 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &serverInfo->Lobby;
}


void ServerInfoLoader::readGameInfo(Json::Value& gameServerLostRoot, ServerProcessInfoPackage* serverInfo) {

	int iGameServerCount = (int)gameServerLostRoot.size();

	for (int i = 0; i < iGameServerCount; ++i) {
		Value& gameServerRoot = gameServerLostRoot[i];

		bool bActive = gameServerRoot["active"].asBool();

		if (!bActive)
			continue;

		Value& gameChannelListRoot = gameServerRoot["active_channel"];
		int iGameChannelCount = int(gameChannelListRoot.size());
		DebugAssertMsg(iGameChannelCount > 0, "액티브 게임서버인데 활성화된 채널 정보가 아무것도 없습니다.");
		GameServerProcessInfo info(iGameChannelCount);
		int iServerId = gameServerRoot["server_id"].asInt();
		int eGameServerType = gameServerRoot["game_server_type"].asInt();

		if (eGameServerType < GameServerType::Begin || eGameServerType > GameServerType::End) {
			_LogWarn_("올바른 게임서버 타입이 아닙니다.");
			continue;
		}

			
		info.Type = (GameServerType_t)eGameServerType;
		info.Name = JsonUtil::getString(gameServerRoot["name"]);
		info.Active = true;
		info.ServerId = iServerId;

		SGString szBindLogicTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_logic_tcp"]);
		SGString szBindLogicUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_logic_udp"]);
		SGString szRemoteLogic = JsonUtil::getStringOrNull(gameServerRoot["remote_logic"]);
		SGString szRemoteInterServer = JsonUtil::getString(gameServerRoot["remote_interserver"]);

		SGString szBindAreaTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_area_tcp"]);
		SGString szRemoteArea = JsonUtil::getStringOrNull(gameServerRoot["remote_area"]);

		SGString szBindChatTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_chat_tcp"]);
		SGString szRemoteChat = JsonUtil::getStringOrNull(gameServerRoot["remote_chat"]);

		SGString szBindInterServerUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_interserver_udp"]);
		SGString szBindInterServerTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_interserver_tcp"]);

		DebugAssertMsg(
			!szBindLogicTcp.IsNull() &&
			!szBindLogicUdp.IsNull() &&
			!szRemoteLogic.IsNull() &&

			!szBindAreaTcp.IsNull() &&
			!szRemoteArea.IsNull() &&

			!szBindChatTcp.IsNull() &&
			!szRemoteChat.IsNull() &&

			!szBindInterServerUdp.IsNull() &&
			!szBindInterServerTcp.IsNull()
			,
			"액티브 게임서버 정보인데 비어있는 설정이 있습니다."
		);

		
		info.BindLogicTcp = Move(szBindLogicTcp);
		info.BindLogicUdp = Move(szBindLogicUdp);
		info.RemoteEP = Move(szRemoteLogic);
		info.RemoteInterServerEP = Move(szRemoteInterServer);
		
		info.BindAreaTcp = Move(szBindAreaTcp);
		info.RemoteArea = Move(szRemoteArea);

		info.BindChatTcp = Move(szBindChatTcp);
		info.RemoteChat = Move(szRemoteChat);

		info.BindInterServerUdp = Move(szBindInterServerUdp);
		info.BindInterServerTcp = Move(szBindInterServerTcp);

		for (int j = 0; j < gameChannelListRoot.size(); ++j) {
			int iChannelNumber;
			int iChannelType;
			int iMaxPlayerCount;

			JsonUtil::parseIntNumber3(gameChannelListRoot[j], iChannelNumber, iChannelType, iMaxPlayerCount);

			info.MaxSessionCount += iMaxPlayerCount;
			info.GameChannelInfoList.PushBack({
				iChannelNumber,
				iChannelType,
				iMaxPlayerCount
			});
		}

		serverInfo->GameList.PushBack(Move(info));
		serverInfo->ActiveServerIdList.PushBack(iServerId);
		serverInfo->InfoMap[iServerId] = &serverInfo->GameList[serverInfo->GameList.Size() - 1];
	}
}

