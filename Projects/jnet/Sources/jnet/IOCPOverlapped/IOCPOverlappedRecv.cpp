/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>

#include <jnet/IOCPOverlapped/IOCPOverlappedRecv.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedRecv::IOCPOverlappedRecv(Session* _pSession, IOCP* _pIocp)
: IOCPOverlapped(_pIocp, Type::Receive)
, receivedSession_(_pSession)
{
	receivedSession_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedRecv::~IOCPOverlappedRecv()
{
	receivedSession_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedRecv::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	const SOCKET receiveSocket = receivedSession_->SocketHandle();
	Int32U errorCode = 0;
	if (IsFailed(receiveSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		receivedSession_->Disconnect();
		receivedSession_->Disconnected(errorCode);
		return;
	}

	receivedSession_->Received(_bytesTransferred);

	// TODO: 리시브 오버랩 재사용 기능 구현
	if (!receivedSession_->RecvAsync())
	{
		errorCode = Winsock::LastError();

		receivedSession_->Disconnect();
		receivedSession_->Disconnected(errorCode);
	}
}

NS_END
