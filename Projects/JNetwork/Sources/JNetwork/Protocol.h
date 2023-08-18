/*
 *	작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>
#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

enum class Transmission
{
	Send,
	Recv
};

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

constexpr const char* TransportProtocolName(TransportProtocol protocol) {
	if (protocol == TransportProtocol::TCP)
		return "TCP";
	if (protocol == TransportProtocol::UDP)
		return "UDP";

	return "None";
}

constexpr char TransmissionName(Transmission transmission) {
	if (transmission == Transmission::Recv)
		return 'R';
	if (transmission == Transmission::Send)
		return 'S';
	return 'X';
}

NS_JNET_END

