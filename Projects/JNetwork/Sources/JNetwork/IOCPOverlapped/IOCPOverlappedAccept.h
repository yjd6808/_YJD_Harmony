/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedAccept : public IOCPOverlapped
{
public:
	IOCPOverlappedAccept(TcpSession* session, IOCP* iocp);
	~IOCPOverlappedAccept() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pAcceptedSession;	
};

NS_JNET_END