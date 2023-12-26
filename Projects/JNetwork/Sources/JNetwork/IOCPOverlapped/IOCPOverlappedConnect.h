/*
 * 작성자 : 윤정도
 */

#pragma once


#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedConnect : public IOCPOverlapped
{
public:
	IOCPOverlappedConnect(TcpClient* client, IOCP* iocp, IPacket* sentPacket);
	~IOCPOverlappedConnect() override;
public:
	void Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) override;
private:
	TcpClient* m_pClient;	
	IPacket* m_pSentPacket;
};

NS_JNET_END