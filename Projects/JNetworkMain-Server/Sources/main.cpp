#include "header.h"

class MyEventListener : public TcpServerEventListener
{
protected:
	virtual void OnStarted() {
		Winsock::Message("서버가 시작되었습니다.");
	}

	struct fsefs
	{
		//char a[1'000'000'000];
		char a[30];
	};

	virtual void OnConnected(TcpSession* connectedSession) {
		std::cout << connectedSession->GetRemoteEndPoint().ToString() << " 클라이언트가 접속하였습니다.\n";
	}

	virtual void OnDisconnected(TcpSession* disconnetedSession) {
		std::cout << disconnetedSession->GetRemoteEndPoint().ToString() << " 클라이언트가 접속을 종료하였습니다.\n";
	}

	virtual void OnSent(TcpSession* sender, IPacket* packet, Int32UL sentBytes) {
		Winsock::Message("송신 : %d 바이트", packet->GetPacketLength());
	}

	virtual void OnReceived(TcpSession* receiver, Int32UL receivedBytes) {
		Winsock::Message("수신 : %d 바이트", receivedBytes);
	}

	virtual void OnPaused() {
		Winsock::Message("서버가 일시정지되었습니다.");
	}

	virtual void OnResume() {
		Winsock::Message("서버가 다시 가동 되었습니다.");
	}

	virtual void OnStopped() {
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