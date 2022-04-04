#pragma once

#include <WinSock2.h>
#include <Windows.h>

namespace JNetwork {

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
}

