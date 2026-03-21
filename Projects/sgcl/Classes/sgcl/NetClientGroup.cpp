/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "NetClientGroup.h"

#include "sg/Cmd.h"

#include "sgcl/S_AUTH.h"
#include "sgcl/S_LOBBY.h"
#include "sgcl/S_GAME.h"
#include "sgcl/S_GAME_UDP.h"
#include "sgcl/S_CHAT.h"
#include "sgcl/S_AREA.h"

#include "sgcl/R_AUTHENTICATION.h"
#include "sgcl/R_MESSAGE.h"
#include "sgcl/R_LOBBY.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

static constexpr int AUTH_RECV_BUFFER_SIZE = 2048;
static constexpr int AUTH_SEND_BUFFER_SIZE = 2048;
static constexpr int LOBBY_RECV_BUFFER_SIZE = 6144;
static constexpr int LOBBY_SEND_BUFFER_SIZE = 6144;

//////////////////////////////////////////////////////////////////////////////////////////
NetClientGroup::NetClientGroup()
: pAuthTcp_(nullptr)
, pAuthUdp_(nullptr)
, pLobbyTcp_(nullptr)
, pLobbyUdp_(nullptr)
, pLogicTcp_(nullptr)
, pLogicUdp_(nullptr)
, pAreaTcp_(nullptr)
, pAreaUdp_(nullptr)
, pChatTcp_(nullptr)
, pChatUdp_(nullptr)
, pParser_(dbg_new jnet::CommandParser())
{
	SetName("클라");
}

//////////////////////////////////////////////////////////////////////////////////////////
NetClientGroup::~NetClientGroup()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientGroup::Initialize()
{
	CreateIOCP(4);
	CreateBufferPool({});
	RunIOCP();

	auto pAuthTcp = MakeShared<TcpClient>(pIocp_, pBufferPool_, nullptr, AUTH_RECV_BUFFER_SIZE, AUTH_SEND_BUFFER_SIZE);
	auto pLobbyTcp = MakeShared<TcpClient>(pIocp_, pBufferPool_, nullptr, LOBBY_RECV_BUFFER_SIZE, LOBBY_SEND_BUFFER_SIZE);

	AddHost(Const::Host::AuthTcpId, pAuthTcp);
	AddHost(Const::Host::LobbyTcpId, pLobbyTcp);

	pAuthTcp_ = pAuthTcp.Get<TcpClient*>();
	pAuthTcp_->SetEventListener(dbg_new NetClientListenerImpl{ ServerType::Auth });

	pLobbyTcp_ = pLobbyTcp.Get<TcpClient*>();
	pLobbyTcp_->SetEventListener(dbg_new NetClientListenerImpl{ ServerType::Lobby });

	// ======================================================================================
	// 샌더 초기화
	// ======================================================================================

	S_AUTH::SetInformation(pAuthTcp_, SendStrategy::SendAsync);
	S_LOBBY::SetInformation(pLobbyTcp_, SendStrategy::SendAsync);
	S_GAME::SetInformation(pLogicTcp_, SendStrategy::SendAsync);
	S_GAME_UDP::SetInformation(pLogicUdp_, SendStrategy::SendToAsync);
	S_CHAT::SetInformation(pChatTcp_, SendStrategy::SendAsync);
	S_AREA::SetInformation(pAreaTcp_, SendStrategy::SendAsync);

	// ======================================================================================
	// 커맨드 초기화
	// ======================================================================================

	// AUTHENTICATION
	pParser_->AddCommand<AUC_LoginAck>(R_AUTHENTICATION::RECV_AUC_LoginAck);

	// LOBBY
	pParser_->AddCommand<LOC_JoinLobbyAck>(R_LOBBY::RECV_LOC_JoinLobbyAck);

	// MESSAGE
	pParser_->AddCommand<SC_ClientText>(R_MESSAGE::RECV_SC_ClientText);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetClientGroup::Finalize()
{
	NetGroup::Finalize();
}
