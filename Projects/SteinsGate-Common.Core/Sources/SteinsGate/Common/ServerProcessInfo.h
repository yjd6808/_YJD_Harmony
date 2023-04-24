/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct CenterServerProcessInfo
{
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	SGEndPoint RemoteCenter;
	int MaxSessionCount;
};


struct AuthServerProcessInfo
{
	SGEndPoint BindAuthTcp;
	SGEndPoint RemoteAuth;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	int MaxSessionCount;
};

struct LobbyServerProcessInfo
{
	SGEndPoint BindLobbyTcp;
	SGEndPoint RemoteLobby;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	int MaxSessionCount;
};

struct GameChannelInfo
{
	int ChannelNumber;
	int ChannelType;
	int MaxPlayerCount;
};

struct GameServerProcessInfo
{
	GameServerProcessInfo() : GameChannelInfoList(1) {}
	GameServerProcessInfo(int channelCount)
		: GameChannelInfoList(channelCount)
		, MaxSessionCount(0) {}

	ServerType_t Code;
	SGString Name;
	
	SGEndPoint BindGameTcp;
	SGEndPoint BindGameUdp;
	SGEndPoint RemoteGame;

	SGEndPoint BindChatTcp;
	SGEndPoint RemoteChat;

	SGEndPoint BindTownTcp;
	SGEndPoint RemoteTown;

	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	bool Active;
	SGVector<GameChannelInfo> GameChannelInfoList;
	int MaxSessionCount;
};


struct ServerProcessInfo : ConfigDataAbstract
{
	ServerProcessInfo(int activeGameServerCount) : GameList(activeGameServerCount) {}
	~ServerProcessInfo() override = default;

	SGString Name;
	CenterServerProcessInfo Center;
	AuthServerProcessInfo Auth;
	LobbyServerProcessInfo Lobby;
	SGVector<GameServerProcessInfo> GameList;
};
