/*
	�ۼ��� : ������
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
	void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pAcceptedSession;	
	TcpServer* m_pListeningServer;
};

} // namespace JNetwork