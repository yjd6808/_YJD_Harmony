#define _WINSOCKAPI_			// Winsock.h include ����

#include <JCore/LockGuard.h>

#include <JNetwork/Network.h>	// 1. JCore.lib ��ũ
								// 2. Mswsock.lib ��ũ
								// 3. ws2_32.lib ��ũ

#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpClient.h>

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
		Winsock::Message("[Ŭ��] Ŀ�ǵ带 �����߽��ϴ�. : %s", cmd->CastCommand<Message*>()->Chat);
	}
};


int main() {
	CriticalSectionMutex mtx;

	auto pPakcet = new Packet<Command<int>, Message, Command<float>>();
	Command<int>* arg1		= pPakcet->Get<0>();
	Message* arg2			= pPakcet->Get<1>();
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


			// ��Ŷ���� �������� Ŀ�ǵ带 ��Ƽ� �ѹ��� ���۰���
			auto pPacket = new Packet<Message>();
			Message* arg1 = pPacket->Get<0>();
			std::cin >> arg1->Chat;

			// Ŭ���̾�Ʈ ����
			if (arg1->Chat[0] == 'c') {
				break;
			}

			if (!client.SendAsync(pPacket)) {
				std::cout << "[Ŭ��] �۽� ����\n";
			}
		}
	}


	if (!client.Disconnect()) {
		std::cout << "[Ŭ��] ���� ���� ����\n";
	}

	Winsock::Finalize();
	return 0;
}