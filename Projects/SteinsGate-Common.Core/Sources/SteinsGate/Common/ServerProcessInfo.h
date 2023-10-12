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
	ServerProcessType_t ProcessType;
	SGString Name;

	// JSON에서 읽은 데이터
	SGEndPoint BindInterServerUdp;
	SGEndPoint BindInterServerTcp;	// 중앙서버X
	SGEndPoint BindTcp;				// 게임서버는 로직서버
	SGEndPoint BindUdp;				// 게임서버는 로직서버
	SGEndPoint RemoteInterServerEP;
	SGEndPoint RemoteEP;
	int ServerId = - 1;
	int MaxSessionCount = 0;

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

	GameServerType_t GameServerType;
	SGEndPoint BindChatTcp;
	SGEndPoint BindChatUdp;
	SGEndPoint RemoteChat;

	SGEndPoint BindAreaTcp;
	SGEndPoint BindAreaUdp;
	SGEndPoint RemoteArea;

	bool Active;
	SGVector<GameChannelInfo> GameChannelInfoList;
};


struct ServerProcessInfoPackage : ConfigDataAbstract
{
	ServerProcessInfoPackage(int activeGameServerCount)
		: GameServerList(activeGameServerCount)
		, ActiveServerIdList(3 + activeGameServerCount)	// 인증 + 로비 + 중앙 + 게임 서버들
	{}
	~ServerProcessInfoPackage() override = default;

	SGString getServerProcessName(int serverId);
	ServerProcessInfo* getServerProcessInfo(int serverId);
	GameServerProcessInfo* getGameServerProcessInfo(GameServerType_t gameServerType);

	SGString Name;
	ServerProcessInfo Auth;
	ServerProcessInfo Center;
	ServerProcessInfo Lobby;
	SGVector<GameServerProcessInfo> GameServerList;
	SGVector<int> ActiveServerIdList;
	ServerProcessInfo* InfoMap[Const::Server::MaxProcessId];
};


