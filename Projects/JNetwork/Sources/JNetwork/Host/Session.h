/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션/클라 들의 공통기능 정의
 */

#pragma once


#include <JCore/Sync/RecursiveLock.h>

#include <JNetwork/Host/Host.h>
#include <JNetwork/Host/SessionHelper.h>

#include <JNetwork/EventListener/TcpServerEventListener.h>

#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Packet/RecvPacket.h>

NS_JNET_BEGIN

class IOCPOverlapped;
class JCORE_NOVTABLE Session : public Host
{
	using SessionLockGuard = JCore::LockGuard<JCore::RecursiveLock>;

public:
	Session(const IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator, int recvBufferSize, int sendBufferSize);
	virtual ~Session();

	IPv4EndPoint GetLocalEndPoint() const { return m_LocalEndPoint; }
	IPv4EndPoint GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	CommandBufferPtr GetRecvBuffer() { return m_spRecvBuffer; }
	CommandBufferPtr GetSendBuffer() { return m_spSendBuffer; }

	void Initialize() override;
	bool Bind(const IPv4EndPoint& bindAddr);
	bool Connect(const IPv4EndPoint& remoteAddr);
	bool Disconnect();
	bool SendAsync(ISendPacket* packet);
	bool RecvAsync();

	template <typename TCommand, typename... Args>
	TCommand& SendAlloc(Args&&... args) {
		static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand is not command [2]");

		SessionLockGuard guard(m_SendBufferLock);
		static constexpr int CmdSize = sizeof(TCommand);
		if (m_spSendBuffer->GetWritePos() + CmdSize >= MAX_MSS) {
			FlushSendBuffer();
		}

		DebugAssertMsg(CmdSize <= m_spSendBuffer->GetRemainBufferSize(), "버퍼의 남은 공간에 넣을 커맨드가 너무 큽니다. (CmdSize: %d, RemainBufferCapacity: %d)", CmdSize, m_spSendBuffer->GetRemainBufferSize());
		return m_spSendBuffer->Alloc<TCommand>(JCore::Forward<Args>(args)...);
	}

	
	CommandBufferPacket* GetCommandBufferForSending();
	virtual void FlushSendBuffer();
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
	JCore::MemoryPoolAbstractPtr m_spBufferAllocator;
	JCore::RecursiveLock m_SendBufferLock;

	CommandBufferPtr m_spSendBuffer;
	CommandBufferPtr m_spRecvBuffer;

	IPv4EndPoint m_LocalEndPoint;
	IPv4EndPoint m_RemoteEndPoint;

	friend class IOCPOverlappedAccept;
	friend class IOCPOverlappedConnect;
	friend class IOCPOverlappedRecv;
	friend class IOCPOverlappedSend;

	friend class SessionHelper;
};

using SessionPtr = JCore::SharedPtr<Session>;

NS_JNET_END