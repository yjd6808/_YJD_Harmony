/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

namespace JNetwork {

class IOCPOverlappedReceive : public IOCPOverlapped
{
public:
	IOCPOverlappedReceive(TcpSession* session, IOCP* iocp);
	~IOCPOverlappedReceive() override;
public:
	void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pReceivedSession;	
};

} // namespace JNetwork