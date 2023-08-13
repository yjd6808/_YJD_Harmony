/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ServerProcessInfo
{
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindInterServerTcp;
	SGEndPoint RemoteInterServerEP;
	SGEndPoint RemoteEP;
	int ServerId = - 1;
	int MaxSessionCount = 0;
};

struct CenterServerProcessInfo : ServerProcessInfo
{
};


struct AuthServerProcessInfo : ServerProcessInfo
{
	SGEndPoint BindAuthTcp;
};

struct LobbyServerProcessInfo : ServerProcessInfo
{
	SGEndPoint BindLobbyTcp;
};

struct GameChannelInfo
{
	int ChannelNumber;
	int ChannelType;
	int MaxPlayerCount;
};

struct GameServerProcessInfo : ServerProcessInfo
{
	GameServerProcessInfo() : GameChannelInfoList(1) {}
	GameServerProcessInfo(int channelCount)
		: GameChannelInfoList(channelCount)
	{}

	GameServerType_t Type;
	SGString Name;
	
	SGEndPoint BindLogicTcp;
	SGEndPoint BindLogicUdp;

	SGEndPoint BindChatTcp;
	SGEndPoint RemoteChat;

	SGEndPoint BindAreaTcp;
	SGEndPoint RemoteArea;

	bool Active;
	SGVector<GameChannelInfo> GameChannelInfoList;
};


struct ServerProcessInfoPackage : ConfigDataAbstract
{
	ServerProcessInfoPackage(int activeGameServerCount)
		: GameList(activeGameServerCount)
		, ActiveServerIdList(3 + activeGameServerCount)	// 인증 + 로비 + 중앙 + 게임 서버들
	{}
	~ServerProcessInfoPackage() override = default;

	GameServerProcessInfo* getGameServerProcessInfo(GameServerType_t gameServerType);

	SGString Name;
	CenterServerProcessInfo Center;
	AuthServerProcessInfo Auth;
	LobbyServerProcessInfo Lobby;
	SGVector<GameServerProcessInfo> GameList;
	SGVector<int> ActiveServerIdList;
	ServerProcessInfo* InfoMap[MaxServerId_v];
};


