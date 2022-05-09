#define _WINSOCKAPI_			// Winsock.h include ����

#include <JCore/LockGuard.h>

#include <JNetwork/Network.h>	// 1. JCore.lib ��ũ
								// 2. Mswsock.lib ��ũ
								// 3. ws2_32.lib ��ũ

#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpClient.h>

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

		char* g = Chat;
		memcpy_s(g, len, msg, len);
		Chat[len] = NULL;
	}

	static int CmdSizeOf(int len) {
		return sizeof(DynamicMessage) + sizeof(char) * len;
	}

	int Length;
	char Chat[0];
};



class MyClientEventListener : public TcpClientEventListener
{
protected:
	virtual void OnConnected() {
		Winsock::Message("[Ŭ��] ������ ����Ǿ����ϴ�.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("[Ŭ��] ������ ����Ǿ����ϴ�.");
	}

	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
		Winsock::Message("[Ŭ��] Ŀ�ǵ� %d���� �۽��߽��ϴ�.", sentPacket->GetCommandCount());
	}

	virtual void OnReceived(ICommand* cmd) {
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE)
			Winsock::Message("[Ŭ��] ����ƽ �޽����� �����߽��ϴ�. : %s", cmd->CastCommand<StaticMessage*>()->Chat);
		else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			Winsock::Message("[Ŭ��] ���̳��� �޽����� �����߽��ϴ�. : %s(���� : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);
		}
	}
};


int main() {
	CriticalSectionMutex mtx;


	// ��Ƽ ��Ŷ ����
	auto pPakcet = new StaticPacket<Command<int>, StaticMessage, Command<float>>();
	Command<int>* arg1		= pPakcet->Get<0>();
	StaticMessage* arg2		= pPakcet->Get<1>();
	Command<float>* arg3	= pPakcet->Get<2>();

	arg1->Value = 30;
	strcpy_s(arg2->Chat, 512, "�ȳ��ϼ���");
	arg3->Value = 40;



	Winsock::SetMutex(&mtx);
	Winsock::Initialize(2, 2);

	TcpClient client;
	MyClientEventListener myClientEventListener;
	client.SetEventListener(&myClientEventListener);


	if (client.ConnectAsync("127.0.0.1:9999")) {
		while (1) {


			auto pStaticPacket = new StaticPacket<StaticMessage>();
			StaticMessage* arg1 = pStaticPacket->Get<0>();
			std::cin >> arg1->Chat;

			int iLen = StringUtil::Length(arg1->Chat);

			// Dynamic ��Ŷ ����
			auto pDynamicPacket = new DynamicPacket<DynamicMessage, DynamicMessage>(
				DynamicMessage::CmdSizeOf(iLen + 1),
				DynamicMessage::CmdSizeOf(iLen + 1)
			);

			
			pDynamicPacket->Construct<0>(iLen, arg1->Chat);
			pDynamicPacket->Construct<1>(iLen, arg1->Chat);
			auto g = pDynamicPacket->Get<0>();
			auto g1 = pDynamicPacket->Get<1>();


			// Ŭ���̾�Ʈ ����
			if (arg1->Chat[0] == 'c') {
				break;
			}
			
			if (!client.SendAsync(pStaticPacket)) {
				std::cout << "[Ŭ��] Static ��Ŷ �۽� ����\n";
			}
			if (!client.SendAsync(pDynamicPacket)) {
				std::cout << "[Ŭ��] Dyanmic �۽� ����\n";
			}
		}
	}


	if (!client.Disconnect()) {
		std::cout << "[Ŭ��] ���� ���� ����\n";
	}

	Winsock::Finalize();
	return 0;
}