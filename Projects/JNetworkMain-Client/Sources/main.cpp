#include "header.h"


TcpClient client;

class MyEventListener : public TcpClientEventListener
{
protected:
	virtual void OnConnected() {
		Winsock::Message("서버와 연결되었습니다.");
	}
	virtual void OnDisconnected() {
		Winsock::Message("서버와 연결이 끊어졌습니다.");
	}

	virtual void OnSent(IPacket* packet, Int32UL sentBytes) {
		Winsock::Message("송신 : %d", sentBytes);
	}

	virtual void OnReceived(SessionBuffer* buffer, Int32UL receivedBytes) {
		Winsock::Message("수신 : %d", receivedBytes);
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