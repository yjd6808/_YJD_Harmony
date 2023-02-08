/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션들의 공통기능 정의
 */

#pragma once


#include <JNetwork/Host/Listener/TcpServerEventListener.h>
#include <JNetwork/Host/SessionState.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

#include <JNetwork/Socket.h>
#include <JNetwork/Buffer/BufferAbstract.h>


NS_JNET_BEGIN

class IOCPOverlapped;
class Session
{
public:
	Session(const IOCPPtr& iocp, int recvBufferSize, int sendBufferSize);
	virtual ~Session();

	IPv4EndPoint GetLocalEndPoint() const { return m_LocalEndPoint; }
	IPv4EndPoint GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	BufferAbstractPtr GetRecvBuffer() { return m_pRecvBuffer; }
	BufferAbstractPtr GetSendBuffer() { return m_pSendBuffer; }
	const Socketv4& Socket() const { return m_Socket; }

	bool Bind(const IPv4EndPoint& bindAddr);
	bool ConnectIocp();
	bool Connect(const IPv4EndPoint& remoteAddr);
	bool Disconnect();
	bool SendAsync(ISendPacket* packet);
	bool RecvAsync();
	bool CreateSocket(TransportProtocol protocol);

	virtual void	ReuseInitialize();
	virtual void	Connected() = 0;
	virtual void	Disconnected() = 0;
	virtual void	Received(Int32UL receivedBytes);						
	virtual void	Sent(ISendPacket* sentPacket, Int32UL receivedBytes) = 0;

	virtual void	NotifyCommand(ICommand* cmd) = 0;

	SessionState	GetSessionState() { return m_eSessionState; }

protected:
	BufferAbstractPtr m_pRecvBuffer;
	BufferAbstractPtr m_pSendBuffer;

	IOCPPtr m_spIocp;
	Socketv4 m_Socket;
	SessionState m_eSessionState;
	IPv4EndPoint m_LocalEndPoint;
	IPv4EndPoint m_RemoteEndPoint;

	bool m_bIocpConneced;

	friend class IOCPOverlappedAccept;
	friend class IOCPOverlappedConnect;
	friend class IOCPOverlappedRecv;
	friend class IOCPOverlappedSend;
};

NS_JNET_END