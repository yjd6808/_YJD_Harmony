#include <JNetworkTest/NetworkTest.h>

#include <JNetwork/IPEndPoint.h>

#if TEST_IPEndPointTest == ON

TEST(IPEndPointTest, IPv4) {
	EXPECT_ANY_THROW(IPv4EndPoint("255.255.255.255:8000 "));
	EXPECT_ANY_THROW(IPv4EndPoint("255.255.255.2558000 "));
	EXPECT_ANY_THROW(IPv4EndPoint("255.255.255.255:8000:"));
	EXPECT_ANY_THROW(IPv4EndPoint("255.255.255.255:"));

	const IPv4EndPoint ep("127.0.0.1:8000");

	EXPECT_TRUE(ep.GetProtocol() == InternetProtocol::IPv4);
	EXPECT_TRUE(ep.GetPort() == 8000);
	EXPECT_TRUE(ep.GetAddress().GetAddressOctet(0) == 127);
	EXPECT_TRUE(ep.GetAddress().GetAddressOctet(1) == 0);
	EXPECT_TRUE(ep.GetAddress().GetAddressOctet(2) == 0);
	EXPECT_TRUE(ep.GetAddress().GetAddressOctet(3) == 1);
	String k = ep.ToString();
	EXPECT_TRUE(ep.ToString() == "127.0.0.1:8000");
}


#endif