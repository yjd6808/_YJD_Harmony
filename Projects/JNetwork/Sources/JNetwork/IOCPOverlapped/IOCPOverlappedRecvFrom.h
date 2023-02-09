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

	SOCKADDR_IN* SenderAddrPtr() { return &m_SenderAddr; }
	int* SenderAddrLenPtr() { return &m_iAddrLen; }
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	UdpClient* m_pReceiver;
	SOCKADDR_IN m_SenderAddr;		// TODO: 기존 IP와 조화를 이루지 못함. 변경 필요
	int m_iAddrLen;
};

NS_JNET_END