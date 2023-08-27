/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:23:59 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/Packet/RecvPacket.h>

NS_JNET_BEGIN

class TcpServer;
class TcpSession : public Session
{
public:
	TcpSession(
		TcpServer* server, 
		const IOCPPtr& iocp, 
		const JCore::MemoryPoolAbstractPtr& bufferAllocator, 
		int recvBufferSize, 
		int sendBufferSize
	);

	virtual void OnCreated() {}
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}

	void NotifyCommand(ICommand* cmd) override;
	void NotifyPacket(IRecvPacket* recvPacket) override;
	void Initialize() override;
	void Connected() override;
	void ConnectFailed(Int32U errorCode) override;
	void Disconnected() override;
	bool AcceptAsync();
	void AcceptWait();
	bool Accepted(Int32UL receivedBytes);
	void Sent(ISendPacket* sentPacket, Int32UL receivedBytes) override;
	Type GetType() const override { return eSession; }
	DetailType GetDetailType() const override { return eTcpSession; }
	const char* TypeName() override { return "TCP 세션"; }

protected:
	TcpServer* m_pServer;
};

using TcpSessionPtr = JCore::SharedPtr<TcpSession>;

NS_JNET_END
