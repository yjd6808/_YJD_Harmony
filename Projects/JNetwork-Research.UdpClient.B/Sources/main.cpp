
#include <JNetwork/Network.h>	// JCore.lib 링크, 2. Mswsock.lib 링크, 3. ws2_32.lib 링크
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/Packet/Packet.h>

#include <JCore/Utils/Console.h>

USING_NS_JNET;
USING_NS_JC;

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



class MyClientEventListener : public ClientEventListener
{
protected:
	virtual void OnConnected() {
		SafeConsole::WriteLine("[B] 서버와 연결되었습니다.");
	}
	virtual void OnDisconnected() {
		SafeConsole::WriteLine("[B] 연결이 종료되었습니다.");
	}

	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
		SafeConsole::WriteLine("[B] 커맨드 %d개를 송신했습니다.", sentPacket->GetCommandCount());
	}

	virtual void OnReceived(ICommand* cmd) {
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE)
			SafeConsole::WriteLine("[B] 스태틱 메시지를 수신했습니다. : %s", cmd->CastCommand<StaticMessage*>()->Chat);
		else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			SafeConsole::WriteLine("[B] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);
		}
	}
};

int main() {
	// 멀티 패킷 예시
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	Winsock::Initialize(2, 2);

	IOCPPtr iocp = MakeShared<IOCP>(8);
	iocp->Run();

	MyClientEventListener myClientEventListener;
	UdpClient client{ iocp, &myClientEventListener};
	auto g = client.Socket().GetLocalEndPoint();
	client.Bind("0.0.0.0:9999");
	client.RecvFromAsync();
	StaticPacket<StaticMessage>* sendPacket = nullptr;
	while (1) {
		sendPacket = new StaticPacket<StaticMessage>();
		StaticMessage* arg1 = sendPacket->Get<0>();
		std::cin >> arg1->Chat;
		// 클라이언트 종료
		if (arg1->Chat[0] == 'c') {
			break;
		}
		
	}

	client.Disconnect();
	iocp->Join();
	iocp->Destroy();
	Winsock::Finalize();

	DeleteSafe(sendPacket);
	return 0;
}