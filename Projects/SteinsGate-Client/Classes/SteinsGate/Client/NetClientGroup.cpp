/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientGroup.h"

#include <SteinsGate/Common/AuthCmd.h>
#include <SteinsGate/Common/LobbyCmd.h>
#include <SteinsGate/Common/GameCmd.h>

#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/S_LOBBY.h>
#include <SteinsGate/Client/S_GAME.h>
#include <SteinsGate/Client/S_GAME_UDP.h>
#include <SteinsGate/Client/S_CHAT.h>
#include <SteinsGate/Client/S_AREA.h>

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
{}

NetClientGroup::~NetClientGroup() {
	NetClientGroup::Finalize();
}

void NetClientGroup::Initialize() {

	CreateIocp(4);
	CreateBufferPool({});
	RunIocp();

	NetClientEventListener* pAuthListener = dbg_new NetAuthEventListener();
	NetClientEventListener* pLobbyListener = dbg_new NetLobbyEventListener();
	NetClientEventListener* pGameListener = nullptr;
	NetClientEventListener* pChatListener = nullptr;
	NetClientEventListener* pAreaListener = nullptr;

	m_Listeners[ClientConnectServerType::Auth] = pAuthListener;
	m_Listeners[ClientConnectServerType::Lobby] = pLobbyListener;
	m_Listeners[ClientConnectServerType::Game] = pGameListener;
	m_Listeners[ClientConnectServerType::Chat] = pChatListener;
	m_Listeners[ClientConnectServerType::Area] = pAreaListener;

	const auto spAuthTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, pAuthListener, AuthRecvBufferSize_v, AuthSendBufferSize_v);
	const auto spLobbyTcp = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, pLobbyListener, LobbyRecvBufferSize_v, LobbySendBufferSize_v);

	AddHost(spAuthTcp);
	AddHost(spLobbyTcp);

	AuthTcp = spAuthTcp.Get<TcpClient*>();
	LobbyTcp = spLobbyTcp.Get<TcpClient*>();


	// ==========================================================
	// 샌더 초기화
	// ==========================================================

	S_AUTH::SetInformation(AuthTcp, eSendAsync);
	S_LOBBY::SetInformation(LobbyTcp, eSendAsync);
	S_GAME::SetInformation(GameTcp, eSendAsync);
	S_GAME_UDP::SetInformation(GameUdp, eSendToAsync, CoreServerProcessInfo_v->Center.RemoteCenter);
	S_CHAT::SetInformation(ChatTcp, eSendAsync);
	S_AREA::SetInformation(AreaTcp, eSendAsync);



	// ==========================================================
	// 커맨드 초기화
	// ==========================================================

	SGCommandParser* parser = nullptr;
	{
		parser = &m_Parser[ClientConnectServerType::Auth];
		parser->AddCommand(CmdLoginAck_SC, R_AUTH::RecvLoginAck);
	}

	{
		parser = &m_Parser[ClientConnectServerType::Lobby];

		// TCP


		// UDP
	}

	{
		parser = &m_Parser[ClientConnectServerType::Game];

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

	// 리스너는 호스트에 의존성이있으므로.. 호스트들을 먼저 제거후 수행해줘야한다.
	for (int i = 0; i < ClientConnectServerType::Max; ++i) {
		DeleteSafe(m_Listeners[i]);
	}
}

NetClientEventListener* NetClientGroup::getListener(ClientConnectServerType_t serverType) {
	return m_Listeners[serverType];
}

SGCommandParser* NetClientGroup::getParser(ClientConnectServerType_t serverType) {
	return &m_Parser[serverType];
}


