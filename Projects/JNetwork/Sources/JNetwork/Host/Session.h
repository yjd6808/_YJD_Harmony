/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션/클라 들의 공통기능 정의
 */

#pragma once


#include <JCore/Sync/RecursiveLock.h>

#include <JNetwork/Host/Host.h>
#include <JNetwork/Host/ISessionContainer.h>

#include <JNetwork/EventListener/ServerEventListener.h>

#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Packet/RecvPacket.h>

NS_JNET_BEGIN

class IOCPOverlapped;
class JCORE_NOVTABLE Session : public Host
{
public:
	Session(const IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator, int recvBufferSize, int sendBufferSize);
	~Session() override;

	const IPv4EndPoint& GetLocalEndPoint() const { return m_LocalEndPoint; }
	const IPv4EndPoint& GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	CommandBufferPtr GetRecvBuffer() { return m_spRecvBuffer; }
	CommandBufferPtr GetSendBuffer() { return m_spSendBuffer; }
	JCore::MemoryPoolAbstractPtr GetBufferAllocator() { return m_spBufferAllocator; }

	void Initialize() override;
	bool Bind(const IPv4EndPoint& bindAddr);
	bool Disconnect();

	bool SendAsync(ISendPacket* packet);
	bool SendAsync(const CommandBufferPtr& buffer);
	bool SendToAsync(ISendPacket* packet);
	bool SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination);
	bool SendToAsync(const CommandBufferPtr& buffer, const IPv4EndPoint& destination);

	bool RecvAsync();
	bool RecvFromAsync();

	void SendAlloc(ICommand* cmd);
	template <typename TCommand>
	TCommand& SendAlloc(int count = 1) {
		JCORE_LOCK_GUARD(m_SendBufferLock);

		const int CmdSize = TCommand::_Size(count);
		if (m_spSendBuffer->GetWritePos() + CmdSize >= MAX_MSS) {
			FlushSendBuffer();
		}

		DebugAssertMsg(CmdSize <= m_spSendBuffer->GetRemainBufferSize(), "버퍼의 남은 공간에 넣을 커맨드가 너무 큽니다. (CmdSize: %d, RemainBufferCapacity: %d)", CmdSize, m_spSendBuffer->GetRemainBufferSize());
		return m_spSendBuffer->Alloc<TCommand>(count);
	}

	CommandBufferPacket* GetCommandBufferForSending();
	virtual void FlushSendBuffer();
	virtual void Connected() = 0;
	virtual void ConnectFailed(Int32U errorCode) = 0;
	virtual void Disconnected() = 0;	
	virtual void Received(Int32UL receivedBytes);						
	virtual void Sent(ISendPacket* sentPacket, Int32UL receivedBytes) = 0;
	virtual void NotifyCommand(ICommand* cmd) = 0;
	virtual void NotifyPacket(IRecvPacket* packet) = 0;

	int	 AddPendingCount()		{ return ++m_iOveralappedPendingCount;	}
	int  DecreasePendingCount()	{ return --m_iOveralappedPendingCount;	}
	int  GetPendingCount()		{ return m_iOveralappedPendingCount;	}
	void WaitForZeroPending();

	void SetHandle(int handle) { m_iHandle = handle; }
	int GetHandle() const { return m_iHandle; }
protected:
	int m_iHandle;

	JCore::AtomicInt m_iOveralappedPendingCount;
	JCore::MemoryPoolAbstractPtr m_spBufferAllocator;
	JCore::RecursiveLock m_SendBufferLock;

	CommandBufferPtr m_spSendBuffer;
	CommandBufferPtr m_spRecvBuffer;

	IPv4EndPoint m_LocalEndPoint;
	IPv4EndPoint m_RemoteEndPoint;
};

using SessionPtr = JCore::SharedPtr<Session>;

NS_JNET_END