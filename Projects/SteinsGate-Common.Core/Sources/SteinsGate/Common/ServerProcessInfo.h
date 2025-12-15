/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Common/Enum.h>

struct ServerProcessInfo
{
	ServerProcessType_t processType_;
	SGString name_;

	// JSON에서 읽은 데이터
	SGEndPoint bindInterServerUdp_;
	SGEndPoint bindInterServerTcp_; // 중앙서버X
	SGEndPoint bindTcp_; // 게임서버는 로직서버
	SGEndPoint bindUdp_; // 게임서버는 로직서버
	SGEndPoint remoteInterServerEp_;
	SGEndPoint remoteEp_;
	int serverId_ = -1;
	int maxSessionCount_ = 0;
};

struct GameChannelInfo
{
	int channelNumber_;
	int channelType_;
	int maxPlayerCount_;
};

struct GameServerProcessInfo : ServerProcessInfo
{
	GameServerProcessInfo()
	: gameServerType_()
	, active_(false)
	, gameChannelInfoList_(1)
	{
	}

	GameServerProcessInfo(int _channelCount)
	: gameServerType_()
	, active_(false)
	, gameChannelInfoList_(_channelCount)
	{
	}

	GameServerType_t gameServerType_;
	SGEndPoint bindChatTcp_;
	SGEndPoint bindChatUdp_;
	SGEndPoint remoteChat_;

	SGEndPoint bindAreaTcp_;
	SGEndPoint bindAreaUdp_;
	SGEndPoint remoteArea_;

	bool active_;
	SGVector<GameChannelInfo> gameChannelInfoList_;
};

struct ServerProcessInfoPackage : ConfigDataAbstract
{
	ServerProcessInfoPackage(int _activeGameServerCount)
	: auth_(), center_(), lobby_(), gameServerList_(_activeGameServerCount)
	, activeServerIdList_(3 + _activeGameServerCount), infoMap_{}
	// 인증 + 로비 + 중앙 + 게임 서버들
	{
	}

	~ServerProcessInfoPackage() override = default;

	SGString GetServerProcessName(int _serverId);
	ServerProcessInfo* GetServerProcessInfo(int _serverId);
	GameServerProcessInfo* GetGameServerProcessInfo(GameServerType_t _gameServerType);

	SGString name_;
	ServerProcessInfo auth_;
	ServerProcessInfo center_;
	ServerProcessInfo lobby_;
	SGVector<GameServerProcessInfo> gameServerList_;
	SGVector<int> activeServerIdList_;
	ServerProcessInfo* infoMap_[Const::Server::MaxProcessId];
};
