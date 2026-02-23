/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>

#include <jnet/Host/TcpServer.h>
#include <jnet/Host/TcpSession.h>
#include <jnet/Host/SessionContainer.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
TcpServer::TcpServer(const IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator)
: Server(_pIocp)
, pBufferAllocator_(_pBufferAllocator)
, pServerEventListener_(nullptr)
, pSessionContainer_(nullptr)
{
	TcpServer::Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
TcpServer::~TcpServer()
{
	TcpServer::Stop();

	JC_DELETE_SAFE(pSessionContainer_);
	JC_DELETE_SAFE(pServerEventListener_);
}

//////////////////////////////////////////////////////////////////////////////////////////
TcpSession* TcpServer::CreateSession()
{
	return dbg_new TcpSession(this, pIocp_, pBufferAllocator_, nullptr, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
SessionContainer* TcpServer::CreateSessionContainer()
{
	// 디폴트 세션 컨테이너, 서버시작전 외부에서 주입해줄 경우 호출안됨
	return dbg_new SessionContainer(10);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionDisconnected(TcpSession* _pSession, _u32 _errorCode)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnDisconnected(_pSession, _errorCode);
	}

	// 세션 재사용... 이거땜에 State를 Atomic으로 변경함.
	// 서버가 다른 쓰레드에서 Stop을 실행하는 순간
	// IOCP 쓰레드들이 서버의 State를 정확하게 관측하도록 하기위함.
	// 이렇게 체크한번만 해주면 쓰레드 세이프하게 재사용할 수 있다. (맞겠지?)
	if (state_ != eListening)
	{
		_NetLogDebug_("IOCP 서버가 리스닝 상태가 아닙니다. 세션 재사용을 하지 않습니다.");
		return;
	}

	_pSession->Initialize();
	_pSession->AcceptWait();

	if (!_pSession->AcceptAsync())
	{
		_NetLogDebug_("세션을 재사용 실패");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionConnected(TcpSession* _pSession)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnConnected(_pSession);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionConnectFailed(TcpSession* _pSession, _u32 _errorCode)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnConnectFailed(_pSession, _errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionSent(TcpSession* _pSession, IPacket* _pSentPacket, _u32l _receivedBytes)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnSent(_pSession, _pSentPacket, _receivedBytes);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionReceived(TcpSession* _pSession, ICommand* _pCommand)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnReceived(_pSession, _pCommand);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionReceived(TcpSession* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnReceived(_pSession, _pRecvPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SessionReceivedRaw(TcpSession* _pSession, char* _pData, int _len)
{
	if (pServerEventListener_)
	{
		pServerEventListener_->OnReceivedRaw(_pSession, _pData, _len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SessionContainer* TcpServer::GetSessionContainer()
{
	JC_LOCK_GUARD(lock_);
	return pSessionContainer_;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerEventListener* TcpServer::GetEventListener()
{
	JC_LOCK_GUARD(lock_);
	return pServerEventListener_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SetSesssionContainer(SessionContainer* _pContainer)
{
	JC_LOCK_GUARD(lock_);
	pSessionContainer_ = _pContainer;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::SetEventListener(ServerEventListener* _pListener)
{
	JC_LOCK_GUARD(lock_);
	pServerEventListener_ = _pListener;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::Initialize()
{
	JC_LOCK_GUARD(lock_);

	if (!CreateSocket(TransportProtocol::TCP))
	{
		_NetLogError_("TCP 서버 소켓 생성 실패");
		return;
	}

	if (!ConnectIocp())
	{
		_NetLogError_("TCP 서버 IOCP 연결 실패");
		return;
	}

	state_ = eInitailized;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TcpServer::Start(const IPv4EndPoint& _localEndPoint)
{
	struct StartFailNotifyGuard
	{
		explicit StartFailNotifyGuard(TcpServer* _pServer)
		: errorCode_(0)
		, server_(_pServer)
		{
		}

		~StartFailNotifyGuard() noexcept
		{
			if (errorCode_ == 0)
			{
				return;
			}

			if (server_->pServerEventListener_)
			{
				server_->pServerEventListener_->OnStartFailed(errorCode_);
			}

			server_->OnStartFailed(errorCode_);
		}

		_u32 errorCode_;
		TcpServer* server_;
	} notifier{this};

	JC_LOCK_GUARD(lock_);

	if (state_ != eInitailized)
	{
		_NetLogError_("서버가 초기화 상태여야 시작할 수 있습니다.");
		notifier.errorCode_ = WSANOTINITIALISED;
		return false;
	}

	if (socket_.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR)
	{
		_NetLogWarn_("서버 소켓 SetReuseAddrEnabled(true) 실패");
	}

	if (socket_.Bind(_localEndPoint) == SOCKET_ERROR)
	{
		_NetLogError_("%s %s %s 바인드 실패 (%u)", TypeName(), _localEndPoint.ToString().Source(), socket_.ProtocolName(),
		              Winsock::LastError());
		notifier.errorCode_ = Winsock::LastError();
		return false;
	}
	_NetLogDebug_("%s %s %s 바인드 완료", TypeName(), _localEndPoint.ToString().Source(), socket_.ProtocolName());

	if (socket_.Listen() == SOCKET_ERROR)
	{
		_NetLogError_("서버 소켓 리슨 실패 (%d)", Winsock::LastError());
		notifier.errorCode_ = Winsock::LastError();
		return false;
	}

	if (!pSessionContainer_)
	{
		pSessionContainer_ = CreateSessionContainer();
	}

	pSessionContainer_->ResetHandleSeq();
	pSessionContainer_->Clear();
	const int maxConnection = pSessionContainer_->Capacity();

	// 세션을 미리 생성해놓고 연결 대기 상태로 둠
	for (int index = 0; index < maxConnection; ++index)
	{
		TcpSession* pSession = CreateSession();

		pSession->OnCreated();
		pSession->SetHandle(pSessionContainer_->CreateHandle());
		pSession->AcceptWait();

		if (!pSession->AcceptAsync())
		{
			notifier.errorCode_ = Winsock::LastError();
			pSessionContainer_->DisconnectAll();
			pSessionContainer_->Clear();
			return false;
		}
		pSessionContainer_->Add(pSession);
	}

	if (pServerEventListener_)
	{
		pServerEventListener_->OnStarted();
	}

	OnStarted();
	return state_ = eListening;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool TcpServer::Stop()
{
	JC_LOCK_GUARD(lock_);

	if (state_ == eStopped)
	{
		return true;
	}

	state_ = eStopped;

	// 강종 진행: GetQueuedCompletionStatus에서 995번에러를 뱉음(I / O operation has been aborted)
	pSessionContainer_->DisconnectAll();

	if (socket_.Close() == SOCKET_ERROR)
	{
		_NetLogError_("서버 소켓을 닫는데 실패했습니다. (%d)", Winsock::LastError());
	}

	socket_.Invalidate();

	// 동적할당된 세션들을 모두 해제해주자.
	pSessionContainer_->Clear();

	if (pServerEventListener_)
	{
		pServerEventListener_->OnStopped();
	}

	OnStopped();
	return true;
}

NS_END
