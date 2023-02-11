/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:13:58 PM
 * =====================
 *
 */
#include "Research.h"
#include <JNetwork/Research/ServerNetMaster.h>
#include <JNetwork/Research/Command.h>
#include <JNetwork/Research/Config.h>

USING_NS_JC;

NS_JNET_BEGIN

/* =============================================================================
 *
 *                             서버 넷마스터
 *
 * ============================================================================= */

void ServerNetMaster::Initialize() {
	NetGroupPtr g1 = MakeShared<ServerLoginChannelNetGroup>();
	NetGroupPtr g2 = MakeShared<ServerGameNetGroup>();
	NetGroupPtr g3 = MakeShared<ServerUdpNetGroup>();

	AddNetGroup(ServerLoginChannelGroup, g1);
	AddNetGroup(ServerGameGroup, g2);
	AddNetGroup(ServerUdpGroup, g3);

	g1->Initialize();
	g2->Initialize();
	g3->Initialize();
}



/* =============================================================================
 *
 *                             로그인 채널 서버 그룹
 *
 * ============================================================================= */

ServerLoginChannelNetGroup::ServerLoginChannelNetGroup()
	: NetGroup("서버 TCP 로그인,채널 서버 그룹")
	, m_LoginListener("로그인 서버")
	, m_ChannelListener("채널 서버")
{}

ServerLoginChannelNetGroup::~ServerLoginChannelNetGroup() {
	ServerLoginChannelNetGroup::Finalize();
}

void ServerLoginChannelNetGroup::Initialize() {
	CreateIocp(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIocp();

	TcpServerPtr spLoginServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool, &m_LoginListener);
	spLoginServer->Start(ServerLoginTcpAddr);

	TcpServerPtr spChannelServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool, &m_ChannelListener);
	spChannelServer->Start(ServerChannelTcpAddr);
	
	AddHost(spLoginServer);
	AddHost(spChannelServer);

	m_spLoginServer = spLoginServer;
	m_spChannelServer = spChannelServer;
}


/* =============================================================================
 *
 *                              게임 서버 그룹
 *
 * ============================================================================= */


ServerGameNetGroup::ServerGameNetGroup()
	: NetGroup("서버 TCP 게임 서버 그룹")
	, m_GameListener("게임 서버")
{}

ServerGameNetGroup::~ServerGameNetGroup() {
	ServerGameNetGroup::Finalize();
}

void ServerGameNetGroup::Initialize() {
	CreateIocp(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIocp();

	TcpServerPtr spGameServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool, &m_GameListener);
	spGameServer->Start(ServerGameTcpAddr);

	AddHost(spGameServer);

	m_spGameServer = spGameServer;
}

 /* =============================================================================
  *
  *                           UDP 클라이언트 그룹
  *
  * ============================================================================= */


ServerUdpNetGroup::ServerUdpNetGroup()
	: NetGroup("서버 UDP 그룹")
	, m_LoginListener("로그인 UDP")
	, m_ChannelListener("채널 UDP")
	, m_GameListener("게임 UDP")
{}

ServerUdpNetGroup::~ServerUdpNetGroup() {
	ServerUdpNetGroup::Finalize();
}

void ServerUdpNetGroup::Initialize() {
	CreateIocp(4);
	CreateBufferPool({
		{ 8192, 30 }
	});

	RunIocp();

	UdpClientPtr spLoginUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_LoginListener);
	spLoginUdp->Bind(ServerLoginUdpAddr);

	UdpClientPtr spChannelUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_ChannelListener);
	spChannelUdp->Bind(ServerChannelUdpAddr);

	UdpClientPtr spGameUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_GameListener);
	spGameUdp->Bind(ServerGameUdpAddr);

	AddHost(spLoginUdp);
	AddHost(spChannelUdp);
	AddHost(spGameUdp);

	m_spLoginUdpClient = spLoginUdp;
	m_spChannelUdpClient = spChannelUdp;
	m_spGameUdpClient = spGameUdp;
}




NS_JNET_END