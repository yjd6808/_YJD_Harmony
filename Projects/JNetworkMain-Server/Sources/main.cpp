#include "header.h"

class MyEventListener : public TcpServerEventListener
{
protected:
	void OnStarted() override {
		Winsock::Message("서버가 시작되었습니다.");
	}

	struct fsefs
	{
		//char a[1'000'000'000];
		char a[30];
	};

	void OnConnected(TcpSession* connectedSession) override {
		std::cout << connectedSession->GetRemoteEndPoint().ToString() << " 클라이언트가 접속하였습니다.\n";
	}

	void OnDisconnected(TcpSession* disconnetedSession) override {
		std::cout << disconnetedSession->GetRemoteEndPoint().ToString() << " 클라이언트가 접속을 종료하였습니다.\n";
	}

	void OnSent(TcpSession* sender, ISendPacket* packet, Int32UL sentBytes) override {
		Winsock::Message("송신 : %d 바이트", packet->GetPacketLength());
	}

	void OnReceived(TcpSession* receiver, ICommand* cmd) override {
		static int cnt = 0;

		Winsock::Message("%d번째 커맨드 길이 : %d", ++cnt, cmd->GetCommandLen());
	}

	void OnPaused() override {
		Winsock::Message("서버가 일시정지되었습니다.");
	}

	void OnResume() override {
		Winsock::Message("서버가 다시 가동 되었습니다.");
	}

	void OnStopped() override {
		Winsock::Message("서버가 종료되었습니다.");
	}

};



int main() {

	Winsock::Initialize(2, 2);

	TcpServer server;
	MyEventListener myEventListener;
	server.SetEventListener(&myEventListener);

	if (server.Start("0.0.0.0:9999")) {
		server.Pause();
		server.Resume();
		//server.Stop();
	}


	Sleep(5000000);

	Winsock::Finalize();
	return 0;
}