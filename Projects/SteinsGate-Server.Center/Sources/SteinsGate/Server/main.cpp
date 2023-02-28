
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Research/ServerNetMaster.h>

#include <JNetwork/Research/Command.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	Winsock::Initialize(2, 2);
	{
		/*
		constexpr int a= DynamicMessage::Size(5);

		auto pPacket = dbg_new DynamicPacket<DynamicMessage, DynamicMessage, DynamicMessage>(5, 6, 7);
		auto a1 = pPacket->Get<0>();
		auto a2 = pPacket->Get<1>();
		auto a3 = pPacket->Get<2>();
		*/

		ServerNetMaster master;
		master.Initialize();

		while (getchar() != 'c') {}

	}
	Winsock::Finalize();
	return 0;
}