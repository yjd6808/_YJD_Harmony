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
};


struct AuthServerInfo
{
	SGEndPoint BindAuthTcp;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	SGEndPoint ConnectCenterTcp;
};

struct LobbyServerInfo
{
	SGEndPoint BindLobbyTcp;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	SGEndPoint ConnectCenterTcp;
};

struct GameServerInfo
{
	ServerType_t Code;
	SGString Name;
	
	SGEndPoint BindGameTcp;
	SGEndPoint BindGameUdp;
	SGEndPoint BindChatTcp;
	SGEndPoint BindTownTcp;
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindCenterTcp;
	SGEndPoint ConnectCenterTcp;
	bool Active;

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
