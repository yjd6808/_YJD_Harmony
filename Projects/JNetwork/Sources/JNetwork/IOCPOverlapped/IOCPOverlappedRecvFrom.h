/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:27 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedRecvFrom : public IOCPOverlapped
{
public:
	IOCPOverlappedRecvFrom(Session* _pSession, IOCP* _pIocp);
	~IOCPOverlappedRecvFrom() override;

public:
	void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	Session* receiver_;
};

NS_JNET_END
