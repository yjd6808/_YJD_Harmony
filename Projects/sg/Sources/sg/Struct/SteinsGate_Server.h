/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:34 AM
 * =====================
 *
 */

#pragma once

#include "sg/Util/DescBase.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 전체 서버 종류
JC_SENUM_BEGIN(ServerType)
	Auth,
	Begin = Auth,
	Lobby,
	Game,
	Chat,
	Area,
	Center,
	Query,
	None,
	End = None,
	Max
JC_SENUM_MIDDLE(ServerType)

static constexpr const char* Name[Max]{
	"인증",
	"로비",
	"게임",
	"채팅",
	"지역",
	"중앙",
	"알 수 없음"
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
	None,
	Max = None
JC_SENUM_MIDDLE(GameServerType)
static constexpr const char* Name[Max]{
	"루크",
	"힐더",
	"시로코",
	"프레이",
	"카시아스",
	"디레지에",
	"카인",
	"세리아",
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
		Query,				// 5
		End = Game,
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
struct NetServerInfo	// 서브 프로세스는 여러개의 서버를 가질 수 있음.
{
	int serverType_ = -1;
	jc::String serverName_;
	int maxSessionCount_ = 50;
	int tcpRecvBufferSize_ = 2048;
	int tcpSendBufferSize_ = 2048;
	int udpRecvBufferSize_ = 2048;
	int udpSendBufferSize_ = 2048;
	jnet::IPv4EndPoint bindTcp_;
	jnet::IPv4EndPoint bindUdp_;
	jnet::IPv4EndPoint remoteEp_;

	static NetServerInfo dummy_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct NetInterServerInfo
{
	jnet::IPv4EndPoint bindTcp_;
	jnet::IPv4EndPoint bindUdp_;
	jnet::IPv4EndPoint remoteCenterServerEp_;
	int iocpThreadCount_ = 2;
	int tcpRecvBufferSize_ = 2048;
	int tcpSendBufferSize_ = 2048;
	int udpRecvBufferSize_ = 2048;
	int udpSendBufferSize_ = 2048;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct NetDBInfo_MySQL
{
	jc::String name_; // DB 접속에 중요한 정보는 아님
	jc::String hostName_; // "127.0.0.1, localhost
	_u16 connPort_; // 3306
	jc::String accountId_; // root
	jc::String accountPass_; // 1234
	jc::String schemaName_; // steinsgate
	int connPoolSize_ = 10;
	int maxConnection_ = 5;
	int iocpThreadCount_ = 2;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct NetDBInfo_SQLServer
{

};

//////////////////////////////////////////////////////////////////////////////////////////
struct ServerProcessInfo : SDescBase
{
	jc::String processName_;
	int mainIOCPThreadCount_ = 8;
	int updatePerSecond_ = 10;
	jc::Vector<NetServerInfo> mainServerInfoList_;	// 이 프로세스에서 구동하는 서버 정보

	virtual ~ServerProcessInfo() override = default;
	virtual const NetInterServerInfo& GetInterServerInfo() const { jc_assert(false); return dummyInterServerInfo_; }
	virtual const NetDBInfo_MySQL& GetDBInfo_MySQL() const { jc_assert(false); return dummyInterDBInfo1_; }
	virtual const NetDBInfo_SQLServer& GetDBInfo_SQLServer() const { jc_assert(false); return dummyInterDBInfo2_; }
private:
	inline static NetInterServerInfo dummyInterServerInfo_;
	inline static NetDBInfo_MySQL dummyInterDBInfo1_;
	inline static NetDBInfo_SQLServer dummyInterDBInfo2_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct AuthServerProcessInfo : ServerProcessInfo
{
	NetInterServerInfo interServerInfo_;
	NetDBInfo_MySQL databaseInfo_;

	virtual const NetInterServerInfo& GetInterServerInfo() const override { return interServerInfo_; }
	virtual const NetDBInfo_MySQL& GetDBInfo_MySQL() const override { return databaseInfo_; }
};

//////////////////////////////////////////////////////////////////////////////////////////
struct LobbyServerProcessInfo : ServerProcessInfo
{
	NetInterServerInfo interServerInfo_;

	virtual const NetInterServerInfo& GetInterServerInfo() const override { return interServerInfo_; }
};

//////////////////////////////////////////////////////////////////////////////////////////
struct CenterServerProcessInfo : ServerProcessInfo
{
};

//////////////////////////////////////////////////////////////////////////////////////////
struct GameServerProcessInfo : ServerProcessInfo
{
	NetInterServerInfo interServerInfo_;
	GameServerType_t gameServerType_;
	int channelNumber_ = -1;
	int channelType_ = -1;

	virtual const NetInterServerInfo& GetInterServerInfo() const override { return interServerInfo_; }
};

//////////////////////////////////////////////////////////////////////////////////////////
struct QueryServerProcessInfo : ServerProcessInfo
{
	NetInterServerInfo interServerInfo_;
	NetDBInfo_MySQL databaseInfo_;
	virtual const NetInterServerInfo& GetInterServerInfo() const override { return interServerInfo_; }
	virtual const NetDBInfo_MySQL& GetDBInfo_MySQL() const override { return databaseInfo_; }
};
