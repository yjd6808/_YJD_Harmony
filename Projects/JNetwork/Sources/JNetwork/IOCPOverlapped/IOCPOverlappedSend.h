/*
	작성자 : 윤정도
*/

#pragma once


#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

namespace JNetwork {

class IOCPOverlappedSend : public IOCPOverlapped
{
public:
	IOCPOverlappedSend(TcpSession* session, IOCP* iocp, ISendPacket* sentPacket);
	~IOCPOverlappedSend() override;
public:
	virtual void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey);
private:
	TcpSession* m_pSentSession;	
	ISendPacket* m_pSentPacket;
};

} // namespace JNetwork