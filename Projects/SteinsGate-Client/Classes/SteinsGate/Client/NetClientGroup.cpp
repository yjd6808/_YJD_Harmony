/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientGroup.h"

#include <SteinsGate/Common/NetAuthCmd.h>
#include <SteinsGate/Common/NetLobbyCmd.h>
#include <SteinsGate/Common/NetGameCmd.h>

#include <SteinsGate/Client/R_AUTH.h>

#include <SteinsGate/Client/NetAuthEventListener.h>
#include <SteinsGate/Client/NetLobbyEventListener.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

static constexpr int AuthRecvBufferSize_v = 2048;
static constexpr int AuthSendBufferSize_v = 2048;
static constexpr int LobbyRecvBufferSize_v = 6144;
static constexpr int LobbySendBufferSize_v = 6144;

NetClientGroup::NetClientGroup()
	: AuthTcp(nullptr)
    , LobbyTcp(nullptr)
    , GameTcp(nullptr)
    , GameUdp(nullptr)
    , AreaTcp(nullptr)
    , ChatTcp(nullptr)
	, m_vListeners(8)
{}

NetClientGroup::~NetClientGroup() {
	m_vListeners.ForEach([](auto listener) { delete listener; });
}

void NetClientGroup::Initialize() {

	CreateIocp(4);
	CreateBufferPool({});
	RunIocp();

	NetClientEventListener* pAuthListener = dbg_new NetAuthEventListener();
	NetClientEventListener* pLobbyListener = dbg_new NetLobbyEventListener();
	NetClientEventListener* pGameListener;
	NetClientEventListener* pChatListener;
	NetClientEventListener* pAreaListener;

	m_vListeners.PushBack(pAuthListener);
	m_vListeners.PushBack(pLobbyListener);

	const auto spAuthTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, pAuthListener, AuthRecvBufferSize_v, AuthSendBufferSize_v);
	const auto spLobbyTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, pLobbyListener, LobbyRecvBufferSize_v, LobbySendBufferSize_v);

	AddHost(spAuthTcp);
	AddHost(spLobbyTcp);

	AuthTcp = spAuthTcp.Get<TcpClient*>();
	LobbyTcp = spLobbyTcp.Get<TcpClient*>();


	// ==========================================================
	//
	// ==========================================================

	m_Parser.AddCommand(CmdLoginAck_SC, R_AUTH::RecvLoginAck);
}

