/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:37 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/Session.h>
#include <JNetwork/EventListener/ClientEventListener.h>


NS_JNET_BEGIN

class IOCPOverlappedRecvFrom;
class UdpClient : public Session
{
public:
	UdpClient(
		const IOCPPtr& iocp, 
		ClientEventListener* listener,
		int recvBufferSize = 6000,
		int sendBufferSize = 6000 
	);
	~UdpClient() override;

	void Initialize() override;
	bool RecvFromAsync();
	bool SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination);
	void Connected() override;
	void Disconnected() override;
	void Received(Int32UL receivedBytes, IOCPOverlappedRecvFrom* recvFrom);

	void NotifyCommand(ICommand* cmd) override;
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
	Type GetType() const override { return eClient; }
protected:
	ClientEventListener* m_pClientEventListener;
};

NS_JNET_END
