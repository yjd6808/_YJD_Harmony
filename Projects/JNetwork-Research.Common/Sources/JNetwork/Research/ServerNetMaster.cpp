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

	TcpServerPtr spLoginServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool);
	spLoginServer->Start(ServerLoginTcpAddr);

	TcpServerPtr spChannelServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool);
	spChannelServer->Start(ServerChannelTcpAddr);
	
	AddHost(1, spLoginServer);
	AddHost(2, spChannelServer);

	m_spLoginServer = spLoginServer;
	m_spLoginServer->SetEventListener(dbg_new ServerListener{ "로그인 서버" });
	m_spChannelServer = spChannelServer;
	m_spChannelServer->SetEventListener(dbg_new ServerListener{ "채널 서버" });
}


/* =============================================================================
 *
 *                              게임 서버 그룹
 *
 * ============================================================================= */


ServerGameNetGroup::ServerGameNetGroup()
	: NetGroup("서버 TCP 게임 서버 그룹")
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

	TcpServerPtr spGameServer = MakeShared<TcpServer>(m_spIOCP, m_spBufferPool);
	spGameServer->Start(ServerGameTcpAddr);

	AddHost(1, spGameServer);

	m_spGameServer = spGameServer;
	m_spGameServer->SetEventListener(dbg_new ServerListener{ "게임 서버 " });
}

 /* =============================================================================
  *
  *                           UDP 클라이언트 그룹
  *
  * ============================================================================= */


ServerUdpNetGroup::ServerUdpNetGroup()
	: NetGroup("서버 UDP 그룹")
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

	UdpClientPtr spLoginUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool);
	spLoginUdp->Bind(ServerLoginUdpAddr);
	spLoginUdp->RecvFromAsync();

	UdpClientPtr spChannelUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool);
	spChannelUdp->Bind(ServerChannelUdpAddr);
	spChannelUdp->RecvFromAsync();

	UdpClientPtr spGameUdp = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool);
	spGameUdp->Bind(ServerGameUdpAddr);
	spGameUdp->RecvFromAsync();

	AddHost(1, spLoginUdp);
	AddHost(2, spChannelUdp);
	AddHost(3, spGameUdp);

	m_spLoginUdpClient = spLoginUdp;
	m_spLoginUdpClient->SetEventListener(dbg_new ClientListener{ "로그인 UDP" });
	m_spChannelUdpClient = spChannelUdp;
	m_spChannelUdpClient->SetEventListener(dbg_new ClientListener{ "채널 UDP" });
	m_spGameUdpClient = spGameUdp;
	m_spGameUdpClient->SetEventListener(dbg_new ClientListener{ "게임 UDP" });
}




NS_JNET_END