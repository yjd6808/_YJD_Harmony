
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/PacketGenerator.h>
#include <JNetwork/Host/TcpServer.h>

#include <JCore/Utils/Console.h>
#include <JCore/Encoding/CodePage.h>

using namespace JCore;
using namespace JNetwork;

#define CMD_STATIC_MESSAGE	0
#define CMD_DYNAMIC_MESSAGE	1

struct StaticMessage : StaticCommand
{
	StaticMessage() {
		this->Cmd = CMD_STATIC_MESSAGE;	// 명령어 코드값 (개발시 사용)
		this->CmdLen = sizeof(StaticMessage);
	}

	char Chat[512];
};


struct DynamicMessage : DynamicCommand
{
	DynamicMessage(int len, char* msg) {
		this->Length = len;
		this->Cmd = CMD_DYNAMIC_MESSAGE;

		char* pChat = Chat;
		memcpy_s(pChat, len, msg, len);
		pChat[len] = NULL;
	}

	static int SetAllocationSize(int len) {
		return sizeof(DynamicMessage) + sizeof(char) * len;
	}

	int Length;
	char Chat[0];
};



class MyServerEventListener : public TcpServerEventListener
{
protected:
	void OnStarted() override {
		SafeConsole::WriteLine("[서버] 서버가 시작되었습니다.");
	}

	void OnConnected(TcpSession* connectedSession) override { 
		SafeConsole::WriteLine("[서버] %s 클라이언트가 접속하였습니다.", 
			connectedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnDisconnected(TcpSession* disconnetedSession) override {
		SafeConsole::WriteLine("[서버] %s 클라이언트가 접속하였습니다.",
			disconnetedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnSent(TcpSession* sender, ISendPacket* packet, Int32UL sentBytes) override {
		SafeConsole::WriteLine("[서버] 송신 : %d 바이트", packet->GetPacketLength());
	}

	void OnReceived(TcpSession* receiver, ICommand* cmd) override {
		// 수신한 메시지 출력
		if (cmd->GetCommand() == CMD_STATIC_MESSAGE) {
			StaticMessage* pMsg = cmd->CastCommand<StaticMessage*>();
			SafeConsole::WriteLine("[서버] 스태틱 메시지를 수신했습니다. : %s", pMsg->Chat);

			// 스태틱 패킷 에코 진행
			auto pPacket = new StaticPacket<StaticMessage>();
			StaticMessage* arg1 = pPacket->Get<0>();
			strcpy_s(arg1->Chat, 512, pMsg->Chat);

			if (!receiver->SendAsync(pPacket)) {
				SafeConsole::WriteLine("[서버] 스태틱 에코 실패");
			}
		} else if (cmd->GetCommand() == CMD_DYNAMIC_MESSAGE) {
			DynamicMessage* pDynamicMessage = cmd->CastCommand<DynamicMessage*>();
			int iLen = pDynamicMessage->Length;
			SafeConsole::WriteLine("[서버] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", pDynamicMessage->Chat, pDynamicMessage->Length);

			// 다이나믹 패킷 에코 진행
			auto pPacket = new DynamicPacket<DynamicMessage>(DynamicMessage::SetAllocationSize(iLen + 1));
			pPacket->Construct<0>(iLen, pDynamicMessage->Chat);

			if (!receiver->SendAsync(pPacket)) {
				SafeConsole::WriteLine("[서버] 다이나믹 에코 실패");
			}
			
		}
	}

	void OnPaused() override {
		SafeConsole::WriteLine("[서버] 일시정지되었습니다.");
	}

	void OnResume() override {
		SafeConsole::WriteLine("[서버] 다시 가동 되었습니다.");
	}

	void OnStopped() override {
		SafeConsole::WriteLine("[서버] 종료되었습니다.");
	}
};


int main() {
    SafeConsole::Init();
    SafeConsole::SetOutputCodePage(CodePage::UTF8);

	Winsock::Initialize(2, 2);
	TcpServer server;
	MyServerEventListener myServerEventListener;

	// 서버 이벤트 리스너 등록
	server.SetEventListener(&myServerEventListener);

	// 서버 실행
	if (server.Start("0.0.0.0:9999")) {
		server.Pause();		// 서버 일시정지 기능
		server.Resume();	// 서버 계속 진행
	}

	// c 입력시 서버 종료
	while (getchar() != 'c') {}
	server.Stop();
	Winsock::Finalize();
	return 0;
}