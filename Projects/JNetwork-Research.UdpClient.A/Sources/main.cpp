
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
		SafeConsole::WriteLine("[A] 서버와 연결되었습니다.");
	}
	virtual void OnDisconnected() {
		SafeConsole::WriteLine("[A] 연결이 종료되었습니다.");
	}

	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
		SafeConsole::WriteLine("[A] 커맨드 %d개를 송신했습니다.", sentPacket->GetCommandCount());
	}

	virtual void OnReceived(ICommand* cmd) {
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE)
			SafeConsole::WriteLine("[A] 스태틱 메시지를 수신했습니다. : %s", cmd->CastCommand<StaticMessage*>()->Chat);
		else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			SafeConsole::WriteLine("[A] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);
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
	

	StaticPacket<StaticMessage>* sendPacket = nullptr;
	while (1) {
		sendPacket = new StaticPacket<StaticMessage>();
		StaticMessage* arg1 = sendPacket->Get<0>();
		std::cin >> arg1->Chat;
		// 클라이언트 종료
		if (arg1->Chat[0] == 'c') {
			break;
		}
		int iLen = StringUtil::Length(arg1->Chat);

		// Dynamic 패킷 예시
		auto pDynamicPacket = new DynamicPacket<DynamicMessage, DynamicMessage>(
			DynamicMessage::CmdSizeOf(iLen + 1),
			DynamicMessage::CmdSizeOf(iLen + 1)
		);

		{
			auto g = client.Socket().GetLocalEndPoint().ToString();
			int c = 0;
		}
		pDynamicPacket->Construct<0>(iLen, arg1->Chat);
		pDynamicPacket->Construct<1>(iLen, arg1->Chat);
		auto g = pDynamicPacket->Get<0>();
		auto g1 = pDynamicPacket->Get<1>();

		if (!client.SendToAsync(sendPacket, "127.0.0.1:9999")) {
			std::cout << "[A] Static 패킷 송신 실패\n";
		}
		if (!client.SendToAsync(pDynamicPacket, "127.0.0.1:9999")) {
			std::cout << "[A] Dyanmic 송신 실패\n";
		}

	}

	client.Disconnect();
	iocp->Join();
	iocp->Destroy();

	Winsock::Finalize();

	DeleteSafe(sendPacket);
	return 0;
}