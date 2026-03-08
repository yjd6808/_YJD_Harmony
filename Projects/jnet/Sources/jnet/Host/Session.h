/*
 *	작성자 : 윤정도
 *	TCP, UDP 세션/클라 들의 공통기능 정의
 */

#pragma once

#include <jc/Sync/RecursiveLock.h>

#include <jnet/Host/Host.h>

#include <jnet/EventListener/ServerEventListener.h>

#include <jnet/Packet/Packet.h>
#include <jnet/Packet/PacketParser.h>

NS_JNET_BEGIN

class IOCPOverlapped;

class JC_NOVTABLE Session : public Host
{
public:
	Session(const IOCPPtr& _pIocp,
	        const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
	        PacketParser* _pPacketParser,
	        int _recvBufferSize,
	        int _sendBufferSize);
	~Session() override;

	const IPv4EndPoint& GetLocalEndPoint() const { return localEndPoint_; }
	const IPv4EndPoint& GetRemoteEndPoint() const { return remoteEndPoint_; }
	PacketBufferPtr GetRecvBuffer() { return recvBuffer_; }
	PacketBufferPtr GetSendBuffer() { return sendBuffer_; }
	jc::MemoryPoolAbstractPtr GetBufferAllocator() { return bufferAllocator_; }

	void Initialize() override;
	bool Bind(const IPv4EndPoint& _bindEndPoint);
	bool Disconnect();

	int Send(char* _pData, int _len);
	int Send(IPacket* _pPacket, bool _releasePacket = true);
	int SendPending(OUT _u32& _errorCode);
	void PushPendingData(char* _pData, int _len);
	bool HasPendingData() const { return pendingData_.Size() > 0; }
	bool PendingDataSize() const { return pendingData_.Size(); }

	bool SendAsync(IPacket* _pPacket);
	bool SendAsync(const PacketBufferPtr& _pBuffer);
	bool SendToAsync(IPacket* _pPacket);
	bool SendToAsync(IPacket* _pPacket, const IPv4EndPoint& _destination);
	bool SendToAsync(const PacketBufferPtr& _pBuffer, const IPv4EndPoint& _destination);

	bool RecvAsync();
	bool RecvFromAsync();

	void EnqueueCmd(ICommand* _pCmd, bool _flushIfOverflow = true);
	jc::CMessage EnqueueMsg(bool _flushIfOverflow = true);

	template <typename TCommand>
	TCommand& EnqueueCmd(int _count = 0, bool _flushIfOverflow = true)
	{
		JC_LOCK_GUARD(sendBufferLock_);

		// 커맨드 유효성 검사
		CMD_CHECK_BASE_OF_COMMAND(TCommand)
		DYNAMIC_CMD_CHECK_ARRAY_FIELD(TCommand)

		PacketBuffer* pBuffer = sendBuffer_.GetPtr();

		const int cmdSize = TCommand::_Size(_count);
		if (_flushIfOverflow && pBuffer->GetWritePos() + cmdSize >= MAX_MSS)
		{
			FlushSendBuffer();
		}

		jc_assert_msg(
			cmdSize <= pBuffer->GetRemainBufferSize(),
			"버퍼의 남은 공간에 넣을 커맨드가 너무 큽니다. (CmdSize: %d, RemainBufferCapacity: %d)",
			cmdSize,
			pBuffer->GetRemainBufferSize());
		return pBuffer->EmplaceCmd<TCommand>(_count);
	}

	PacketBufferPacket* GetCommandBufferForSending();
	virtual void FlushSendBuffer();
	virtual void Connected() = 0;
	virtual void ConnectFailed(_u32 _errorCode) = 0;
	virtual void Disconnected(_u32 _errorCode) = 0;
	virtual void Received(_u32l _receivedBytes);
	virtual void Sent(IPacket* _pSentPacket, _u32l _receivedBytes) = 0;

	virtual void NotifyRaw(char* _pData, int _len) = 0;
	virtual void NotifyCommand(ICommand* _pCmd) = 0;
	virtual void NotifyPacket(RecvedPacket* _pPacket) = 0;
	virtual void NotifyMessage(jc::CMessage _msg) = 0;

	int  AddPendingCount()      { return ++overlappedPendingCount_; }
	int  DecreasePendingCount() { return --overlappedPendingCount_; }
	int  GetPendingCount()      { return overlappedPendingCount_; }
	void WaitForZeroPending();

protected:
	jc::AtomicInt overlappedPendingCount_;
	jc::MemoryPoolAbstractPtr bufferAllocator_;
	jc::RecursiveLock sendBufferLock_;
	jc::Vector<char> pendingData_;

	PacketParser packetParser_;
	PacketBufferPtr recvBuffer_;
	PacketBufferPtr sendBuffer_;

	IPv4EndPoint localEndPoint_;
	IPv4EndPoint remoteEndPoint_;
};

using SessionPtr = jc::SharedPtr<Session>;

NS_END
