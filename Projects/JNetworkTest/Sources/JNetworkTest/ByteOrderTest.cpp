#include <JNetworkTest/NetworkTest.h>

#include <JCore/Exception.h>

#include <JNetwork/ByteOrder.h>

#if TEST_ByteOrderTest == ON

TEST(ByteOrderTest, ByteOrderTest) {
	Int32U val = 0xaabbccdd;

	Int32U valToNetwork = htonl(val);
	EXPECT_TRUE(valToNetwork == ByteOrder::HostToNetwork(val));
	EXPECT_TRUE(ntohl(valToNetwork) == ByteOrder::NetworkToHost(valToNetwork));
}


#endif