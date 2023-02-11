
#include <JNetwork/Network.h>
#include <JNetwork/Research/UdpClientNetGroup.h>

USING_NS_JNET;
USING_NS_JC;


int main() {
	
	
	Winsock::Initialize(2, 2);

	IOCPPtr iocp = MakeShared<IOCP>(8);
	iocp->Run();

	MyClientEventListener myClientEventListener;
	UdpClient client{ iocp, &myClientEventListener};
	

	StaticPacket<StaticMessage>* sendPacket = nullptr;
	while (1) {
		sendPacket = new StaticPacket<StaticMessage>();
		StaticMessage* arg1 = sendPacket->Get<0>();
		std::cin >> arg1->Chat;
		// 클라이언트 종료
		if (arg1->Chat[0] == 'c') {
			break;
		}
		int iLen = StringUtil::Length(arg1->Chat);

		// Dynamic 패킷 예시
		auto pDynamicPacket = new DynamicPacket<DynamicMessage, DynamicMessage>(
			DynamicMessage::CmdSizeOf(iLen + 1),
			DynamicMessage::CmdSizeOf(iLen + 1)
		);

		{
			auto g = client.Socket().GetLocalEndPoint().ToString();
			int c = 0;
		}
		pDynamicPacket->Construct<0>(iLen, arg1->Chat);
		pDynamicPacket->Construct<1>(iLen, arg1->Chat);
		auto g = pDynamicPacket->Get<0>();
		auto g1 = pDynamicPacket->Get<1>();

		if (!client.SendToAsync(sendPacket, "127.0.0.1:9999")) {
			std::cout << "[A] Static 패킷 송신 실패\n";
		}
		if (!client.SendToAsync(pDynamicPacket, "127.0.0.1:9999")) {
			std::cout << "[A] Dyanmic 송신 실패\n";
		}

	}

	client.Disconnect();
	iocp->Join();
	iocp->Destroy();

	Winsock::Finalize();

	DeleteSafe(sendPacket);
	return 0;
}