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
	IOCPOverlappedConnect(TcpSession* session, IOCP* iocp, IPacket* sentPacket);
	~IOCPOverlappedConnect() override;
public:
	virtual void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey);
private:
	TcpSession* m_pConnectedSession;	
	IPacket* m_pSentPacket;
};

} // namespace JNetwork