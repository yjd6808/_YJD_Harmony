/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:27 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Host/UdpClient.h"
#include "jnet/IOCPOverlapped/IOCPOverlapped.h"

NS_JNET_BEGIN

class IOCPOverlappedRecvFrom : public IOCPOverlapped
#ifdef USE_OVERLAPPED_STATIC_POOL
	, public jc::ObjectPool<IOCPOverlappedRecvFrom>
#endif
	
{
public:
	IOCPOverlappedRecvFrom(Session* _pSession, IOCP* _pIocp);
	~IOCPOverlappedRecvFrom() override;

public:
	void Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker) override;

private:
	Session* receiver_;
};

NS_END
