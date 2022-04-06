#define _WINSOCKAPI_		// winsock.h 연속 include 방지
							// 아니 근데, 에바아닙니까 ㄹㅇ; 이거 땜에 1시간 동안 손발 덜덜떨렸네

#include <TF/Host/GameServerEventListener.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Util/Console.h>
#include <TF/Game/World.h>
#include <TF/Game/PlayerPool.h>

#include <JNetwork/Host/TcpSession.h>

#define _Database	MysqlDatabase::GetInstance()
#define _PlayerPool PlayerPool::GetInstance()
#define _World		World::GetInstance()

void GameServerEventListener::OnStarted() {
	Console::WriteLine(ConsoleColor::LIGHTGRAY, "게임 서버 시작됨");
}

void GameServerEventListener::OnConnected(JNetwork::TcpSession* connectedSession) {
	Console::WriteLine(ConsoleColor::CYAN, "유저 : %s 접속", connectedSession->GetRemoteEndPoint().ToString().Source());
	connectedSession->SetTag(_PlayerPool->PopPlayer(connectedSession));
}

void GameServerEventListener::OnDisconnected(JNetwork::TcpSession* disconnetedSession) {
	Console::WriteLine(ConsoleColor::LIGHTGRAY, "유저 : %s 접속 종료", disconnetedSession->GetRemoteEndPoint().ToString().Source());
	Player* player = disconnetedSession->GetTag<Player*>();

	if (player == nullptr) {
		return;
	}

	if (_World->RemovePlayer(player)) {
		Console::WriteLine("월드에서 플레이어 정보 안전하게 제거완료");
	}

	_PlayerPool->ReleasePlayer(player);

	disconnetedSession->SetTag(nullptr);
}

void GameServerEventListener::OnSent(JNetwork::TcpSession* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void GameServerEventListener::OnReceived(JNetwork::TcpSession* receiver, JNetwork::ICommand* cmd) {
	if (!m_pParser->RunCommand(receiver->GetTag<Player*>(), cmd)) {
		Console::WriteLine(ConsoleColor::BROWN, "커맨드 파싱 실패(Command ID : %d)", cmd->GetCommand());
	}
}

void GameServerEventListener::OnPaused() {
}

void GameServerEventListener::OnResume() {
}

void GameServerEventListener::OnStopped() {
	Console::WriteLine("서버가 종료되었습니다.");
}