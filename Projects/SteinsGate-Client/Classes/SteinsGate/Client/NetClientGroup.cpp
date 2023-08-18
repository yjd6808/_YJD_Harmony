/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientGroup.h"

#include <SteinsGate/Common/Cmd_AUTH.h>
#include <SteinsGate/Common/Cmd_CHAT.h>

#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/S_LOBBY.h>
#include <SteinsGate/Client/S_GAME.h>
#include <SteinsGate/Client/S_GAME_UDP.h>
#include <SteinsGate/Client/S_CHAT.h>
#include <SteinsGate/Client/S_AREA.h>

#include <SteinsGate/Client/R_AUTH.h>

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
{}

NetClientGroup::~NetClientGroup() {
	NetClientGroup::Finalize();
}

void NetClientGroup::Initialize() {

	CreateIocp(4);
	CreateBufferPool({});
	RunIocp();

	const auto spAuthTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, AuthRecvBufferSize_v, AuthSendBufferSize_v);
	const auto spLobbyTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, LobbyRecvBufferSize_v, LobbySendBufferSize_v);

	AddHost(spAuthTcp);
	AddHost(spLobbyTcp);

	AuthTcp = spAuthTcp.Get<TcpClient*>();
	AuthTcp->SetEventListener(dbg_new NetClientEventListener{ClientConnectServerType::Auth});

	LobbyTcp = spLobbyTcp.Get<TcpClient*>();
	LobbyTcp->SetEventListener(dbg_new NetClientEventListener{ClientConnectServerType::Lobby});


	// ==========================================================
	// 샌더 초기화
	// ==========================================================

	S_AUTH::SetInformation(AuthTcp, eSendAsync);
	S_LOBBY::SetInformation(LobbyTcp, eSendAsync);
	S_GAME::SetInformation(GameTcp, eSendAsync);
	S_GAME_UDP::SetInformation(GameUdp, eSendToAsync);
	S_CHAT::SetInformation(ChatTcp, eSendAsync);
	S_AREA::SetInformation(AreaTcp, eSendAsync);



	// ==========================================================
	// 커맨드 초기화
	// ==========================================================

	SGCommandParser* parser = nullptr;
	{
		parser = &m_Parser[ClientConnectServerType::Auth];
		parser->AddCommand<AUC_LoginAck>(R_AUTH::RecvLoginAck);
	}

	{
		parser = &m_Parser[ClientConnectServerType::Lobby];

		// TCP


		// UDP
	}

	{
		parser = &m_Parser[ClientConnectServerType::Logic];

		// TCP


		// UDP
	}

	{
		parser = &m_Parser[ClientConnectServerType::Chat];
	}

	{
		parser = &m_Parser[ClientConnectServerType::Area];
	}
}

void NetClientGroup::Finalize() {
	NetGroup::Finalize();
}

SGCommandParser* NetClientGroup::getParser(ClientConnectServerType_t serverType) {
	return &m_Parser[serverType];
}


