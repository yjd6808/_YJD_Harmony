/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>
#include <jnet/Config.h>

#include <jnet/Host/Session.h>
#include <jnet/Buffer/CommandBuffer.h>

#include <jnet/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedSend.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

#include <jc/Primitives/RefCountObjectPtr.h>

USING_NS_JC;

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
Session::Session(
const IOCPPtr& _pIocp,
const MemoryPoolAbstractPtr& _pBufferAllocator,
PacketParser* _pPacketParser,
int _recvBufferSize,
int _sendBufferSize)
//////////////////////////////////////////////////////////////////////////////////////////
: Host(_pIocp)
, handle_(-1)
, bufferAllocator_(_pBufferAllocator)
, pendingData_(0) // Lazy-Allocation
, packetParser_(_pPacketParser)
, recvBuffer_(nullptr)
, sendBuffer_(nullptr)
{
	recvBuffer_ = MakeShared<CommandBuffer>(bufferAllocator_,_recvBufferSize <= 0 ? DefaultRecvBufferSize : _recvBufferSize);
	sendBuffer_ = MakeShared<CommandBuffer>(bufferAllocator_,_sendBufferSize <= 0 ? DefaultSendBufferSize : _sendBufferSize);

	if (packetParser_ == nullptr)
	{
		packetParser_ = PacketParser::Create(DefaultParserType, this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Session::~Session()
{
	JC_DELETE_SAFE(packetParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::Initialize()
{
	const int currentState = state_.Load();

	if (currentState == eConnectWait || currentState == eConnected)
	{
		_NetLogWarn_("초기화 되지 않았거나 혹은 연결이 끊긴 대상만 초기화를 진행할 수 있습니다.");
		return;
	}

	localEndPoint_ = {};
	remoteEndPoint_ = {};

	recvBuffer_->MoveReadPos(0);
	recvBuffer_->MoveWritePos(0);

	sendBuffer_->MoveReadPos(0);
	sendBuffer_->MoveWritePos(0);

	state_ = eInitailized;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::Bind(const IPv4EndPoint& _bindEndPoint)
{
	if (!socket_.IsValid())
	{
		_NetLogError_("바인드에 실패했습니다. INVALID_SOCKET 입니다.");
		WSASetLastError(WSA_INVALID_HANDLE);
		return false;
	}

	const int bindResult = socket_.Bind(_bindEndPoint);
	if (bindResult == SOCKET_ERROR)
	{
		_NetLogError_("%s %s 바인드 실패 (%u)", TypeName(), _bindEndPoint.ToString().Source(), Winsock::LastError());
		return false;
	}

	_NetLogDebug_("%s %s 바인드 완료", TypeName(), _bindEndPoint.ToString().Source());
	localEndPoint_ = _bindEndPoint;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::Disconnect()
{
	const int previousState = state_;

	if (previousState == eDisconnected)
	{
		return true;
	}

	state_ = eDisconnected;
	iocpConnected_ = false;

	// 굳이 오류 처리를 할 필요가 있나
	// WSAENOTSOCK: 소켓 할당이 안된 
	// WSAENOTCONN: 연결안된 소켓

	socket_.ShutdownBoth();
	socket_.Close();
	socket_.Invalidate();

	// 여기서 Disconnected를 호춡토록 구현했었는데 잘못된 로직이었다.
	// 예를들어서 TcpClient가 Connect 시도중 Disconnect를 시도하면
	// Connect 오버랩이 실패하면서 한번더 Disconnect를 호출하게되어버림.

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Session::Send(char* _pData, int _len)
{
	Int32U sendPendingErrorCode = 0;
	int sent = SendPending(sendPendingErrorCode);
	const int pendingSent = sent;

	if (sendPendingErrorCode != 0)
	{
		PushPendingData(_pData, _len);
		return sent;
	}

	if (HasPendingData())
	{
		PushPendingData(_pData, _len);
		return sent;
	}

	for (;;)
	{
		const int sendResult = socket_.Send(_pData, _len);

		if (sendResult == SOCKET_ERROR)
		{
			const Int32U errorCode = Winsock::LastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				PushPendingData(_pData, _len);
			}
			else
			{
				_NetLogError_("Send 실패 (%u)", errorCode);
			}
			break;
		}

		_pData += sendResult;
		_len -= sendResult;
		sent += sendResult;

		// 다 보낸 경우 나가도록
		if (sent - pendingSent >= _len)
		{
			break;
		}
	}

	return sent;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Session::Send(IPacket* _pPacket, bool _releasePacket)
{
	const WSABUF data = _pPacket->GetWSABuf();
	const int sent = Send(data.buf, data.len);

	if (_releasePacket)
	{
		JC_RELEASE_SAFE(_pPacket);
	}

	return sent;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Session::SendPending(OUT Int32U& _errorCode)
{
	char* pPendingData = pendingData_.Source();
	int pendingDataCount = pendingData_.Size(); // 보내야하는 바이트 수

	if (pendingDataCount == 0)
	{
		return 0;
	}

	for (;;)
	{
		const int sendResult = socket_.Send(pPendingData, pendingDataCount);
		if (sendResult == SOCKET_ERROR)
		{
			_errorCode = Winsock::LastError();
			if (_errorCode != WSAEWOULDBLOCK)
			{
				_NetLogError_("SendPending 실패 (%u)", _errorCode);
			}
			break;
		}
		pPendingData += sendResult;
		pendingDataCount -= sendResult;

		// 다 보낸 경우 나가도록
		if (pendingDataCount <= 0)
		{
			break;
		}
	}

	const int sent = pendingData_.Size() - pendingDataCount;
	pendingData_.PopFront(sent);
	return sent;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::PushPendingData(char* _pData, int _len)
{
	if (pendingData_.Source() == nullptr)
	{
		pendingData_.Reserve(_len + 1);
	}

	pendingData_.PushBack(_pData, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::SendAsync(IPacket* _pPacket)
{
	_pPacket->AddRef();
	WSABUF buf = _pPacket->GetWSABuf();
	Int32UL sendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSend(this, pIocp_.GetPtr(), _pPacket);

	// 0으로 즉시 성공하더라도 IOCP 워커에서 오버랩이 처리되므로 여기서 삭제를 해줄필요가 없다.
	const int sendResult = socket_.SendEx(&buf, &sendBytes, pOverlapped);

	if (sendResult == SOCKET_ERROR)
	{
		Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_NetLogError_("SendAsync 실패 (%u)", errorCode);
			_pPacket->Release();
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::SendAsync(const CommandBufferPtr& _pBuffer)
{
#ifdef DebugMode
	jc_assert_msg(_pBuffer->IsValid(), "보내고자하는 커맨드 버퍼 데이터가 이상합니다.");
#endif
	return SendAsync(dbg_new CommandBufferPacket(_pBuffer));
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBufferPacket* Session::GetCommandBufferForSending()
{
	JC_LOCK_GUARD(sendBufferLock_);

	if (sendBuffer_->GetCommandCount() == 0)
	{
		return nullptr;
	}

	CommandBufferPtr pNewSendBuffer = MakeShared<CommandBuffer>(bufferAllocator_, sendBuffer_->GetBufferRequestSize());
	CommandBufferPtr pOldSendBuffer = sendBuffer_;

	sendBuffer_ = pNewSendBuffer;

	CommandBufferPacket* pWrappedPacket = dbg_new CommandBufferPacket(pOldSendBuffer);

#ifdef DebugMode
	if (!pOldSendBuffer->IsValid())
	{
		delete pWrappedPacket;
		_NetLogError_("무야! 보내고자하는 커맨드 센드 버퍼 데이터가 이상합니다.");
		return nullptr;
	}
#endif
	return pWrappedPacket;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::FlushSendBuffer()
{
	CommandBufferPacket* pWrappedPacket = GetCommandBufferForSending();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pWrappedPacket);
	if (pWrappedPacket)
	{
		SendAsync(pWrappedPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::SendToAsync(IPacket* _pPacket, const IPv4EndPoint& _destination)
{
	if (!_destination.IsValidRemoteEndPoint())
	{
		_NetLogError_("유효한 목적지 주소가 아닙니다.");
		return false;
	}

	_pPacket->AddRef();
	WSABUF buf = _pPacket->GetWSABuf();
	Int32UL sendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSendTo(this, pIocp_.GetPtr(), _pPacket);

	const int result = socket_.SendToEx(&buf, &sendBytes, pOverlapped, _destination);
	if (result == SOCKET_ERROR)
	{
		Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			jc_assert_msg(false, "SendToAsync 실패 (%d)", errorCode);
			_pPacket->Release();
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::SendToAsync(const CommandBufferPtr& _pBuffer, const IPv4EndPoint& _destination)
{
#ifdef DebugMode
	jc_assert_msg(_pBuffer->IsValid(), "보내고자하는 커맨드 버퍼 데이터가 이상합니다.");
#endif
	auto pPacket = dbg_new CommandBufferPacket(_pBuffer);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	return SendToAsync(pPacket, _destination);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::SendToAsync(IPacket* _pPacket)
{
	return SendToAsync(_pPacket, remoteEndPoint_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::RecvAsync()
{
	WSABUF buf = recvBuffer_->GetRemainBuffer();
	Int32UL receivedBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedRecv(this, pIocp_.GetPtr());

	const int result = socket_.ReceiveEx(&buf, &receivedBytes, pOverlapped);

	if (result == SOCKET_ERROR)
	{
		Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_NetLogError_("RecvAsync 실패 (%u)", errorCode);
			pOverlapped->Release();
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Session::RecvFromAsync()
{
	if (!socket_.IsBinded())
	{
		_NetLogError_("소켓이 바인딩된 상태여야 수신이 가능합니다. 상대방에게 먼저 송신하여 오토 바인딩해주거나 수동 바인딩을 해주세요.");
		return false;
	}

	WSABUF buf = recvBuffer_->GetRemainBuffer();
	Int32UL receivedBytes = 0;
	IOCPOverlappedRecvFrom* pRecvFromOverlapped = dbg_new IOCPOverlappedRecvFrom(this, pIocp_.GetPtr());

	const int result = socket_.ReceiveFromEx(
		&buf,
		&receivedBytes,
		pRecvFromOverlapped,
		&remoteEndPoint_.InternetAddr);

	if (result == SOCKET_ERROR)
	{
		Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			jc_assert_msg(false, "RecvFromAsync 실패 (%d)", errorCode);
			pRecvFromOverlapped->Release();
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::SendAlloc(ICommand* _pCmd)
{
	JC_LOCK_GUARD(sendBufferLock_);

	const int cmdSize = _pCmd->GetLength();
	if (sendBuffer_->GetWritePos() + cmdSize >= MAX_MSS)
	{
		FlushSendBuffer();
	}

	jc_assert_msg(
		cmdSize <= sendBuffer_->GetRemainBufferSize(),
		"버퍼의 남은 공간에 넣을 커맨드가 너무 큽니다. (CmdSize: %d, RemainBufferCapacity: %d)",
		cmdSize,
		sendBuffer_->GetRemainBufferSize());
	sendBuffer_->Alloc(_pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::Received(Int32UL _receivedBytes)
{
	// 0 byte를 수신하는 경우
	// Page-Lock 방지를 위한 0 byte가 들어올 수 있음 (TODO: 아직 0 byte recv는 구현안함)
	// 상대 클라이언트 Disconnect시 들어올 수 있음
	if (_receivedBytes == 0)
	{
		return;
	}

	packetParser_->Received(_receivedBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Session::WaitForZeroPending()
{
	int previousPendingCount = 0;
	int equalCount = 0;

	while (true)
	{
		const int pending = overlappedPendingCount_;

		if (pending == 0)
		{
			break;
		}

		if (pending < 0)
		{
			_NetLogWarn_("멍미 펜딩 카운트가 움수 인뎁쇼 (%d)", pending);
			break;
		}

		if (previousPendingCount == pending)
		{
			equalCount++;
		}

		if (equalCount >= 1'000'000)
		{
			equalCount = 0;
			_NetLogWarn_("펜딩 카운트 기달 %d", pending);
		}

		previousPendingCount = pending;
	}
}

NS_JNET_END
