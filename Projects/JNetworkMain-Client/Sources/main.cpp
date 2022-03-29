#include "header.h"




class MyEventListener : public TcpClientEventListener
{
protected:
	virtual void OnConnected() {
		Winsock::Message("������ ����Ǿ����ϴ�.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("������ ������ ���������ϴ�.");
	}

	virtual void OnSent(ISendPacket* packet, Int32UL sentBytes) {
		static int cnt = 0;
		static CriticalSectionMutex mtx;
		mtx.Lock();
		Winsock::Message("%d��° ��Ŷ �۽� : %d����Ʈ", ++cnt, sentBytes);
		mtx.Unlock();
	}

	virtual void OnReceived(ICommand* command) {
		Winsock::Message("���� : %d", command->GetCommandLen());
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
		pPacket->AddRef(); // ��Ŷ ������
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