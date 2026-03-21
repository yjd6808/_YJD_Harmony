
#include "jnet/ByteOrder.h"

#if TEST_ByteOrderTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
TEST(ByteOrderTest, ByteOrderTest)
{
	constexpr _u32 val = 0xaabbccdd;

	const _u32 valToNetwork = htonl(val);
	EXPECT_TRUE(valToNetwork == ByteOrder::HostToNetwork(val));
	EXPECT_TRUE(ntohl(valToNetwork) == ByteOrder::NetworkToHost(valToNetwork));
}

#endif
