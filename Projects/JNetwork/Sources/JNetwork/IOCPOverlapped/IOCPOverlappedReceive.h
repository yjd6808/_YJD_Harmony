/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedReceive : public IOCPOverlapped
{
public:
	IOCPOverlappedReceive(TcpSession* session, IOCP* iocp);
	~IOCPOverlappedReceive() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pReceivedSession;	
};

NS_JNET_END