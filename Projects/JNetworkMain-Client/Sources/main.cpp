#include "header.h"


TcpClient client;

class MyEventListener : public TcpClientEventListener
{
protected:
	virtual void OnConnected() {
		Winsock::Message("������ ����Ǿ����ϴ�.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("������ ������ ���������ϴ�.");
	}

	virtual void OnSent(IPacket* packet, Int32UL sentBytes) {
		Winsock::Message("�۽� : %d", sentBytes);
	}

	virtual void OnReceived(SessionBuffer* buffer, Int32UL receivedBytes) {
		Winsock::Message("���� : %d", receivedBytes);
	}
};

int main() {
	Winsock::Initialize(2, 2);

	
	client.SetEventListener(new MyEventListener());
	client.ConnectAsync("121.145.173.195:9999");
	Sleep(1000);
	client.Disconnect();
	

	Sleep(500000);
	Winsock::Finalize();
	return 0;
}