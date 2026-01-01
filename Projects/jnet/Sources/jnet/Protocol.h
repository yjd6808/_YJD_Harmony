/*
 *	작성자 : 윤정도
 */

#pragma once

#include <WinSock2.h>
#include <jnet/Namespace.h>

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

constexpr const char* TransportProtocolName(TransportProtocol _protocol)
{
	if (_protocol == TransportProtocol::TCP)
		return "TCP";
	if (_protocol == TransportProtocol::UDP)
		return "UDP";

	return "None";
}

constexpr char TransmissionName(Transmission _transmission)
{
	if (_transmission == Transmission::Recv)
		return 'R';
	if (_transmission == Transmission::Send)
		return 'S';
	return 'X';
}

NS_JNET_END
