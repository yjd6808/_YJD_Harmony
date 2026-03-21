/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:13:58 PM
 * =====================
 *
 */
#include "jnetr_common/ServerNetGroupMgr.h"
#include "jnetr_common/Command.h"
#include "jnetr_common/Config.h"

USING_NS_JC;

NS_JNET_BEGIN
/* =============================================================================
 *
 *                             서버 넷마스터
 *
 * ============================================================================= */

//////////////////////////////////////////////////////////////////////////////////////////
bool ServerNetGroupMgr::Initialize()
{
	NetGroupPtr pLoginChannelNetGroup = MakeShared<ServerLoginChannelNetGroup>();
	NetGroupPtr pGameNetGroup = MakeShared<ServerGameNetGroup>();
	NetGroupPtr pUdpNetGroup = MakeShared<ServerUdpNetGroup>();

	AddNetGroup(SERVER_LOGIN_CHANNEL_GROUP, pLoginChannelNetGroup);
	AddNetGroup(SERVER_GAME_GROUP, pGameNetGroup);
	AddNetGroup(SERVER_UDP_GROUP, pUdpNetGroup);

	pLoginChannelNetGroup->Initialize();
	pGameNetGroup->Initialize();
	pUdpNetGroup->Initialize();
	return true;
}


/* =============================================================================
 *
 *                             로그인 채널 서버 그룹
 *
 * ============================================================================= */

//////////////////////////////////////////////////////////////////////////////////////////
ServerLoginChannelNetGroup::ServerLoginChannelNetGroup()
: NetGroup("서버 TCP 로그인,채널 서버 그룹")
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerLoginChannelNetGroup::~ServerLoginChannelNetGroup()
{
	ServerLoginChannelNetGroup::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerLoginChannelNetGroup::Initialize()
{
	CreateIOCP(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIOCP();

	TcpServerPtr pLoginServer = MakeShared<TcpServer>(pIocp_, pBufferPool_);
	pLoginServer->Start(IPv4EndPoint{ SERVER_LOGIN_TCP_ADDR });

	TcpServerPtr pChannelServer = MakeShared<TcpServer>(pIocp_, pBufferPool_);
	pChannelServer->Start(IPv4EndPoint{ SERVER_CHANNEL_TCP_ADDR });

	AddHost(1, pLoginServer);
	AddHost(2, pChannelServer);

	loginServer_ = pLoginServer;
	loginServer_->SetEventListener(dbg_new ServerListener{ "로그인 서버" });
	channelServer_ = pChannelServer;
	channelServer_->SetEventListener(dbg_new ServerListener{ "채널 서버" });
}


/* =============================================================================
 *
 *                              게임 서버 그룹
 *
 * ============================================================================= */

//////////////////////////////////////////////////////////////////////////////////////////
ServerGameNetGroup::ServerGameNetGroup()
: NetGroup("서버 TCP 게임 서버 그룹")
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerGameNetGroup::~ServerGameNetGroup()
{
	ServerGameNetGroup::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerGameNetGroup::Initialize()
{
	CreateIOCP(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIOCP();

	TcpServerPtr pGameServer = MakeShared<TcpServer>(pIocp_, pBufferPool_);
	pGameServer->Start(IPv4EndPoint{ SERVER_GAME_TCP_ADDR });

	AddHost(1, pGameServer);

	gameServer_ = pGameServer;
	gameServer_->SetEventListener(dbg_new ServerListener{ "게임 서버 " });
}

/* =============================================================================
 *
 *                           UDP 클라이언트 그룹
 *
 * ============================================================================= */

//////////////////////////////////////////////////////////////////////////////////////////
ServerUdpNetGroup::ServerUdpNetGroup()
: NetGroup("서버 UDP 그룹")
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerUdpNetGroup::~ServerUdpNetGroup()
{
	ServerUdpNetGroup::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerUdpNetGroup::Initialize()
{
	CreateIOCP(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIOCP();

	UdpClientPtr pLoginUdp = MakeShared<UdpClient>(pIocp_, pBufferPool_);
	pLoginUdp->Bind(IPv4EndPoint{ SERVER_LOGIN_UDP_ADDR });
	pLoginUdp->RecvFromAsync();

	UdpClientPtr pChannelUdp = MakeShared<UdpClient>(pIocp_, pBufferPool_);
	pChannelUdp->Bind(IPv4EndPoint{ SERVER_CHANNEL_UDP_ADDR });
	pChannelUdp->RecvFromAsync();

	UdpClientPtr pGameUdp = MakeShared<UdpClient>(pIocp_, pBufferPool_);
	pGameUdp->Bind(IPv4EndPoint{ SERVER_GAME_UDP_ADDR });
	pGameUdp->RecvFromAsync();

	AddHost(1, pLoginUdp);
	AddHost(2, pChannelUdp);
	AddHost(3, pGameUdp);

	loginUdpClient_ = pLoginUdp;
	loginUdpClient_->SetEventListener(dbg_new ClientListener{ "로그인 UDP" });
	channelUdpClient_ = pChannelUdp;
	channelUdpClient_->SetEventListener(dbg_new ClientListener{ "채널 UDP" });
	gameUdpClient_ = pGameUdp;
	gameUdpClient_->SetEventListener(dbg_new ClientListener{ "게임 UDP" });
}


NS_END
