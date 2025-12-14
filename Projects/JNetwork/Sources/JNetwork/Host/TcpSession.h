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
	TcpSession(
		TcpServer* _pServer,
		const IOCPPtr& _pIocp,
		const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
		PacketParser* _pParser,
		int _recvBufferSize,
		int _sendBufferSize);

	virtual void OnCreated() {}
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}

	void NotifyCommand(ICommand* _pCmd) override;
	void NotifyPacket(RecvedCommandPacket* _pRecvPacket) override;
	void NotifyRaw(char* _pData, int _len) override;

	void Initialize() override;
	void Connected() override;
	void ConnectFailed(Int32U _errorCode) override;
	void Disconnected(Int32U _errorCode) override;
	bool AcceptAsync();
	void AcceptWait();
	bool Accepted(Int32UL _receivedBytes);
	void Sent(IPacket* _pSentPacket, Int32UL _receivedBytes) override;
	Type GetType() const override { return eSession; }
	DetailType GetDetailType() const override { return eTcpSession; }
	const char* TypeName() override { return "TCP 세션"; }

protected:
	TcpServer* pServer_;
};

using TcpSessionPtr = JCore::SharedPtr<TcpSession>;

NS_JNET_END
