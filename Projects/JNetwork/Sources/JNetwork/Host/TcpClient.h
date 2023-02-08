/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/Host/Listener/ClientEventListener.h>

#include <JNetwork/IOCP/IOCP.h>

NS_JNET_BEGIN

class TcpClient : public Session
{
public:
	TcpClient(const IOCPPtr& iocp, ClientEventListener* listener, int sendBufferSize = 6000, int recvBufferSize = 6000);
	~TcpClient() override;

	bool ConnectAsync(const IPv4EndPoint& destination);
	void Connected() override;
	void Disconnected() override;
	void NotifyCommand(ICommand* cmd) override;
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
protected:
	ClientEventListener* m_pClientEventListener;
};

NS_JNET_END
