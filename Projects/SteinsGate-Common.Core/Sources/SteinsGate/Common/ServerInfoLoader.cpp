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
	SGString szBindCenterTcp = JsonUtil::getString(serverRoot["bind_center_tcp"]);
	SGString szBindCenterUdp = JsonUtil::getString(serverRoot["bind_center_udp"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szRemoteCenter = JsonUtil::getString(serverRoot["remote_center"]);
	int iServerId = serverRoot["server_id"].asInt();

	ServerProcessInfo& info = serverInfo->Center;

	info.ProcessType = ServerProcessType::Center;
	info.Name = "중앙";
	info.BindInterServerUdp = szBindInterServerUdp;
	info.BindInterServerTcp = SGEndPoint::Invalid;
	info.BindTcp = szBindCenterTcp;
	info.BindUdp = szBindCenterUdp;
	info.RemoteEP = szRemoteCenter;
	info.RemoteInterServerEP = szRemoteInterServer;
	info.MaxSessionCount = serverRoot["max_session_count"].asInt();
	info.ServerId = iServerId;
	DebugAssertMsg(info.MaxSessionCount > 0, "센터 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &info;
}

void ServerInfoLoader::readAuthInfo(Json::Value& serverRoot, ServerProcessInfoPackage* serverInfo) {
	SGString szBindAuthTcp = JsonUtil::getString(serverRoot["bind_auth_tcp"]);
	SGString szBindAuthUdp = JsonUtil::getString(serverRoot["bind_auth_udp"]);
	SGString szRemoteAuth = JsonUtil::getString(serverRoot["remote_auth"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szBindInterServerUdp = JsonUtil::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindInterServerTcp = JsonUtil::getString(serverRoot["bind_interserver_tcp"]);
	int iServerId = serverRoot["server_id"].asInt();

	ServerProcessInfo& info = serverInfo->Auth;

	info.ProcessType = ServerProcessType::Auth;
	info.Name = "인증";
	info.BindTcp = szBindAuthTcp;
	info.BindUdp = szBindAuthUdp;
	info.RemoteEP = szRemoteAuth;
	info.RemoteInterServerEP = szRemoteInterServer;
	info.BindInterServerUdp = szBindInterServerUdp;
	info.BindInterServerTcp = szBindInterServerTcp;
	info.ServerId = iServerId;
	info.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(info.MaxSessionCount > 0, "오쓰 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &info;
}

void ServerInfoLoader::readLobbyInfo(Json::Value& serverRoot, ServerProcessInfoPackage* serverInfo) {
	SGString szBindLobbyTcp = JsonUtil::getString(serverRoot["bind_lobby_tcp"]);
	SGString szBindLobbyUdp = JsonUtil::getString(serverRoot["bind_lobby_udp"]);
	SGString szRemoteLobby = JsonUtil::getString(serverRoot["remote_lobby"]);
	SGString szRemoteInterServer = JsonUtil::getString(serverRoot["remote_interserver"]);
	SGString szBindInterServerUdp = JsonUtil::getString(serverRoot["bind_interserver_udp"]);
	SGString szBindInterServerTcp = JsonUtil::getString(serverRoot["bind_interserver_tcp"]);
	int iServerId = serverRoot["server_id"].asInt();


	ServerProcessInfo& info = serverInfo->Lobby;
	info.ProcessType = ServerProcessType::Lobby;
	info.Name = "로비";
	info.BindTcp = szBindLobbyTcp;
	info.BindUdp = szBindLobbyUdp;
	info.RemoteEP = szRemoteLobby;
	info.RemoteInterServerEP = szRemoteInterServer;
	info.BindInterServerUdp = szBindInterServerUdp;
	info.BindInterServerTcp = szBindInterServerTcp;
	info.ServerId = iServerId;
	info.MaxSessionCount = serverRoot["max_session_count"].asInt();
	DebugAssertMsg(info.MaxSessionCount > 0, "로비 맥스 세션 수는 0보다 커야합니다.");

	serverInfo->ActiveServerIdList.PushBack(iServerId);
	serverInfo->InfoMap[iServerId] = &info;
}


void ServerInfoLoader::readGameInfo(Value& serverRoot, JCORE_OUT ServerProcessInfoPackage* serverInfo) {

	const int iGameServerCount = (int)serverRoot.size();

	for (int i = 0; i < iGameServerCount; ++i) {
		Value& gameServerRoot = serverRoot[i];

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


		info.ProcessType = ServerProcessType::Game;
		info.GameServerType = (GameServerType_t)eGameServerType;
		info.Name = StringUtil::Format("게임(%s)", GameServerType::Name[eGameServerType]);
		info.Active = true;
		info.ServerId = iServerId;

		SGString szBindLogicTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_logic_tcp"]);
		SGString szBindLogicUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_logic_udp"]);
		SGString szRemoteLogic = JsonUtil::getStringOrNull(gameServerRoot["remote_logic"]);
		SGString szRemoteInterServer = JsonUtil::getString(gameServerRoot["remote_interserver"]);

		SGString szBindAreaTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_area_tcp"]);
		SGString szBindAreaUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_area_udp"]);
		SGString szRemoteArea = JsonUtil::getStringOrNull(gameServerRoot["remote_area"]);

		SGString szBindChatTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_chat_tcp"]);
		SGString szBindChatUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_chat_udp"]);
		SGString szRemoteChat = JsonUtil::getStringOrNull(gameServerRoot["remote_chat"]);

		SGString szBindInterServerUdp = JsonUtil::getStringOrNull(gameServerRoot["bind_interserver_udp"]);
		SGString szBindInterServerTcp = JsonUtil::getStringOrNull(gameServerRoot["bind_interserver_tcp"]);

		DebugAssertMsg(
			!szBindLogicTcp.IsNull() &&
			!szBindLogicUdp.IsNull() &&
			!szRemoteLogic.IsNull() &&

			!szBindAreaTcp.IsNull() &&
			!szBindAreaUdp.IsNull() &&
			!szRemoteArea.IsNull() &&

			!szBindChatTcp.IsNull() &&
			!szBindChatUdp.IsNull() &&
			!szRemoteChat.IsNull() &&

			!szBindInterServerUdp.IsNull() &&
			!szBindInterServerTcp.IsNull()
			,
			"액티브 게임서버 정보인데 비어있는 설정이 있습니다."
		);

		
		info.BindTcp = Move(szBindLogicTcp);
		info.BindUdp = Move(szBindLogicUdp);

		info.RemoteEP = Move(szRemoteLogic);
		info.RemoteInterServerEP = Move(szRemoteInterServer);
		
		info.BindAreaTcp = Move(szBindAreaTcp);
		info.BindAreaUdp = Move(szBindAreaUdp);
		info.RemoteArea = Move(szRemoteArea);

		info.BindChatTcp = Move(szBindChatTcp);
		info.BindChatUdp = Move(szBindChatUdp);
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

		serverInfo->GameServerList.PushBack(Move(info));
		serverInfo->ActiveServerIdList.PushBack(iServerId);
		serverInfo->InfoMap[iServerId] = &serverInfo->GameServerList[serverInfo->GameServerList.Size() - 1];
	}
}

