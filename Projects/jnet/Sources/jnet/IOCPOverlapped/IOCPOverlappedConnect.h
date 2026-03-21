/*
 * 작성자 : 윤정도
 */

#pragma once


#include "jnet/Host/TcpClient.h"
#include "jnet/IOCPOverlapped/IOCPOverlapped.h"

NS_JNET_BEGIN

class IOCPOverlappedConnect : public IOCPOverlapped
#ifdef USE_OVERLAPPED_STATIC_POOL
	, public jc::ObjectPool<IOCPOverlappedConnect>
#endif
{
public:
	IOCPOverlappedConnect(TcpClient* _pClient, IOCP* _pIocp, IPacket* _pSentPacket);
	~IOCPOverlappedConnect() override;

public:
	void Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker) override;

private:
	TcpClient* client_;
	IPacket* sentPacket_;
};

NS_END
