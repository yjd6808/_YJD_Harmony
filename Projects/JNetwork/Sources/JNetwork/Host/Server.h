#pragma once

#include <JNetwork/Socket.h>

namespace JNetwork {

class Server
{
public:
	enum class State
	{
		Stopped,
		Initialized,
		Running,
		Paused
	};

	enum class Type
	{
		None,
		Tcp,
		Udp
	};
public:
	Server() : m_ServerSocket(INVALID_SOCKET), m_eState(State::Initialized), m_eServerType(Type::None) {}
	virtual ~Server() {}

	virtual bool Start(const IPv4EndPoint& localEndPoint) = 0;
	virtual bool Stop() = 0;

	TcpSocketv4 Socket() const { return m_ServerSocket; }
protected:
	Type m_eServerType;
	State m_eState;
	TcpSocketv4 m_ServerSocket;
};

}