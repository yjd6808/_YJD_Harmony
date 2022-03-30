#include "header.h"

class MyEventListener : public TcpServerEventListener
{
protected:
	void OnStarted() override {
		Winsock::Message("������ ���۵Ǿ����ϴ�.");
	}

	struct fsefs
	{
		//char a[1'000'000'000];
		char a[30];
	};

	void OnConnected(TcpSession* connectedSession) override {
		std::cout << connectedSession->GetRemoteEndPoint().ToString() << " Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.\n";
	}

	void OnDisconnected(TcpSession* disconnetedSession) override {
		std::cout << disconnetedSession->GetRemoteEndPoint().ToString() << " Ŭ���̾�Ʈ�� ������ �����Ͽ����ϴ�.\n";
	}

	void OnSent(TcpSession* sender, ISendPacket* packet, Int32UL sentBytes) override {
		Winsock::Message("�۽� : %d ����Ʈ", packet->GetPacketLength());
	}

	void OnReceived(TcpSession* receiver, ICommand* cmd) override {
		static int cnt = 0;

		Winsock::Message("%d��° Ŀ�ǵ� ���� : %d", ++cnt, cmd->GetCommandLen());
	}

	void OnPaused() override {
		Winsock::Message("������ �Ͻ������Ǿ����ϴ�.");
	}

	void OnResume() override {
		Winsock::Message("������ �ٽ� ���� �Ǿ����ϴ�.");
	}

	void OnStopped() override {
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