
#include <JNetwork/Network.h>
#include <JNetwork/Research/UdpClientNetGroup.h>

USING_NS_JNET;
USING_NS_JC;

int main() {
	// 멀티 패킷 예시
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	Winsock::Initialize(2, 2);

	IOCPPtr iocp = MakeShared<IOCP>(8);
	iocp->Run();

	MyClientEventListener myClientEventListener;
	UdpClient client{ iocp, &myClientEventListener};
	auto g = client.Socket().GetLocalEndPoint();
	client.Bind("0.0.0.0:9999");
	client.RecvFromAsync();
	StaticPacket<StaticMessage>* sendPacket = nullptr;
	while (1) {
		sendPacket = new StaticPacket<StaticMessage>();
		StaticMessage* arg1 = sendPacket->Get<0>();
		std::cin >> arg1->Chat;
		// 클라이언트 종료
		if (arg1->Chat[0] == 'c') {
			break;
		}
		
	}

	client.Disconnect();
	iocp->Join();
	iocp->Destroy();
	Winsock::Finalize();

	DeleteSafe(sendPacket);
	return 0;
}