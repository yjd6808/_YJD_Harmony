#pragma once

#include <JCore/Declspec.h>
#include <JNetwork/Socket.h>

NS_JNET_BEGIN

class JCORE_NOVTABLE Server
{
public:
	enum class State
	{
		Stopped,
		Initialized,
		Running
	};

	enum class Type
	{
		None,
		Tcp,
		Udp
	};
public:
	Server() : m_eServerType(Type::None), m_eState(State::Initialized), m_ServerSocket(TransportProtocol::None, INVALID_SOCKET) {}
	virtual ~Server() = default;

	virtual bool Start(const IPv4EndPoint& localEndPoint) = 0;
	virtual bool Stop() = 0;

	Socketv4 Socket() const { return m_ServerSocket; }
protected:
	Type m_eServerType;
	State m_eState;
	Socketv4 m_ServerSocket;
};

NS_JNET_END