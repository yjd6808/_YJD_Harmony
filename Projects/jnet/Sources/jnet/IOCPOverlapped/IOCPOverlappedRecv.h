/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jnet/Host/Session.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedRecv : public IOCPOverlapped
{
public:
	IOCPOverlappedRecv(Session* _pSession, IOCP* _pIocp);
	~IOCPOverlappedRecv() override;

public:
	void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	Session* receivedSession_;
};

NS_END
