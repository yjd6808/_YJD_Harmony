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

void NetCore::Finalize() {
	m_spGameClient = nullptr;	// 레퍼런스 정리

	NetGroup::Finalize();
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
		S_GAME::SetMemoryPool(m_spBufferPool);	// 세션 버퍼 크기가 커맨드크기보다 훨씬 크기때문에 경합이 없으므로 같이써도 ㄱㅊ
	}
}

void NetCore::initializeParser() {
	m_Parser.AddCommand<SC_Login>(R_GAME::RECV_SC_Login);
	m_Parser.AddCommand<SC_Register>(R_GAME::RECV_SC_Register);
	m_Parser.AddCommand<SC_Logout>(R_GAME::RECV_SC_Logout);
	m_Parser.AddCommand<SC_Disconnect>(R_GAME::RECV_SC_Login);
	m_Parser.AddCommand<SC_LoadChannelInfo>(R_GAME::RECV_SC_LoadChannelInfo);
	m_Parser.AddCommand<SC_JoinChannel>(R_GAME::RECV_SC_JoinChannel);
	m_Parser.AddCommand<SC_LeaveChannel>(R_GAME::RECV_SC_LeaveChannel);
	m_Parser.AddCommand<SC_LoadCharacterInfo>(R_GAME::RECV_SC_LoadCharacterInfo);
	m_Parser.AddCommand<SC_LeaveLobby>(R_GAME::RECV_SC_LeaveLobby);
	m_Parser.AddCommand<SC_CreateRoom>(R_GAME::RECV_SC_CreateRoom);
	m_Parser.AddCommand<SC_SelectCharacterAndJoinLobby>(R_GAME::RECV_SC_SelectCharacter);
	m_Parser.AddCommand<SC_UpdateCharacterInfo>(R_GAME::RECV_SC_UpdateCharacterInfo);
	m_Parser.AddCommand<SC_UpdateRoomList>(R_GAME::RECV_SC_UpdateRoomList);
	m_Parser.AddCommand<SC_UpdateFriendList>(R_GAME::RECV_SC_UpdateFriendList);
	m_Parser.AddCommand<SC_JoinRoom>(R_GAME::RECV_SC_JoinRoom);
	m_Parser.AddCommand<SC_LoadRoomInfo>(R_GAME::RECV_SC_LoadRoomInfo);
	m_Parser.AddCommand<SC_UpdatePlayerList>(R_GAME::RECV_SC_UpdatePlayerList);
	m_Parser.AddCommand<SC_AddFriendRequest>(R_GAME::RECV_SC_AddFriendRequest);
	m_Parser.AddCommand<SC_UpdateRoomMemberList>(R_GAME::RECV_SC_UpdateRoomMemberList);
	m_Parser.AddCommand<SC_UpdateRoomMember>(R_GAME::RECV_SC_UpdateRoomMember);
	m_Parser.AddCommand<SC_RoomGameStart>(R_GAME::RECV_SC_RoomGameStart);
	m_Parser.AddCommand<SC_RoomGameReady>(R_GAME::RECV_SC_RoomGameReady);
	m_Parser.AddCommand<SC_RoomGameIntrude>(R_GAME::RECV_SC_RoomGameIntrude);
	m_Parser.AddCommand<SC_RoomLeave>(R_GAME::RECV_SC_RoomLeave);
	m_Parser.AddCommand<SC_RoomGameEnd>(R_GAME::RECV_SC_RoomGameEnd);
	m_Parser.AddCommand<SC_RoomGameJudge>(R_GAME::RECV_SC_RoomGameJudge);
	m_Parser.AddCommand<SC_ServerMessage>(R_GAME::RECV_SC_ServerMessage);
	m_Parser.AddCommand<SC_ChatMessage>(R_GAME::RECV_SC_ChatMessage);
	m_Parser.AddCommand<SC_BattleFieldTimeSync>(R_GAME::RECV_SC_BattleFieldTimeSync);
	m_Parser.AddCommand<SC_BattleFieldTankSpawn>(R_GAME::RECV_SC_BattleFieldTankSpawn);
	m_Parser.AddCommand<SC_BattleFieldTankList>(R_GAME::RECV_SC_BattleFieldTankList);
	m_Parser.AddCommand<SC_BattleFieldMove>(R_GAME::RECV_SC_BattleFieldMove);
	m_Parser.AddCommand<SC_BattleFieldFire>(R_GAME::RECV_SC_BattleFieldFire);
	m_Parser.AddCommand<SC_BattleFieldDeath>(R_GAME::RECV_SC_BattleFieldDeath);
	m_Parser.AddCommand<SC_BattleFieldStateChanged>(R_GAME::RECV_SC_BattleFieldStateChanged);
	m_Parser.AddCommand<SC_BattleFieldStatisticsUpdate>(R_GAME::RECV_SC_BattleFieldStatisticsUpdate);
	m_Parser.AddCommand<SC_BattleFieldLeave>(R_GAME::RECV_SC_BattleFieldLeave);
	m_Parser.AddCommand<SC_TcpRTT>(R_GAME::RECV_SC_TcpRTT);
	
}

void NetCore::update(const TimeSpan& elapsed) {
	processCommands();
}

void NetCore::runCommand(Session* session, ICommand* cmd) {
	if (m_Parser.RunCommand(session, cmd)) {
		return;
	}

	Cmd_t uiCmd = cmd->GetCommand();
	if (Core::FilteredCommandSet.Exist(uiCmd)) return;
	_LogInfo_("%s 커맨드(%d) 실행실패", Core::CommandNameMap.Get(uiCmd), uiCmd);
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
