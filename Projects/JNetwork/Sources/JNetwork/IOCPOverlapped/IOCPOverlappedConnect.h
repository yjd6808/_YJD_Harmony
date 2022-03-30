/*
	작성자 : 윤정도
*/

#pragma once


#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

namespace JNetwork {

class IOCPOverlappedConnect : public IOCPOverlapped
{
public:
	IOCPOverlappedConnect(TcpSession* session, IOCP* iocp, ISendPacket* sentPacket);
	~IOCPOverlappedConnect() override;
public:
	void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pConnectedSession;	
	ISendPacket* m_pSentPacket;
};

} // namespace JNetwork