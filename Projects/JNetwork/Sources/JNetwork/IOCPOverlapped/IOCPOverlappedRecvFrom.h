/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:27 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedRecvFrom : public IOCPOverlapped
{
public:
	IOCPOverlappedRecvFrom(UdpClient* client, IOCP* iocp);
	~IOCPOverlappedRecvFrom() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	UdpClient* m_pReceiver;
};

NS_JNET_END