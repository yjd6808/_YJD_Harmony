/*
 * �ۼ��� : ������
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
	Console::WriteLine(ConsoleColor::LIGHTGRAY, "���� ���� ���۵�");
}

void GameServerEventListener::OnConnected(JNetwork::TcpSession* connectedSession) {
	Console::WriteLine(ConsoleColor::CYAN, "���� : %s ����", connectedSession->GetRemoteEndPoint().ToString().Source());
	connectedSession->SetTag(_PlayerPool->PopPlayer(connectedSession));
}

void GameServerEventListener::OnDisconnected(JNetwork::TcpSession* disconnetedSession) {
	Console::WriteLine(ConsoleColor::LIGHTGRAY, "���� : %s ���� ����", disconnetedSession->GetRemoteEndPoint().ToString().Source());
	Player* player = disconnetedSession->GetTag<Player*>();

	if (player == nullptr) {
		return;
	}

	// �̰� �÷��̾ �����ع����� �� �÷��̾� ������ ������� IOCP �����忡�� ������ ���ٵ�..
	// �̰� ��� ó���ϸ� ������?
	// �ϴ� ���� ���·δ� ���� ��� ���ľ��ϱ� ���� ������ �����ϱ� �����.

	Room* pRoom = _World->GetRoomByPlayer(player);
	
	if (pRoom && pRoom->RemovePlayer(player)) {
		bool bRemoveSuccess = pRoom->GetChannel()->RemoveRoomIfEmpty(pRoom);

		// �� ����־ ���� �����ȵ� ��� �����ִ� �����鿡�� ��ε�ĳ��Ʈ
		if (bRemoveSuccess == false) {
			SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
		}
	}

	if (_World->RemovePlayer(player)) {
		Console::WriteLine("���忡�� �÷��̾� ���� �����ϰ� ���ſϷ�");
	}

	_PlayerPool->ReleasePlayer(player);
	disconnetedSession->SetTag(nullptr);
}

void GameServerEventListener::OnSent(JNetwork::TcpSession* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void GameServerEventListener::OnReceived(JNetwork::TcpSession* receiver, JNetwork::ICommand* cmd) {
	if (!m_pParser->RunCommand(receiver->GetTag<Player*>(), cmd)) {
		Console::WriteLine(ConsoleColor::BROWN, "Ŀ�ǵ� �Ľ� ����(Command ID : %d)", cmd->GetCommand());
	}
}

void GameServerEventListener::OnPaused() {
}

void GameServerEventListener::OnResume() {
}

void GameServerEventListener::OnStopped() {
	Console::WriteLine("������ ����Ǿ����ϴ�.");
}