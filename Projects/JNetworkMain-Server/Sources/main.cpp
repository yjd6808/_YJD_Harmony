
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>

#include <JCore/LockGuard.h>
#include <JCore/Time.h>

using namespace JCore;
using namespace JNetwork;

#define CMD_STATIC_MESSAGE	0
#define CMD_DYNAMIC_MESSAGE	1

struct StaticMessage : ICommand
{
	StaticMessage() {
		this->Cmd = CMD_STATIC_MESSAGE;	// ��ɾ� �ڵ尪 (���߽� ���)
		this->CmdLen = sizeof(StaticMessage);
	}

	char Chat[512];
};


struct DynamicMessage : ICommand
{
	DynamicMessage(int len, char* msg) {
		this->Length = len;
		this->Cmd = CMD_DYNAMIC_MESSAGE;

		char* pChat = Chat;
		memcpy_s(pChat, len, msg, len);
		pChat[len] = NULL;
	}

	static int CmdSizeOf(int len) {
		return sizeof(DynamicMessage) + sizeof(char) * len;
	}

	int Length;
	char Chat[0];
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
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE) {
			StaticMessage* pMsg = cmd->CastCommand<StaticMessage*>();
			Winsock::Message("[����] ����ƽ �޽����� �����߽��ϴ�. : %s", pMsg->Chat);

			// ����ƽ ��Ŷ ���� ����
			auto pPacket = new StaticPacket<StaticMessage>();
			StaticMessage* arg1 = pPacket->Get<0>();
			strcpy_s(arg1->Chat, 512, pMsg->Chat);

			if (!receiver->SendAsync(pPacket)) {
				Winsock::Message("[����] ����ƽ ���� ����");
			}
		} else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			int iLen = pDynamicMessage->Length;
			Winsock::Message("[����] ���̳��� �޽����� �����߽��ϴ�. : %s(���� : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);

			// ���̳��� ��Ŷ ���� ����
			auto pPacket = new DynamicPacket<DynamicMessage>(DynamicMessage::CmdSizeOf(iLen + 1));
			pPacket->Construct<0>(iLen, pDynamicMessage->Chat);

			if (!receiver->SendAsync(pPacket)) {
				Winsock::Message("[����] ���̳��� ���� ����");
			}
			
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