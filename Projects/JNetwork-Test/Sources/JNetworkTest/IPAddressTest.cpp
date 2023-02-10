#include <JNetworkTest/NetworkTest.h>

#include <JNetwork/IPAddress.h>


#if TEST_IPAddressTest == ON

TEST(IPAddressTest, IPv4) {
	EXPECT_DEATH(IPv4Address("255.255.255.256"), "");
	EXPECT_DEATH(IPv4Address("255.255.256.255"), "");
	EXPECT_DEATH(IPv4Address("255.256.255.255"), "");
	EXPECT_DEATH(IPv4Address("256.255.255.255"), "");
	EXPECT_NO_THROW(IPv4Address("255.255.255.255"));
	EXPECT_NO_THROW(IPv4Address("255.255.255.255"));
	EXPECT_NO_THROW(IPv4Address("0.0.0.0"));

	const IPv4Address ip("121.122.123.124");

	EXPECT_ANY_THROW(ip.GetAddressOctet(-1));
	auto k = ip.GetAddressOctet(0);
	EXPECT_TRUE(ip.GetAddressOctet(0) == 121);
	EXPECT_TRUE(ip.GetAddressOctet(1) == 122);
	EXPECT_TRUE(ip.GetAddressOctet(2) == 123);
	EXPECT_TRUE(ip.GetAddressOctet(3) == 124);
	EXPECT_ANY_THROW(ip.GetAddressOctet(4));

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	//inet_pton(AF_INET, ip.ToString().Source(), &addr.sin_addr);
	addr.sin_addr.S_un.S_addr = htonl(ip.GetAddress());

	char buff[22];
	const String s = inet_ntoa(addr.sin_addr);
	String s2 = inet_ntop(AF_INET, &addr, buff, 22);
	EXPECT_TRUE(s == "121.122.123.124");
}


#endif