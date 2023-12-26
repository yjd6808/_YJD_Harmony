/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientGroup.h"

#include <SteinsGate/Common/Cmd.h>

#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/S_LOBBY.h>
#include <SteinsGate/Client/S_GAME.h>
#include <SteinsGate/Client/S_GAME_UDP.h>
#include <SteinsGate/Client/S_CHAT.h>
#include <SteinsGate/Client/S_AREA.h>

#include <SteinsGate/Client/R_AUTHENTICATION.h>
#include <SteinsGate/Client/R_MESSAGE.h>
#include <SteinsGate/Client/R_LOBBY.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

static constexpr int AuthRecvBufferSize_v = 2048;
static constexpr int AuthSendBufferSize_v = 2048;
static constexpr int LobbyRecvBufferSize_v = 6144;
static constexpr int LobbySendBufferSize_v = 6144;

NetClientGroup::NetClientGroup()
	: AuthTcp(nullptr)
	, AuthUdp(nullptr)
	, LobbyTcp(nullptr)
	, LobbyUdp(nullptr)
	, LogicTcp(nullptr)
	, LogicUdp(nullptr)
	, AreaTcp(nullptr)
	, AreaUdp(nullptr)
	, ChatTcp(nullptr)
	, ChatUdp(nullptr)
{
	SetName("클라");
}

NetClientGroup::~NetClientGroup() {
}

void NetClientGroup::Initialize() {

	CreateIocp(4);
	CreateBufferPool({});
	RunIocp();

	auto spAuthTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, nullptr, AuthRecvBufferSize_v, AuthSendBufferSize_v);
	auto spLobbyTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, nullptr, LobbyRecvBufferSize_v, LobbySendBufferSize_v);

	AddHost(Const::Host::AuthTcpId, spAuthTcp);
	AddHost(Const::Host::LobbyTcpId, spLobbyTcp);

	AuthTcp = spAuthTcp.Get<TcpClient*>();
	AuthTcp->SetEventListener(dbg_new NetClientEventListener{ClientConnectServerType::Auth});

	LobbyTcp = spLobbyTcp.Get<TcpClient*>();
	LobbyTcp->SetEventListener(dbg_new NetClientEventListener{ClientConnectServerType::Lobby});


	// ==========================================================
	// 샌더 초기화
	// ==========================================================

	S_AUTH::SetInformation(AuthTcp, SendStrategy::SendAsync);
	S_LOBBY::SetInformation(LobbyTcp, SendStrategy::SendAsync);
	S_GAME::SetInformation(LogicTcp, SendStrategy::SendAsync);
	S_GAME_UDP::SetInformation(LogicUdp, SendStrategy::SendToAsync);
	S_CHAT::SetInformation(ChatTcp, SendStrategy::SendAsync);
	S_AREA::SetInformation(AreaTcp, SendStrategy::SendAsync);



	// ==========================================================
	// 커맨드 초기화
	// ==========================================================

	// AUTHENTICATION
	m_Parser.AddCommand<AUC_LoginAck>			(R_AUTHENTICATION::RECV_AUC_LoginAck);

	// LOBBY
	m_Parser.AddCommand<LOC_JoinLobbyAck>		(R_LOBBY::RECV_LOC_JoinLobbyAck);

	// MESSAGE
	m_Parser.AddCommand<SC_ClientText>			(R_MESSAGE::RECV_SC_ClientText);
}

void NetClientGroup::Finalize() {
	NetGroup::Finalize();
}


