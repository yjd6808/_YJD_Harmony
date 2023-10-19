/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 10:41:49 AM
 * =====================
 *
 */

#include "Pch.h"
#include "NetCore.h"

#include <TF/Common/Command.h>

#include <TF/Client/Net/Listener/L_GAME.h>
#include <TF/Client/Net/Recv/R_GAME.h>
#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_JC;
USING_NS_JNET;

NetCore::NetCore()
	: m_pListenerGame(nullptr)
	, m_pCommandQueue(dbg_new CommandQueue) {
	SetName("클라이언트");
}
NetCore::~NetCore() {
	clearCommandQueue();
	JCORE_DELETE_SAFE(m_pCommandQueue);
}


void NetCore::Initialize() {
	initializeBuffer();
	initializeIOCP();
	initializeHost();
	initializeParser();
}

void NetCore::initializeBuffer() {
	CreateBufferPool({});
}

void NetCore::initializeIOCP() {
	CreateIocp(1);
	RunIocp();
}

void NetCore::initializeHost() {
	m_spGameClient = MakeShared<GameClient>(m_spIOCP, m_spBufferPool);
	m_spGameClient->SetEventListener(m_pListenerGame = dbg_new L_GAME);
	
	AddHost(0, m_spGameClient);

	if (m_spGameClient->ConnectAsync(IPv4EndPoint::Parse(Const::Net::Server::GameConnectEndPoint))) {
		S_GAME::SetInformation(m_spGameClient.GetPtr(), SendStrategy::SendAsync);
	}
}

void NetCore::initializeParser() {
	m_Parser.AddCommand<SC_Login>(R_GAME::RECV_SC_Login);
	m_Parser.AddCommand<SC_Register>(R_GAME::RECV_SC_Register);
	m_Parser.AddCommand<SC_LoadChannelInfo>(R_GAME::RECV_SC_LoadChannelInfo);
	m_Parser.AddCommand<SC_SelectChannel>(R_GAME::RECV_SC_SelectChannel);
	m_Parser.AddCommand<SC_LeaveChannel>(R_GAME::RECV_SC_LeaveChannel);
	m_Parser.AddCommand<SC_LoadCharacterInfo>(R_GAME::RECV_SC_LoadCharacterInfo);
	m_Parser.AddCommand<SC_LeaveLobby>(R_GAME::RECV_SC_LeaveLobby);
	m_Parser.AddCommand<SC_CreateRoom>(R_GAME::RECV_SC_CreateRoom);
	m_Parser.AddCommand<SC_SelectCharacter>(R_GAME::RECV_SC_SelectCharacter);
	m_Parser.AddCommand<SC_UpdateCharacterInfo>(R_GAME::RECV_SC_UpdateCharacterInfo);
	m_Parser.AddCommand<SC_UpdateRoomList>(R_GAME::RECV_SC_UpdateRoomList);
	m_Parser.AddCommand<SC_UpdateFriendList>(R_GAME::RECV_SC_UpdateFriendList);
	m_Parser.AddCommand<SC_JoinRoom>(R_GAME::RECV_SC_JoinRoom);
	m_Parser.AddCommand<SC_LoadRoomInfo>(R_GAME::RECV_SC_LoadRoomInfo);
	m_Parser.AddCommand<SC_UpdatePlayerList>(R_GAME::RECV_SC_UpdatePlayerList);
	m_Parser.AddCommand<SC_AddFriendRequest>(R_GAME::RECV_SC_AddFriendRequest);
	m_Parser.AddCommand<SC_UpdateRoomMemberList>(R_GAME::RECV_SC_UpdateRoomMemberList);
	m_Parser.AddCommand<SC_BattleFieldLoad>(R_GAME::RECV_SC_BattleFieldLoad);
	m_Parser.AddCommand<SC_BattleFieldLeave>(R_GAME::RECV_SC_BattleFieldLeave);
	m_Parser.AddCommand<SC_ServerMessage>(R_GAME::RECV_SC_ServerMessage);
	m_Parser.AddCommand<SC_ChatMessage>(R_GAME::RECV_SC_ChatMessage);
	m_Parser.AddCommand<SC_BattleFieldFire>(R_GAME::RECV_SC_BattleFieldFire);
	m_Parser.AddCommand<SC_BattleFieldDeath>(R_GAME::RECV_SC_BattleFieldDeath);
	// m_Parser.AddCommand<SC_BattleFieldRevival>(R_GAME::RECV_SC_BattleFieldRevival);
	// m_Parser.AddCommand<SC_BattleFieldStatisticsUpdate>(R_GAME::RECV_SC_BattleFieldStatisticsUpdate);
	m_Parser.AddCommand<SC_TcpRTT>(R_GAME::RECV_SC_TcpRTT);
	
}

void NetCore::update(const TimeSpan& elpased) {
	processCommands();
}

void NetCore::runCommand(Session* session, ICommand* cmd) {
	if (!m_Parser.RunCommand(session, cmd)) {
		_LogInfo_("%s 커맨드(%d) 실행실패", Core::CommandNameMap.Get(cmd->GetCommand()), cmd->GetCommand());
	}
}

void NetCore::clearCommandQueue() {
	while (!m_pCommandQueue->IsEmpty()) {
		ICommand* pCmd = m_pCommandQueue->Front();
		m_pCommandQueue->Dequeue();
		delete[](char*)pCmd;
	}
}


void NetCore::processCommands() {

	if (m_pListenerGame == nullptr || m_spGameClient == nullptr) {
		return;
	}

	m_pListenerGame->SwapCommandQueue(&m_pCommandQueue);

	while (!m_pCommandQueue->IsEmpty()) {
		ICommand* pCmd = m_pCommandQueue->Front();
		runCommand(m_spGameClient.GetPtr(), pCmd);
		m_pCommandQueue->Dequeue();
		delete[](char*)pCmd;
	}
}
