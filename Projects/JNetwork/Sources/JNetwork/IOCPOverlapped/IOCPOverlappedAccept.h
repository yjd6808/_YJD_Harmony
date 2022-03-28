/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

namespace JNetwork {

class IOCPOverlappedAccept : public IOCPOverlapped
{
public:
	IOCPOverlappedAccept(TcpServer* tcpServer, TcpSession* session, IOCP* iocp);
	~IOCPOverlappedAccept() override;
public:
	virtual void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey);
private:
	TcpSession* m_pAcceptedSession;	
	TcpServer* m_pListeningServer;
};

} // namespace JNetwork