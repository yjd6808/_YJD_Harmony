#include "header.h"

class MyEventListener : public TcpServerEventListener
{
protected:
	virtual void OnStarted() {
		Winsock::Message("������ ���۵Ǿ����ϴ�.");
	}

	struct fsefs
	{
		//char a[1'000'000'000];
		char a[30];
	};

	virtual void OnConnected(TcpSession* connectedSession) {
		std::cout << connectedSession->GetRemoteEndPoint().ToString() << " Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.\n";
	}

	virtual void OnDisconnected(TcpSession* disconnetedSession) {
		std::cout << disconnetedSession->GetRemoteEndPoint().ToString() << " Ŭ���̾�Ʈ�� ������ �����Ͽ����ϴ�.\n";
	}

	virtual void OnSent(TcpSession* sender, IPacket* packet, Int32UL sentBytes) {
		Winsock::Message("�۽� : %d ����Ʈ", packet->GetPacketLength());
	}

	virtual void OnReceived(TcpSession* receiver, Int32UL receivedBytes) {
		Winsock::Message("���� : %d ����Ʈ", receivedBytes);
	}

	virtual void OnPaused() {
		Winsock::Message("������ �Ͻ������Ǿ����ϴ�.");
	}

	virtual void OnResume() {
		Winsock::Message("������ �ٽ� ���� �Ǿ����ϴ�.");
	}

	virtual void OnStopped() {
		Winsock::Message("������ ����Ǿ����ϴ�.");
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