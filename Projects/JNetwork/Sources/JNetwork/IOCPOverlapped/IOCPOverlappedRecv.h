/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedRecv : public IOCPOverlapped
{
public:
	IOCPOverlappedRecv(Session* session, IOCP* iocp);
	~IOCPOverlappedRecv() override;
public:
	void Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) override;
private:
	Session* m_pReceivedSession;	
};

NS_JNET_END