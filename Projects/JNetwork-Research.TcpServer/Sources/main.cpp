#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Research/ServerNetMaster.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	Winsock::Initialize(2, 2);
	InitializeNetLogger();
	{
		ServerNetMaster master;
		master.Initialize();

		while (getchar() != 'c') {}

	}
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}