/*
 * 작성자 : 윤정도
 * 생성일: 2/8/2023
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Winsock.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedAccept::IOCPOverlappedAccept(TcpSession* _pSession, IOCP* _pIocp)
: IOCPOverlapped(_pIocp, Type::Accept)
, acceptedSession_(_pSession)
{
	acceptedSession_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedAccept::~IOCPOverlappedAccept()
{
	acceptedSession_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedAccept::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	const SOCKET acceptedSocket = acceptedSession_->SocketHandle();
	Int32U errorCode = 0;
	if (IsFailed(acceptedSocket, _result, _bytesTransferred, errorCode))
	{
		acceptedSession_->Disconnect();
		acceptedSession_->ConnectFailed(errorCode);
		return;
	}

	if (acceptedSession_->Accepted(_bytesTransferred))
	{
		// https://learn.microsoft.com/en-us/windows/win32/fileio/createiocompletionport
		if (!acceptedSession_->ConnectIocp())
		{
			acceptedSession_->Disconnect();
			acceptedSession_->ConnectFailed(::GetLastError()); // 오류코드: CreateIoCompletionPort() 함수 실패시
			return;
		}

		if (!acceptedSession_->RecvAsync())
		{
			acceptedSession_->Disconnect();
			acceptedSession_->ConnectFailed(Winsock::LastError()); // 오류코드: WSARecv() 함수 실패시
			return;
		}

		acceptedSession_->Connected();
	}
	else
	{
		// Disconnect시에 오류코드가 덮어질 우려가 있으므로 미리받음.
		errorCode = Winsock::LastError();

		acceptedSession_->Disconnect();
		acceptedSession_->ConnectFailed(errorCode); // 오류코드: SetUpdateAcceptContext 실패시
	}
}

NS_JNET_END
