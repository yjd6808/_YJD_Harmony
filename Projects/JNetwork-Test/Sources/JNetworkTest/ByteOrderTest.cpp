#include <JNetworkTest/NetworkTest.h>

#include <JNetwork/ByteOrder.h>

#if TEST_ByteOrderTest == ON

TEST(ByteOrderTest, ByteOrderTest) {
	const Int32U val = 0xaabbccdd;

	const Int32U valToNetwork = htonl(val);
	EXPECT_TRUE(valToNetwork == ByteOrder::HostToNetwork(val));
	EXPECT_TRUE(ntohl(valToNetwork) == ByteOrder::NetworkToHost(valToNetwork));
}


#endif