/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 7:47:29 PM
 * =====================
 *
 */


#include "Pch.h"
#include "NetCore.h"

#include <TF/Common/Command.h>

#include <TF/Server/Const.h>
#include <TF/Server/Listener/L_GAME.h>
#include <TF/Server/Recv/R_GAME.h>


USING_NS_JC;
USING_NS_JNET;

NetCore::NetCore()
	: NetGroup("게임 서버")
	, m_bRunning(false)
	, m_pGameServer(nullptr)
{}

NetCore::~NetCore()
{}


void NetCore::Initialize() {
	InitializeBuffer();
	InitializeIOCP();
	InitializeHost();
	InitializeParser();
}

void NetCore::ProcessMainLoop() {
	constexpr int UpdatePerSecond = 10;

	PulserStatistics pulseStat;
	Pulser pulser(1000 / UpdatePerSecond, Pulser::eSliceCycle, &pulseStat);
	TimeSpan elapsed;

	pulser.Start();
	for (;;) {

		// 0x0001: 이전 GetAsyncKeyState() 호출과 현재 GetAsyncKeyState 호출사이에 눌려진적이 있는 경우
		// 0x8001: 현재 GetAsyncKeyState() 호출시점에 키가 눌려져있는 경우
		// Virtual Key: X(0x58)
		if (GetAsyncKeyState(0x58)) { 
			break;
		}

		Update(elapsed);
		elapsed = pulser.Wait();
	}

	Finalize();
}

void NetCore::RunCommand(Session* session, ICommand* cmd) {
	if (!m_Parser.RunCommand(session, cmd)) {
		_LogWarn_("%s(%d) 커맨드 실행실패", Core::CommandNameMap.Get(cmd->GetCommand()), cmd->GetCommand());
	} else {
		_LogDebug_("%s(%d) 커맨드 수신", Core::CommandNameMap.Get(cmd->GetCommand()), cmd->GetCommand());
	}
}

void NetCore::InitializeBuffer() {
	CreateBufferPool({});
}

void NetCore::InitializeIOCP() {
	CreateIocp(4);
	RunIocp();
}

void NetCore::InitializeHost() {
	auto spServer = MakeShared<GameServer>(m_spIOCP, m_spBufferPool);
	spServer->SetEventListener(dbg_new L_GAME);
	spServer->Start(Const::Net::Server::GameBindEndPoint);

	AddHost(0, spServer);

	m_pGameServer = spServer.GetPtr();
}

void NetCore::InitializeParser() {
	m_Parser.AddCommand<CS_Login>(R_GAME::RECV_CS_Login);
	m_Parser.AddCommand<CS_Register>(R_GAME::RECV_CS_Register);
	m_Parser.AddCommand<CS_LoadChannelInfo>(R_GAME::RECV_CS_LoadChannelInfo);
	m_Parser.AddCommand<CS_JoinChannel>(R_GAME::RECV_CS_JoinChannel);
	m_Parser.AddCommand<CS_LeaveChannel>(R_GAME::RECV_CS_LeaveChannel);
	m_Parser.AddCommand<CS_LoadCharacterInfo>(R_GAME::RECV_CS_LoadCharacterInfo);
	m_Parser.AddCommand<CS_CreateCharacter>(R_GAME::RECV_CS_CreateCharacter);
	m_Parser.AddCommand<CS_DeleteCharacter>(R_GAME::RECV_CS_DeleteCharacter);
	m_Parser.AddCommand<CS_SelectCharacterAndJoinLobby>(R_GAME::RECV_CS_SelectCharacterAndJoinLobby);
	m_Parser.AddCommand<CS_LoadLobbyInfo>(R_GAME::RECV_CS_LoadLobbyInfo);
	m_Parser.AddCommand<CS_LeaveLobby>(R_GAME::RECV_CS_LeaveLobby);
	m_Parser.AddCommand<CS_CreateRoom>(R_GAME::RECV_CS_CreateRoom);
	m_Parser.AddCommand<CS_JoinRoom>(R_GAME::RECV_CS_JoinRoom);
	m_Parser.AddCommand<CS_AddFriend>(R_GAME::RECV_CS_AddFriend);
	m_Parser.AddCommand<CS_AddFriendRequest>(R_GAME::RECV_CS_AddFriendRequest);
	m_Parser.AddCommand<CS_DeleteFriend>(R_GAME::RECV_CS_DeleteFriend);
	m_Parser.AddCommand<CS_LoadRoomInfo>(R_GAME::RECV_CS_LoadRoomInfo);
	m_Parser.AddCommand<CS_RoomGameReady>(R_GAME::RECV_CS_RoomGameReady);
	m_Parser.AddCommand<CS_RoomGameStart>(R_GAME::RECV_CS_RoomGameStart);
	m_Parser.AddCommand<CS_RoomLeave>(R_GAME::RECV_CS_RoomLeave);
	m_Parser.AddCommand<CS_BattleFieldLoad>(R_GAME::RECV_CS_BattleFieldLoad);
	m_Parser.AddCommand<CS_BattileFieldTankMove>(R_GAME::RECV_CS_BattileFieldTankMove);
	m_Parser.AddCommand<CS_BattileFieldTankUpdate>(R_GAME::RECV_CS_BattileFieldTankUpdate);
	m_Parser.AddCommand<CS_BattleFieldPlayWaitEnd>(R_GAME::RECV_CS_BattleFieldPlayWaitEnd);
	m_Parser.AddCommand<CS_BattleFieldPlayingEnd>(R_GAME::RECV_CS_BattleFieldPlayingEnd);
	m_Parser.AddCommand<CS_BattleFieldEndWaitEnd>(R_GAME::RECV_CS_BattleFieldEndWaitEnd);
	m_Parser.AddCommand<CS_BattleFieldLeave>(R_GAME::RECV_CS_BattleFieldLeave);
	m_Parser.AddCommand<CS_ChatMessage>(R_GAME::RECV_CS_ChatMessage);
	m_Parser.AddCommand<CS_BattleFieldFire>(R_GAME::RECV_CS_BattleFieldFire);
	m_Parser.AddCommand<CS_BattleFieldDeath>(R_GAME::RECV_CS_BattleFieldDeath);
	m_Parser.AddCommand<CS_BattleFieldRevival>(R_GAME::RECV_CS_BattleFieldRevival);
	m_Parser.AddCommand<CS_BattleFieldStatisticsUpdate>(R_GAME::RECV_CS_BattleFieldStatisticsUpdate);
	m_Parser.AddCommand<CS_TcpRTT>(R_GAME::RECV_CS_TcpRTT);
}


void NetCore::Update(const TimeSpan& elpased) {
}

