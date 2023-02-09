/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션/클라 들의 공통기능 정의
 */

#pragma once


#include <JNetwork/Host/Host.h>
#include <JNetwork/EventListener/TcpServerEventListener.h>

#include <JNetwork/Buffer/BufferAbstract.h>


NS_JNET_BEGIN

class IOCPOverlapped;
class JCORE_NOVTABLE Session : public Host
{
public:
	Session(const IOCPPtr& iocp, int recvBufferSize, int sendBufferSize);
	virtual ~Session();

	IPv4EndPoint GetLocalEndPoint() const { return m_LocalEndPoint; }
	IPv4EndPoint GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	BufferAbstractPtr GetRecvBuffer() { return m_pRecvBuffer; }
	BufferAbstractPtr GetSendBuffer() { return m_pSendBuffer; }

	void Initialize() override;
	bool Bind(const IPv4EndPoint& bindAddr);
	bool Connect(const IPv4EndPoint& remoteAddr);
	bool Disconnect();
	bool SendAsync(ISendPacket* packet);
	bool RecvAsync();

	virtual void Connected() = 0;
	virtual void Disconnected() = 0;
	virtual void Received(Int32UL receivedBytes);						
	virtual void Sent(ISendPacket* sentPacket, Int32UL receivedBytes) = 0;
	virtual void NotifyCommand(ICommand* cmd) = 0;

	void AddPendingCount()		{ ++m_iOveralappedPendingCount;		}
	void DecreasePendingCount()	{ --m_iOveralappedPendingCount;		}
	int  GetPendingCount()		{ return m_iOveralappedPendingCount;	}
	void WaitForZeroPending();
protected:
	JCore::AtomicInt m_iOveralappedPendingCount;

	BufferAbstractPtr m_pRecvBuffer;
	BufferAbstractPtr m_pSendBuffer;

	IPv4EndPoint m_LocalEndPoint;
	IPv4EndPoint m_RemoteEndPoint;

	friend class IOCPOverlappedAccept;
	friend class IOCPOverlappedConnect;
	friend class IOCPOverlappedRecv;
	friend class IOCPOverlappedSend;
};

using SessionPtr = JCore::SharedPtr<Session>;

NS_JNET_END