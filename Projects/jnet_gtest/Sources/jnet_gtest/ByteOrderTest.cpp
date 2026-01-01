#include <jnet_gtest/Core.h>

#include <jnet/ByteOrder.h>

#if TEST_ByteOrderTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
TEST(ByteOrderTest, ByteOrderTest)
{
	constexpr Int32U val = 0xaabbccdd;

	const Int32U valToNetwork = htonl(val);
	EXPECT_TRUE(valToNetwork == ByteOrder::HostToNetwork(val));
	EXPECT_TRUE(ntohl(valToNetwork) == ByteOrder::NetworkToHost(valToNetwork));
}

#endif
