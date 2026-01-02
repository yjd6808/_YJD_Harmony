/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */

#pragma once

#include <sg/_Util/DescBase.h>
#include <sg/Enum.h>

struct ServerProcessInfo
{
	ServerProcessType_t processType_;
	jc::String name_;

	// JSON에서 읽은 데이터
	jnet::IPv4EndPoint bindInterServerUdp_;
	jnet::IPv4EndPoint bindInterServerTcp_; // 중앙서버X
	jnet::IPv4EndPoint bindTcp_; // 게임서버는 로직서버
	jnet::IPv4EndPoint bindUdp_; // 게임서버는 로직서버
	jnet::IPv4EndPoint remoteInterServerEp_;
	jnet::IPv4EndPoint remoteEp_;
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
	jnet::IPv4EndPoint bindChatTcp_;
	jnet::IPv4EndPoint bindChatUdp_;
	jnet::IPv4EndPoint remoteChat_;

	jnet::IPv4EndPoint bindAreaTcp_;
	jnet::IPv4EndPoint bindAreaUdp_;
	jnet::IPv4EndPoint remoteArea_;

	bool active_;
	jc::Vector<GameChannelInfo> gameChannelInfoList_;
};

struct ServerProcessInfoPackage : SDescBase
{
	ServerProcessInfoPackage(int _activeGameServerCount)
	: auth_(), center_(), lobby_(), gameServerList_(_activeGameServerCount)
	, activeServerIdList_(3 + _activeGameServerCount), infoMap_{}
	// 인증 + 로비 + 중앙 + 게임 서버들
	{
	}

	~ServerProcessInfoPackage() override = default;

	jc::String GetServerProcessName(int _serverId);
	ServerProcessInfo* GetServerProcessInfo(int _serverId);
	GameServerProcessInfo* GetGameServerProcessInfo(GameServerType_t _gameServerType);

	jc::String name_;
	ServerProcessInfo auth_;
	ServerProcessInfo center_;
	ServerProcessInfo lobby_;
	jc::Vector<GameServerProcessInfo> gameServerList_;
	jc::Vector<int> activeServerIdList_;
	ServerProcessInfo* infoMap_[Const::Server::MaxProcessId];
};
