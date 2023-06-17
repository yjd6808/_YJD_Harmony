/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:12:18 PM
 * =====================
 * IOCP 이용자의 공통 기능 정의
 */


#pragma once



#include <JNetwork/Socket.h>
#include <JNetwork/IOCP/IOCP.h>

NS_JNET_BEGIN

class JCORE_NOVTABLE Host
{
public:
	Host(const IOCPPtr& iocp)
		: m_spIocp(iocp)
		, m_Socket(TransportProtocol::None, INVALID_SOCKET)
		, m_eState(eNone)
		, m_bIocpConneced(false)
	{}
	virtual ~Host() = default;

	enum Type
	{
		eListener,
		eClient,
		eSession
	};

	enum DetailType
	{
		eTcpListener,
		eTcpSession,
		eTcpClient,
		eUdpClient
	};

	enum State
	{
		eNone			= 0,
		eInitailized	= 1,
		eAcceptWait		= 2,
		eConnectWait	= 2,
		eListening		= 3,
		eAccepted		= 3,
		eConnected		= 3,
		eDisconnected	= 4,
		eStopped		= 4
	};

	virtual void Initialize() = 0;
	virtual Type GetType() const = 0;
	virtual DetailType GetDetailType() const = 0;
	virtual const char* TypeName() = 0;
	State GetState() { return (State)m_eState.Load(); }
	bool CreateSocket(TransportProtocol protocol, bool nonBlocking = true);
	bool ConnectIocp();
	const Socketv4& Socket() const { return m_Socket; }
	SOCKET SocketHandle() const { return m_Socket.Handle; }
	bool IsTCP() const { return m_Socket.Protocol == TransportProtocol::TCP; }
	bool IsUDP() const { return m_Socket.Protocol == TransportProtocol::UDP; }
	TransportProtocol Protocol() const { return m_Socket.Protocol; }
protected:
	IOCPPtr m_spIocp;
	Socketv4 m_Socket;
	JCore::AtomicInt m_eState;
	bool m_bIocpConneced;
};

using HostPtr = JCore::SharedPtr<Host>;

NS_JNET_END