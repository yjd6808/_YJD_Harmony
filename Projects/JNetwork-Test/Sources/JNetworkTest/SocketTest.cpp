#include <JNetworkTest/NetworkTest.h>

#if TEST_SocketTest == ON

TEST(SocketTest, IPv4) {
	Winsock::Initialize(2, 2);
	Socketv4 socket = Socket::CreateV4(TransportProtocol::TCP, true);

	EXPECT_TRUE(socket.Option().IsReuseAddressEnabled() == false);
	socket.Option().SetReuseAddrEnabled(true);
	EXPECT_TRUE(socket.Option().IsReuseAddressEnabled() == true);
	socket.Bind(IPv4EndPoint::Parse("127.0.0.1:9999"));
	EXPECT_TRUE(socket.GetLocalEndPoint() == "127.0.0.1:9999");


	Winsock::Finalize();
}


#endif