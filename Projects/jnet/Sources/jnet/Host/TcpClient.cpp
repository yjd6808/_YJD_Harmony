/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>

#include <jnet/Host/TcpClient.h>

#include <jnet/IOCPOverlapped/IOCPOverlappedConnect.h>

USING_NS_JC;

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
TcpClient::TcpClient(
	const IOCPPtr& _pIocp,
	const MemoryPoolAbstractPtr& _pBufferAllocator,
	PacketParser* _pParser,
	int _sendBufferSize,
	int _recvBufferSize)
	: Session(_pIocp, _pBufferAllocator, _pParser, _recvBufferSize, _sendBufferSize)
	, pEventListener_(nullptr)
{
	TcpClient::Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
TcpClient::~TcpClient()
{
	Disconnect();
	WaitForZeroPending();
	JC_DELETE_SAFE(pEventListener_);
}

//////////////////////////////////////////////////////////////////////////////////////////
static StaticPacket<GenericCommand<int>, GenericCommand<int>>* GenerateTestDummyPacket()
{
	auto* pDummyPacket = dbg_new StaticPacket<GenericCommand<int>, GenericCommand<int>>;
	pDummyPacket->Get<0>()->SetId(1);
	pDummyPacket->Get<0>()->value_ = 2;
	pDummyPacket->Get<1>()->SetId(3);
	pDummyPacket->Get<1>()->value_ = 4;
	pDummyPacket->AddRef();
	return pDummyPacket;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::Initialize()
{
	Session::Initialize();

	if (!CreateSocket(TransportProtocol::TCP, NonblokingSocket))
	{
		jc_assert_msg(false, "TCP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}

	if (!ConnectIocp())
	{
		jc_assert_msg(false, "IOCP에 연결하는데 실패했습니다. (%u)", Winsock::LastError());
	}
}

// https://stackoverflow.com/questions/46045434/winsock-c-connect-timeout
// select로 타임아웃 연결 구현가능
//////////////////////////////////////////////////////////////////////////////////////////
bool TcpClient::Connect(const IPv4EndPoint& _remoteEndPoint, int _timeoutMilliseconds)
{
	if (!socket_.IsValid())
	{
		_NetLogError_("연결에 실패했습니다. INVALID_SOCKET 입니다.");

		if (pEventListener_)
		{
			pEventListener_->OnConnectFailed(this, WSA_INVALID_HANDLE);
		}

		return false;
	}

	if (socket_.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR)
	{
		const Int32U errorCode = Winsock::LastError();

		if (pEventListener_)
		{
			pEventListener_->OnConnectFailed(this, errorCode);
		}

		_NetLogError_("연결에 실패했습니다. 논블로킹 소켓 전환실패 (%u)", errorCode);
		return false;
	}

	if (socket_.Connect(_remoteEndPoint) == SOCKET_ERROR)
	{
		const Int32U errorCode = Winsock::LastError();

		if (errorCode != WSAEWOULDBLOCK)
		{
			_NetLogError_("연결에 실패했습니다. (%u)", errorCode);

			if (pEventListener_)
			{
				pEventListener_->OnConnectFailed(this, errorCode);
			}

			return false;
		}

		fd_set writeSet, exceptionSet;

		FD_ZERO(&writeSet);
		FD_ZERO(&exceptionSet);

		FD_SET(socket_.Handle, &writeSet);
		FD_SET(socket_.Handle, &exceptionSet);

		timeval timeout;
		timeout.tv_sec = _timeoutMilliseconds / 1000;
		timeout.tv_usec = (_timeoutMilliseconds % 1000) * 1000;

		const int selectResult = select(0, nullptr, &writeSet, &exceptionSet, _timeoutMilliseconds == 0 ? nullptr : &timeout);
		if (selectResult <= 0)
		{
			// 이때 연결을 시도 중(10037)일 수도 있지만 시간이 지났으므로 타임아웃 처리
			socket_.Close();
			state_ = eDisconnected;
			Initialize();
			WSASetLastError(WSAETIMEDOUT);

			if (pEventListener_)
			{
				pEventListener_->OnConnectFailed(this, WSAETIMEDOUT);
			}

			return false;
		}

		if (FD_ISSET(socket_.Handle, &exceptionSet))
		{
			const int errorCode = socket_.Option().GetErrorCode();
			socket_.Close();
			state_ = eDisconnected;
			Initialize();
			WSASetLastError(errorCode);

			if (pEventListener_)
			{
				pEventListener_->OnConnectFailed(this, errorCode);
			}

			return false;
		}
	}

	remoteEndPoint_ = _remoteEndPoint;
	Connected();
#if TEST_DUMMY_PACKET_TRANSFER
	// 연결 후 곧장 데이터 전송 테스트
	if (SendAsync(GenerateTestDummyPacket()))
	{
	}
#endif

	return RecvAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TcpClient::ConnectAsync(const IPv4EndPoint& _destination)
{
	// 초기화된 상태에서만 연결을 진행할 수 있습니다.
	if (state_ != eInitailized)
	{
		if (pEventListener_)
		{
			pEventListener_->OnConnectFailed(this, WSANOTINITIALISED);
		}

		return false;
	}

	state_ = eConnectWait;

	if (!iocpConnected_)
	{
		jc_assert_msg(false, "IOCP와 연결해주세요.");

		if (pEventListener_)
		{
			pEventListener_->OnConnectFailed(this, WSANOTINITIALISED);
		}

		return false;
	}

	// ConnectEx를 사용하기 위해서 클라이언트더라도 바인딩을 해줘야한다.
	if (!socket_.IsBinded() && !Bind({}))
	{
		if (pEventListener_)
		{
			pEventListener_->OnConnectFailed(this, Winsock::LastError());
		}

		return false;
	}

	// 연결 후 곧장 데이터 전송 테스트
	// 패킷은 모두 오버랩 Process에서 해제하도록 한다.
	Int32UL sentBytes = 0;
	auto pDummyPacket =
#if TEST_DUMMY_PACKET_TRANSFER
		GenerateTestDummyPacket();
#else
		nullptr;
#endif

	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedConnect(this, pIocp_.GetPtr(), pDummyPacket);
	if (socket_.ConnectEx(_destination, pOverlapped,
#if TEST_DUMMY_PACKET_TRANSFER
		pDummyPacket->GetWSABuf().buf,
		TEST_DUMMY_PACKET_SIZE,
#else
		nullptr,
		0,
#endif
		&sentBytes) == FALSE)
	{
		const Int32U errorCode = Winsock::LastError();
		if (errorCode != WSA_IO_PENDING)
		{
			jc_assert_msg(false, "서버 접속에 실패하였습니다. (%u)", errorCode);
			Disconnect();
			pOverlapped->Release();

			if (pEventListener_)
			{
				pEventListener_->OnConnectFailed(this, errorCode);
			}

			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::Disconnected(Int32U _errorCode)
{
	if (pEventListener_)
	{
		pEventListener_->OnDisconnected(this, _errorCode);
	}

	Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::NotifyCommand(ICommand* _pCmd)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceived(this, _pCmd);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::NotifyPacket(RecvedCommandPacket* _pPacket)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceived(this, _pPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::NotifyRaw(char* _pData, int _len)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceivedRaw(this, _pData, _len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::Sent(IPacket* _pSentPacket, Int32UL _sentBytes)
{
	if (pEventListener_)
	{
		pEventListener_->OnSent(this, _pSentPacket, _sentBytes);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::Connected()
{
	state_ = eConnected;

	// 일정주기마다 "나 살아있소" 전송
	if (socket_.Option().SetKeepAliveEnabled(true) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "클라이언트 소켓 Keep Alive 활성화 실패");
	}

	// 빠른 반응을 위해 Nagle 알고리즘을 꺼준다.
	if (socket_.Option().SetNagleEnabled(false) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "클라이언트 소켓 Nagle 비활성화 실패");
	}

	// 클라이언트는 린저를 꺼주자.
	// 송신 버퍼에 있는 데이터를 모두 보내고 안전하게 종료할 수 있도록
	if (socket_.Option().SetLingerEnabled(false) == SOCKET_ERROR)
	{
		jc_assert_msg(false, "클라이언트 소켓 린저 타임아웃 설정 실패");
	}

	if (pEventListener_)
	{
		pEventListener_->OnConnected(this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClient::ConnectFailed(Int32U _errorCode)
{
	if (pEventListener_)
	{
		pEventListener_->OnConnectFailed(this, _errorCode);
	}

	Initialize();
}

NS_JNET_END
