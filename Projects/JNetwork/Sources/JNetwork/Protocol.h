/*
 *	작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>
#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

enum class InternetProtocol
{
	None = 0,
	IPv4 = AF_INET,
	IPv6 = AF_INET6
};

enum class TransportProtocol
{
	None = 0,
	TCP  = IPPROTO_TCP,
	UDP  = IPPROTO_UDP 
};

inline const char* TransportProtocolName(TransportProtocol protocol) {
	if (protocol == TransportProtocol::TCP)
		return "TCP";
	if (protocol == TransportProtocol::UDP)
		return "UDP";

	return "None";
}

NS_JNET_END

