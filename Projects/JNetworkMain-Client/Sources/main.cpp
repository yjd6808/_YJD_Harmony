#include "header.h"




class MyEventListener : public TcpClientEventListener
{
protected:
	void OnConnected() override {
		Winsock::Message("서버와 연결되었습니다.");
	}

	void OnDisconnected() override {
		Winsock::Message("서버와 연결이 끊어졌습니다.");
	}

	void OnSent(ISendPacket* packet, Int32UL sentBytes) override {
		static int cnt = 0;
		static CriticalSectionMutex mtx;
		mtx.Lock();
		Winsock::Message("%d번째 패킷 송신 : %d바이트", ++cnt, sentBytes);
		mtx.Unlock();
	}

	void OnReceived(ICommand* command) override {
		Winsock::Message("수신 : %d", command->GetCommandLen());
	}
};


int main() {
	Winsock::Initialize(2, 2);


	{
		MemoryLeakDetector detector;
		TcpClient client;
		MyEventListener eventListener;
		client.SetEventListener(&eventListener);
		client.ConnectAsync("121.145.173.195:9999");
		Sleep(1000);

		struct A
		{
			char b[1];
		};

		auto* pPacket = new Packet<Command<A>, Command<A>>();
		pPacket->Get<0>()->SetCommand(0xa);
		pPacket->Get<1>()->SetCommand(0xd);
		pPacket->AddRef(); // 패킷 베리어
		for (int i = 0; i < 500; i++) {
			client.SendAsync(pPacket);
		}
		pPacket->Release();

		Sleep(1000000);
		client.Disconnect();
	}


	//client.Disconnect();
	

	Sleep(500000);
	Winsock::Finalize();
	return 0;
}