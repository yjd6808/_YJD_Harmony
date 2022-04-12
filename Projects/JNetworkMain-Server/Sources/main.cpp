
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
		this->Cmd = 0;	// 명령어 코드값 (개발시 사용)
		this->CmdLen = sizeof(Message);
	}

	char Chat[512];
};

class MyServerEventListener : public TcpServerEventListener
{
protected:
	void OnStarted() override {
		Winsock::Message("[서버] 서버가 시작되었습니다.");
	}

	void OnConnected(TcpSession* connectedSession) override { 
		Winsock::Message("[서버] %s 클라이언트가 접속하였습니다.", 
			connectedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnDisconnected(TcpSession* disconnetedSession) override {
		Winsock::Message("[서버] %s 클라이언트가 접속하였습니다.",
			disconnetedSession->GetRemoteEndPoint().ToString().Source());
	}

	void OnSent(TcpSession* sender, ISendPacket* packet, Int32UL sentBytes) override {
		Winsock::Message("[서버] 송신 : %d 바이트", packet->GetPacketLength());
	}

	void OnReceived(TcpSession* receiver, ICommand* cmd) override {
		// 수신한 메시지 출력
		Message* pMsg = cmd->CastCommand<Message*>();
		Winsock::Message("[서버] 메시지 수신 : %s", pMsg->Chat);

		// 에코 진행
		auto pPacket = new Packet<Message>();
		Message* arg1 = pPacket->Get<0>();
		strcpy_s(arg1->Chat, 512, pMsg->Chat);

		if (!receiver->SendAsync(pPacket)) {
			Winsock::Message("[서버] 에코 실패");
		}
	}

	void OnPaused() override {
		Winsock::Message("[서버] 일시정지되었습니다.");
	}

	void OnResume() override {
		Winsock::Message("[서버] 다시 가동 되었습니다.");
	}

	void OnStopped() override {
		Winsock::Message("[서버] 종료되었습니다.");
	}
};



int main() {
	CriticalSectionMutex mtx;

	Winsock::SetMutex(&mtx);
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
	
	Sleep(5000000);		// 비동기로 시작되기 때문에 main 함수 종료를 막음

	Winsock::Finalize();
	return 0;
}