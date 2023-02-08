/*
 * 작성자 : 윤정도
 */

#pragma once


#include <JNetwork/Host/Session.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedSend : public IOCPOverlapped
{
public:
	IOCPOverlappedSend(Session* session, IOCP* iocp, ISendPacket* sentPacket);
	~IOCPOverlappedSend() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	Session* m_pSender;	
	ISendPacket* m_pSentPacket;
};

NS_JNET_END