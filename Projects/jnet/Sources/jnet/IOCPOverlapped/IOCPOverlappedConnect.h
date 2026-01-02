/*
 * 작성자 : 윤정도
 */

#pragma once


#include <jnet/Host/TcpClient.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedConnect : public IOCPOverlapped
{
public:
	IOCPOverlappedConnect(TcpClient* _pClient, IOCP* _pIocp, IPacket* _pSentPacket);
	~IOCPOverlappedConnect() override;

public:
	void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	TcpClient* client_;
	IPacket* sentPacket_;
};

NS_JNET_END
