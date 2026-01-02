/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:50 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerInfoLoader.h"

#include <sg/_Util/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ServerInfoLoader::ServerInfoLoader(DescLoaderMgr* _pManager)
: DescLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ServerInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& serverInfoRootList = root["server"];

		for (int serverInfoIndex = 0; serverInfoIndex < serverInfoRootList.size(); ++serverInfoIndex)
		{
			Value& serverInfoRoot = serverInfoRootList[serverInfoIndex];
			Value& gameServerInfoListRoot = serverInfoRoot["game"];
			int gameServerCount = (int)gameServerInfoListRoot.size();

			jc_assert_msg(gameServerCount > 0, "게임 서버 카운트가 0입니다.");
			ServerProcessInfoPackage* pServerInfo = dbg_new ServerProcessInfoPackage(gameServerCount);
			pServerInfo->code_ = serverInfoRoot["code"].asInt();
			pServerInfo->name_ = JsonUtil::GetString(serverInfoRoot["name"]);
			ReadCenterInfo(serverInfoRoot["center"], pServerInfo);
			ReadAuthInfo(serverInfoRoot["auth"], pServerInfo);
			ReadLobbyInfo(serverInfoRoot["lobby"], pServerInfo);
			ReadGameInfo(gameServerInfoListRoot, pServerInfo);
			AddData(pServerInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadCenterInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo)
{
	jc::String bindInterServerUdp = JsonUtil::GetString(_serverRoot["bind_interserver_udp"]);
	jc::String bindCenterTcp = JsonUtil::GetString(_serverRoot["bind_center_tcp"]);
	jc::String bindCenterUdp = JsonUtil::GetString(_serverRoot["bind_center_udp"]);
	jc::String remoteInterServer = JsonUtil::GetString(_serverRoot["remote_interserver"]);
	jc::String remoteCenter = JsonUtil::GetString(_serverRoot["remote_center"]);
	int serverId = _serverRoot["server_id"].asInt();

	ServerProcessInfo& info = _pServerInfo->center_;

	info.processType_ = ServerProcessType::Center;
	info.name_ = "중앙";
	info.bindInterServerUdp_ = IPv4EndPoint{ bindInterServerUdp };
	info.bindInterServerTcp_ = IPv4EndPoint::Invalid;
	info.bindTcp_ = IPv4EndPoint{ bindCenterTcp };
	info.bindUdp_ = IPv4EndPoint{ bindCenterUdp };
	info.remoteEp_ = IPv4EndPoint{ remoteCenter };
	info.remoteInterServerEp_ = IPv4EndPoint{ remoteInterServer };
	info.maxSessionCount_ = _serverRoot["max_session_count"].asInt();
	info.serverId_ = serverId;
	jc_assert_msg(info.maxSessionCount_ > 0, "센터 맥스 세션 수는 0보다 커야합니다.");

	_pServerInfo->activeServerIdList_.PushBack(serverId);
	_pServerInfo->infoMap_[serverId] = &info;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadAuthInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo)
{
	jc::String bindAuthTcp = JsonUtil::GetString(_serverRoot["bind_auth_tcp"]);
	jc::String bindAuthUdp = JsonUtil::GetString(_serverRoot["bind_auth_udp"]);
	jc::String remoteAuth = JsonUtil::GetString(_serverRoot["remote_auth"]);
	jc::String remoteInterServer = JsonUtil::GetString(_serverRoot["remote_interserver"]);
	jc::String bindInterServerUdp = JsonUtil::GetString(_serverRoot["bind_interserver_udp"]);
	jc::String bindInterServerTcp = JsonUtil::GetString(_serverRoot["bind_interserver_tcp"]);
	int serverId = _serverRoot["server_id"].asInt();

	ServerProcessInfo& info = _pServerInfo->auth_;

	info.processType_ = ServerProcessType::Auth;
	info.name_ = "인증";
	info.bindTcp_ = IPv4EndPoint{ bindAuthTcp };
	info.bindUdp_ = IPv4EndPoint{ bindAuthUdp };
	info.remoteEp_ = IPv4EndPoint{ remoteAuth };
	info.remoteInterServerEp_ = IPv4EndPoint{ remoteInterServer };
	info.bindInterServerUdp_ = IPv4EndPoint{ bindInterServerUdp };
	info.bindInterServerTcp_ = IPv4EndPoint{ bindInterServerTcp };
	info.serverId_ = serverId;
	info.maxSessionCount_ = _serverRoot["max_session_count"].asInt();
	jc_assert_msg(info.maxSessionCount_ > 0, "오쓰 맥스 세션 수는 0보다 커야합니다.");

	_pServerInfo->activeServerIdList_.PushBack(serverId);
	_pServerInfo->infoMap_[serverId] = &info;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadLobbyInfo(Json::Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo)
{
	jc::String bindLobbyTcp = JsonUtil::GetString(_serverRoot["bind_lobby_tcp"]);
	jc::String bindLobbyUdp = JsonUtil::GetString(_serverRoot["bind_lobby_udp"]);
	jc::String remoteLobby = JsonUtil::GetString(_serverRoot["remote_lobby"]);
	jc::String remoteInterServer = JsonUtil::GetString(_serverRoot["remote_interserver"]);
	jc::String bindInterServerUdp = JsonUtil::GetString(_serverRoot["bind_interserver_udp"]);
	jc::String bindInterServerTcp = JsonUtil::GetString(_serverRoot["bind_interserver_tcp"]);
	int serverId = _serverRoot["server_id"].asInt();

	ServerProcessInfo& info = _pServerInfo->lobby_;
	info.processType_ = ServerProcessType::Lobby;
	info.name_ = "로비";
	info.bindTcp_ = IPv4EndPoint{ bindLobbyTcp };
	info.bindUdp_ = IPv4EndPoint{ bindLobbyUdp };
	info.remoteEp_ = IPv4EndPoint{ remoteLobby };
	info.remoteInterServerEp_ = IPv4EndPoint{ remoteInterServer };
	info.bindInterServerUdp_ = IPv4EndPoint{ bindInterServerUdp };
	info.bindInterServerTcp_ = IPv4EndPoint{ bindInterServerTcp };
	info.serverId_ = serverId;
	info.maxSessionCount_ = _serverRoot["max_session_count"].asInt();
	jc_assert_msg(info.maxSessionCount_ > 0, "로비 맥스 세션 수는 0보다 커야합니다.");

	_pServerInfo->activeServerIdList_.PushBack(serverId);
	_pServerInfo->infoMap_[serverId] = &info;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadGameInfo(Value& _serverRoot, OUT ServerProcessInfoPackage* _pServerInfo)
{
	const int gameServerCount = (int)_serverRoot.size();

	for (int gameServerIndex = 0; gameServerIndex < gameServerCount; ++gameServerIndex)
	{
		Value& gameServerRoot = _serverRoot[gameServerIndex];

		bool isActive = gameServerRoot["active"].asBool();

		if (!isActive)
		{
			continue;
		}

		Value& gameChannelListRoot = gameServerRoot["active_channel"];
		int gameChannelCount = int(gameChannelListRoot.size());
		jc_assert_msg(gameChannelCount > 0, "액티브 게임서버인데 활성화된 채널 정보가 아무것도 없습니다.");
		GameServerProcessInfo info(gameChannelCount);
		int serverId = gameServerRoot["server_id"].asInt();
		int gameServerType = gameServerRoot["game_server_type"].asInt();

		if (gameServerType < GameServerType::Begin || gameServerType > GameServerType::End)
		{
			_LogWarn_("올바른 게임서버 타입이 아닙니다.");
			continue;
		}

		info.processType_ = ServerProcessType::Game;
		info.gameServerType_ = (GameServerType_t)gameServerType;
		info.name_ = StringUtil::Format("게임(%s)", GameServerType::Name[gameServerType]);
		info.active_ = true;
		info.serverId_ = serverId;

		jc::String bindLogicTcp = JsonUtil::GetStringOrNull(gameServerRoot["bind_logic_tcp"]);
		jc::String bindLogicUdp = JsonUtil::GetStringOrNull(gameServerRoot["bind_logic_udp"]);
		jc::String remoteLogic = JsonUtil::GetStringOrNull(gameServerRoot["remote_logic"]);
		jc::String remoteInterServer = JsonUtil::GetString(gameServerRoot["remote_interserver"]);

		jc::String bindAreaTcp = JsonUtil::GetStringOrNull(gameServerRoot["bind_area_tcp"]);
		jc::String bindAreaUdp = JsonUtil::GetStringOrNull(gameServerRoot["bind_area_udp"]);
		jc::String remoteArea = JsonUtil::GetStringOrNull(gameServerRoot["remote_area"]);

		jc::String bindChatTcp = JsonUtil::GetStringOrNull(gameServerRoot["bind_chat_tcp"]);
		jc::String bindChatUdp = JsonUtil::GetStringOrNull(gameServerRoot["bind_chat_udp"]);
		jc::String remoteChat = JsonUtil::GetStringOrNull(gameServerRoot["remote_chat"]);

		jc::String bindInterServerUdp = JsonUtil::GetStringOrNull(gameServerRoot["bind_interserver_udp"]);
		jc::String bindInterServerTcp = JsonUtil::GetStringOrNull(gameServerRoot["bind_interserver_tcp"]);

		jc_assert_msg(
			!bindLogicTcp.IsNull() &&
			!bindLogicUdp.IsNull() &&
			!remoteLogic.IsNull() &&

			!bindAreaTcp.IsNull() &&
			!bindAreaUdp.IsNull() &&
			!remoteArea.IsNull() &&

			!bindChatTcp.IsNull() &&
			!bindChatUdp.IsNull() &&
			!remoteChat.IsNull() &&

			!bindInterServerUdp.IsNull() &&
			!bindInterServerTcp.IsNull(),
			"액티브 게임서버 정보인데 비어있는 설정이 있습니다."
		);

		info.bindTcp_ = IPv4EndPoint{ Move(bindLogicTcp) };
		info.bindUdp_ = IPv4EndPoint{ Move(bindLogicUdp) };

		info.remoteEp_ = IPv4EndPoint{ Move(remoteLogic) };
		info.remoteInterServerEp_ = IPv4EndPoint{ Move(remoteInterServer) };

		info.bindAreaTcp_ = IPv4EndPoint{ Move(bindAreaTcp) };
		info.bindAreaUdp_ = IPv4EndPoint{ Move(bindAreaUdp) };
		info.remoteArea_ = IPv4EndPoint{ Move(remoteArea) };

		info.bindChatTcp_ = IPv4EndPoint{ Move(bindChatTcp) };
		info.bindChatUdp_ = IPv4EndPoint{ Move(bindChatUdp) };
		info.remoteChat_ = IPv4EndPoint{ Move(remoteChat) };

		info.bindInterServerUdp_ = IPv4EndPoint{ Move(bindInterServerUdp) };
		info.bindInterServerTcp_ = IPv4EndPoint{ Move(bindInterServerTcp) };

		for (int channelIndex = 0; channelIndex < gameChannelListRoot.size(); ++channelIndex)
		{
			int channelNumber;
			int channelType;
			int maxPlayerCount;

			JsonUtil::ParseIntNumber3(gameChannelListRoot[channelIndex], channelNumber, channelType, maxPlayerCount);

			info.maxSessionCount_ += maxPlayerCount;
			info.gameChannelInfoList_.PushBack({
				channelNumber,
				channelType,
				maxPlayerCount
			});
		}

		_pServerInfo->gameServerList_.PushBack(Move(info));
		_pServerInfo->activeServerIdList_.PushBack(serverId);
		_pServerInfo->infoMap_[serverId] = &_pServerInfo->gameServerList_[_pServerInfo->gameServerList_.Size() - 1];
	}
}
