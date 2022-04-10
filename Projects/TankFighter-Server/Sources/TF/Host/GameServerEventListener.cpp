/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Host/GameServerEventListener.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Util/Console.h>
#include <TF/Game/World.h>
#include <TF/Game/PlayerPool.h>

#include <JNetwork/Host/TcpSession.h>

#include <TF/Parser/SendFn.h>

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

	// 이게 플레이어가 강종해버리면 이 플레이어 정보를 사용중인 IOCP 쓰레드에서 오류가 날텐데..
	// 이걸 어떻게 처리하면 좋을까?
	// 일단 지금 상태로는 전부 뜯어 고쳐야하기 땜에 지금은 수정하긴 힘들다.

	Room* pRoom = _World->GetRoomByPlayer(player);
	
	if (pRoom && pRoom->RemovePlayer(player)) {
		bool bRemoveSuccess = pRoom->GetChannel()->RemoveRoomIfEmpty(pRoom);

		// 안 비어있어서 방이 삭제안된 경우 남아있는 유저들에게 브로드캐스트
		if (bRemoveSuccess == false) {
			SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
		}
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