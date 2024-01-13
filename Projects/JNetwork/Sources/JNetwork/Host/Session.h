/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션/클라 들의 공통기능 정의
 */

#pragma once


#include <JCore/Sync/RecursiveLock.h>

#include <JNetwork/Host/Host.h>
#include <JNetwork/Host/ISessionContainer.h>

#include <JNetwork/EventListener/ServerEventListener.h>

#include <JNetwork/Packet/Packet.h>
#include <JNetwork/Packet/PacketParser.h>

NS_JNET_BEGIN

class IOCPOverlapped;
class JCORE_NOVTABLE Session : public Host
{
public:
	Session(const IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator, PacketParser* packetParser, int recvBufferSize, int sendBufferSize);
	~Session() override;

	const IPv4EndPoint& GetLocalEndPoint() const { return m_LocalEndPoint; }
	const IPv4EndPoint& GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	CommandBufferPtr GetRecvBuffer() { return m_spRecvBuffer; }
	CommandBufferPtr GetSendBuffer() { return m_spSendBuffer; }
	JCore::MemoryPoolAbstractPtr GetBufferAllocator() { return m_spBufferAllocator; }

	void Initialize() override;
	bool Bind(const IPv4EndPoint& bindAddr);
	bool Disconnect();

	int Send(char* data, int len);
	int Send(IPacket* packet, bool releasePacket = true);
	int SendPending(JCORE_REF_OUT Int32U& errorCode);
	void PushPendingData(char* data, int len);
	bool HasPendingData() const { return m_PendingData.Size() > 0; }
	bool PendingDataSize() const { return m_PendingData.Size(); }

	bool SendAsync(IPacket* packet);
	bool SendAsync(const CommandBufferPtr& buffer);
	bool SendToAsync(IPacket* packet);
	bool SendToAsync(IPacket* packet, const IPv4EndPoint& destination);
	bool SendToAsync(const CommandBufferPtr& buffer, const IPv4EndPoint& destination);

	bool RecvAsync();
	bool RecvFromAsync();

	void SendAlloc(ICommand* cmd);
	template <typename TCommand>
	TCommand& SendAlloc(int count = 0) {
		JCORE_LOCK_GUARD(m_SendBufferLock);
		CMD_CHECK_BASE_OF_COMMAND(TCommand)
		DYNAMIC_CMD_CHECK_ZERO_SIZE_ARRAY_FIELD(TCommand)

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
	virtual void Disconnected(Int32U errorCode) = 0;
	virtual void Received(Int32UL receivedBytes);
	virtual void Sent(IPacket* sentPacket, Int32UL receivedBytes) = 0;

	virtual void NotifyRaw(char* data, int len) = 0;
	virtual void NotifyCommand(ICommand* cmd) = 0;
	virtual void NotifyPacket(RecvedCommandPacket* packet) = 0;

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
	JCore::Vector<char> m_PendingData;

	PacketParser* m_pPacketParser;

	CommandBufferPtr m_spRecvBuffer;
	CommandBufferPtr m_spSendBuffer;

	IPv4EndPoint m_LocalEndPoint;
	IPv4EndPoint m_RemoteEndPoint;
};

using SessionPtr = JCore::SharedPtr<Session>;

NS_JNET_END