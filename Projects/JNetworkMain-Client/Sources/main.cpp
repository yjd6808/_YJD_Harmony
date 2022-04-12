#define _WINSOCKAPI_			// Winsock.h include 방지

#include <JCore/LockGuard.h>

#include <JNetwork/Network.h>	// 1. JCore.lib 링크
								// 2. Mswsock.lib 링크
								// 3. ws2_32.lib 링크

#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpClient.h>

using namespace JCore;
using namespace JNetwork;

struct Message : ICommand
{
	Message() {
		this->Cmd = 0;	// 명령어 코드값 (개발시 사용)
		this->CmdLen = sizeof(Message);
	}

	char Chat[512];
};


class MyClientEventListener : public TcpClientEventListener
{
protected:
	virtual void OnConnected() {
		Winsock::Message("[클라] 서버와 연결되었습니다.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("[클라] 연결이 종료되었습니다.");
	}

	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
		Winsock::Message("[클라] 커맨드 %d개를 송신했습니다.", sentPacket->GetCommandCount());
	}

	virtual void OnReceived(ICommand* cmd) {
		Winsock::Message("[클라] 커맨드를 수신했습니다. : %s", cmd->CastCommand<Message*>()->Chat);
	}
};


int main() {
	CriticalSectionMutex mtx;

	auto pPakcet = new Packet<Command<int>, Message, Command<float>>();
	Command<int>* arg1		= pPakcet->Get<0>();
	Message* arg2			= pPakcet->Get<1>();
	Command<float>* arg3	= pPakcet->Get<2>();

	arg1->Value = 30;
	strcpy_s(arg2->Chat, 512, "안녕하세요");
	arg3->Value = 40;



	Winsock::SetMutex(&mtx);
	Winsock::Initialize(2, 2);

	TcpClient client;
	MyClientEventListener myClientEventListener;
	client.SetEventListener(&myClientEventListener);


	if (client.ConnectAsync("127.0.0.1:9999")) {
		while (1) {


			// 패킷에는 여러개의 커맨드를 담아서 한번에 전송가능
			auto pPacket = new Packet<Message>();
			Message* arg1 = pPacket->Get<0>();
			std::cin >> arg1->Chat;

			// 클라이언트 종료
			if (arg1->Chat[0] == 'c') {
				break;
			}

			if (!client.SendAsync(pPacket)) {
				std::cout << "[클라] 송신 실패\n";
			}
		}
	}


	if (!client.Disconnect()) {
		std::cout << "[클라] 연결 종료 실패\n";
	}

	Winsock::Finalize();
	return 0;
}