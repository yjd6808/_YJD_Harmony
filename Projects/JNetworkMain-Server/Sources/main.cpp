
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>

#define _WINSOCKAPI_

#include <JCore/LockGuard.h>

using namespace JCore;
using namespace JNetwork;

struct Message : ICommand
{
	Message() {
		this->Cmd = 0;	// ��ɾ� �ڵ尪 (���߽� ���)
		this->CmdLen = sizeof(Message);
	}

	char Chat[512];
};

class MyServerEventListener : public TcpServerEventListener
{
protected:
	void OnStarted() override {
		Winsock::Message("[����] ������ ���۵Ǿ����ϴ�.");
	}

	void OnConnected(TcpSession* connectedSession) override { 
		Winsock::Message("[����] %s Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.", 
			connectedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnDisconnected(TcpSession* disconnetedSession) override {
		Winsock::Message("[����] %s Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.",
			disconnetedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnSent(TcpSession* sender, ISendPacket* packet, Int32UL sentBytes) override {
		Winsock::Message("[����] �۽� : %d ����Ʈ", packet->GetPacketLength());
	}

	void OnReceived(TcpSession* receiver, ICommand* cmd) override {
		// ������ �޽��� ���
		Message* pMsg = cmd->CastCommand<Message*>();
		Winsock::Message("[����] �޽��� ���� : %s", pMsg->Chat);

		// ���� ����
		auto pPacket = new Packet<Message>();
		Message* arg1 = pPacket->Get<0>();
		strcpy_s(arg1->Chat, 512, pMsg->Chat);

		if (!receiver->SendAsync(pPacket)) {
			Winsock::Message("[����] ���� ����");
		}
	}

	void OnPaused() override {
		Winsock::Message("[����] �Ͻ������Ǿ����ϴ�.");
	}

	void OnResume() override {
		Winsock::Message("[����] �ٽ� ���� �Ǿ����ϴ�.");
	}

	void OnStopped() override {
		Winsock::Message("[����] ����Ǿ����ϴ�.");
	}
};



int main() {
	CriticalSectionMutex mtx;

	Winsock::SetMutex(&mtx);
	Winsock::Initialize(2, 2);

	TcpServer server;
	MyServerEventListener myServerEventListener;

	// ���� �̺�Ʈ ������ ���
	server.SetEventListener(&myServerEventListener);

	// ���� ����
	if (server.Start("0.0.0.0:9999")) {
		server.Pause();		// ���� �Ͻ����� ���
		server.Resume();	// ���� ��� ����
	}

	// c �Է½� ���� ����
	while (getchar() != 'c') {}
	server.Stop();
	
	Sleep(5000000);		// �񵿱�� ���۵Ǳ� ������ main �Լ� ���Ḧ ����

	Winsock::Finalize();
	return 0;
}