/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:26:19 AM
 * =====================
 *
 */
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpServer.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
TcpSession::TcpSession(
	TcpServer* _pServer,
	const IOCPPtr& _pIocp,
	const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
	PacketParser* _pParser,
	int _recvBufferSize,
	int _sendBufferSize)
: Session(_pIocp, _pBufferAllocator, _pParser, _recvBufferSize, _sendBufferSize)
, pServer_(_pServer)
{
	TcpSession::Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::Initialize()
{
	Session::Initialize();

	if (!CreateSocket(TransportProtocol::TCP, NonblokingSocket))
	{
		DebugAssertMsg(false, "TCP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TcpSession::AcceptAsync()
{
	Int32UL receivedBytes = 0;

	SOCKET listeningSocket = pServer_->SocketHandle();
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedAccept(this, iocp_.GetPtr());

	if (socket_.AcceptEx(
		listeningSocket,
		recvBuffer_->GetRemainBuffer().buf,
#if TEST_DUMMY_PACKET_TRANSFER
		TEST_DUMMY_PACKET_SIZE, // TcpClient에서 테스트 더미 패킷을 보내기 땜에 8로 세팅
#else
		0,
#endif
		&receivedBytes,
		pOverlapped) == FALSE)
	{
		Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_NetLogWarn_("세션 AcceptEx 실패 (%d:%s)", errorCode, Winsock::LastErrorMessageUTF8().Source());
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::AcceptWait()
{
	state_ = eAcceptWait;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TcpSession::Accepted(Int32UL _receivedBytes)
{
	(void)_receivedBytes;

	// AcceptEx 함수  호출 후 연결된 소켓에 대해서 로컬 주소와 리모트 주소를 가져올 수 있도록 업데이트 해준다.
	// 이걸 실행하지 않으면 해당 소켓에 바인딩된 로컬 주소와 리모트 주소를 못가져옴
	//    = getsockname(), getpeername() 안먹힘
	if (socket_.Option().SetUpdateAcceptContext(pServer_->SocketHandle()) == SOCKET_ERROR)
	{
		return false;
	}

	char* pReads = recvBuffer_->Peek<char*>();
#if TEST_DUMMY_PACKET_TRANSFER
	socket_.AcceptExResult(pReads, TEST_DUMMY_PACKET_SIZE, &localEndPoint_, &remoteEndPoint_);
#else
	socket_.AcceptExResult(pReads, 0, &localEndPoint_, &remoteEndPoint_);
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::NotifyCommand(ICommand* _pCmd)
{
	pServer_->SessionReceived(this, _pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::NotifyPacket(RecvedCommandPacket* _pRecvPacket)
{
	pServer_->SessionReceived(this, _pRecvPacket);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::NotifyRaw(char* _pData, int _len)
{
	pServer_->SessionReceivedRaw(this, _pData, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::Connected()
{
	state_ = eAccepted;
	OnConnected();
	pServer_->SessionConnected(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::ConnectFailed(Int32U _errorCode)
{
	pServer_->SessionConnectFailed(this, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::Disconnected(Int32U _errorCode)
{
	OnDisconnected();
	pServer_->SessionDisconnected(this, _errorCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpSession::Sent(IPacket* _pSentPacket, Int32UL _receivedBytes)
{
	pServer_->SessionSent(this, _pSentPacket, _receivedBytes);
}

NS_JNET_END
