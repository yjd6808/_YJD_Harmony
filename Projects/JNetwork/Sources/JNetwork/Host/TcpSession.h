/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:23:59 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

class TcpServer;
class TcpSession : public Session
{
public:
	TcpSession(TcpServer* server, const IOCPPtr& iocp, int recvBufferSize, int sendBufferSize);

	void NotifyCommand(ICommand* cmd) override;

	void Initialize() override;
	void Connected() override;
	void Disconnected() override;
	bool AcceptAsync();
	void AcceptWait();
	bool Accepted(Int32UL receivedBytes);
	void Sent(ISendPacket* sentPacket, Int32UL receivedBytes) override;
	Type GetType() const override { return eSession; }
private:
	TcpServer* m_pServer;
};

using TcpSessionPtr = JCore::SharedPtr<TcpSession>;

NS_JNET_END
