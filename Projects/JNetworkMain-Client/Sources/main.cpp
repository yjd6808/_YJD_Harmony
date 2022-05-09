#define _WINSOCKAPI_			// Winsock.h include 방지

#include <JCore/LockGuard.h>

#include <JNetwork/Network.h>	// 1. JCore.lib 링크
								// 2. Mswsock.lib 링크
								// 3. ws2_32.lib 링크

#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpClient.h>

using namespace JCore;
using namespace JNetwork;

#define CMD_STATIC_MESSAGE	0
#define CMD_DYNAMIC_MESSAGE	1

struct StaticMessage : ICommand
{
	StaticMessage() {
		this->Cmd = CMD_STATIC_MESSAGE;	// 명령어 코드값 (개발시 사용)
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
		Winsock::Message("[클라] 서버와 연결되었습니다.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("[클라] 연결이 종료되었습니다.");
	}

	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
		Winsock::Message("[클라] 커맨드 %d개를 송신했습니다.", sentPacket->GetCommandCount());
	}

	virtual void OnReceived(ICommand* cmd) {
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE)
			Winsock::Message("[클라] 스태틱 메시지를 수신했습니다. : %s", cmd->CastCommand<StaticMessage*>()->Chat);
		else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			Winsock::Message("[클라] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);
		}
	}
};


int main() {
	CriticalSectionMutex mtx;


	// 멀티 패킷 예시
	auto pPakcet = new StaticPacket<Command<int>, StaticMessage, Command<float>>();
	Command<int>* arg1		= pPakcet->Get<0>();
	StaticMessage* arg2		= pPakcet->Get<1>();
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


			auto pStaticPacket = new StaticPacket<StaticMessage>();
			StaticMessage* arg1 = pStaticPacket->Get<0>();
			std::cin >> arg1->Chat;

			int iLen = StringUtil::Length(arg1->Chat);

			// Dynamic 패킷 예시
			auto pDynamicPacket = new DynamicPacket<DynamicMessage, DynamicMessage>(
				DynamicMessage::CmdSizeOf(iLen + 1),
				DynamicMessage::CmdSizeOf(iLen + 1)
			);

			
			pDynamicPacket->Construct<0>(iLen, arg1->Chat);
			pDynamicPacket->Construct<1>(iLen, arg1->Chat);
			auto g = pDynamicPacket->Get<0>();
			auto g1 = pDynamicPacket->Get<1>();


			// 클라이언트 종료
			if (arg1->Chat[0] == 'c') {
				break;
			}
			
			if (!client.SendAsync(pStaticPacket)) {
				std::cout << "[클라] Static 패킷 송신 실패\n";
			}
			if (!client.SendAsync(pDynamicPacket)) {
				std::cout << "[클라] Dyanmic 송신 실패\n";
			}
		}
	}


	if (!client.Disconnect()) {
		std::cout << "[클라] 연결 종료 실패\n";
	}

	Winsock::Finalize();
	return 0;
}