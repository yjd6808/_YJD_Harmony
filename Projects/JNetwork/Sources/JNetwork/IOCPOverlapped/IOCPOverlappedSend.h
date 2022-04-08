/*
 * �ۼ��� : ������
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
	void Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpSession* m_pSentSession;	
	ISendPacket* m_pSentPacket;
};

} // namespace JNetwork