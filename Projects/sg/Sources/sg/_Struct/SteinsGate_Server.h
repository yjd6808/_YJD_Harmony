/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */

#pragma once

#include <sg/_Util/DescBase.h>

//////////////////////////////////////////////////////////////////////////////////////////
// 전체 서버 종류
JC_SENUM_BEGIN(ServerType)
	Auth,
	Begin = Auth,
	Lobby,
	Logic,
	Chat,
	Area,
	Center,
	End = Center,
	Max
JC_SENUM_MIDDLE(ServerType)

static constexpr const char* Name[Max]{
	"인증",
	"로비",
	"로직",
	"채팅",
	"지역",
	"중앙"
};

JC_SENUM_MIDDLE_END(ServerType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(SingleServerType)
	Center,
	Begin = Center,
	Auth,
	Lobby,
	End = Lobby,
	Max
JC_SENUM_END(SingleServerType)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(GameServerType)
	Begin,
	Luke = Begin,
	Hilder,
	Siroco,
	Prey,
	Kasias, // 대충 발음나는데..
	Diregie,
	Cain,
	Seria,
	End = Seria,
	Max
JC_SENUM_MIDDLE(GameServerType)
static constexpr const char* Name[Max]{
	"루크",
	"힐더",
	"시로코",
	"프레이",
	"카시아스",
	"디레지에",
	"카인",
	"세리아"
};
JC_SENUM_MIDDLE_END(GameServerType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(ServerProcessType)
		None,
		Auth,				// 1
		Begin = Auth,
		Center,				// 2
		Lobby,				// 3
		Game,				// 4
		End = Game
JC_SENUM_MIDDLE(ServerProcessType)
static constexpr int Count = End;	// 4
static constexpr const char* Name[Count + 1]{
	"없음",
	"인증",
	"중앙",
	"로비",
	"게임",
};

JC_SENUM_MIDDLE_END(ServerProcessType)

//////////////////////////////////////////////////////////////////////////////////////////
// 클라이언트가 게임을 플레이하는 동안 한번이라도 접속하게되는 서버타입
JC_SENUM_BEGIN(ClientConnectServerType)
	Auth,
	Begin = Auth,
	Lobby,
	Logic,
	Chat,
	Area,
	End = Area,
	Max
JC_SENUM_END(ClientConnectServerType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(GamePlayMode)
	Single,
	Lan,
	Wan,
	Max
JC_SENUM_MIDDLE(GamePlayMode)

static constexpr bool IsNetworkMode[Max]{
	false,
	true,
};

JC_SENUM_MIDDLE_END(GamePlayMode)

//////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////
struct GameChannelInfo
{
	int channelNumber_;
	int channelType_;
	int maxPlayerCount_;
};

//////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////
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
