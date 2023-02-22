/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct CenterServerInfo
{
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	SGEndPoint RemoteCenter;
	int MaxSessionCount;
};


struct AuthServerInfo
{
	SGEndPoint BindAuthTcp;
	SGEndPoint RemoteAuth;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	int MaxSessionCount;
};

struct LobbyServerInfo
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

struct GameServerInfo
{
	GameServerInfo() : GameChannelInfoList(1) {}
	GameServerInfo(int channelCount)
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


struct ServerInfo : ConfigDataAbstract
{
	ServerInfo(int activeGameServerCount) : GameList(activeGameServerCount) {}
	~ServerInfo() override = default;

	SGString Name;
	CenterServerInfo Center;
	AuthServerInfo Auth;
	LobbyServerInfo Lobby;
	SGVector<GameServerInfo> GameList;
};
