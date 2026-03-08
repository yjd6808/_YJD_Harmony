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
void IOCPOverlappedRecv::Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker)
{
	(void)_pCompletionKey;

	const SOCKET receiveSocket = receivedSession_->SocketHandle();
	_u32 errorCode = 0;
	if (IsFailed(receiveSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		receivedSession_->Disconnect();
		receivedSession_->Disconnected(errorCode);
		return;
	}

	receivedSession_->Received(_bytesTransferred);

	if (!receivedSession_->RecvAsync())
	{
		errorCode = Winsock::LastError();

		receivedSession_->Disconnect();
		receivedSession_->Disconnected(errorCode);
	}
}

NS_END
