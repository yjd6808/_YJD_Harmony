/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:23:59 AM
 * =====================
 *
 */

#pragma once

#include "jnet/Host/Session.h"

NS_JNET_BEGIN

class TcpServer;
class TcpSession : public Session
{
public:
	TcpSession(
		TcpServer* _pServer,
		const IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		PacketParser* _pParser,
		int _recvBufferSize,
		int _sendBufferSize);

	virtual void OnCreated() {}
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}

	virtual void NotifyCommand(ICommand* _pCmd) override;
	virtual void NotifyMessage(jc::CMessage _msg) override;
	virtual void NotifyPacket(RecvedPacket* _pRecvPacket) override;
	virtual void NotifyRaw(char* _pData, int _len) override;

	void Initialize() override;
	void Connected() override;
	void ConnectFailed(_u32 _errorCode) override;
	void Disconnected(_u32 _errorCode) override;
	bool AcceptAsync();
	void AcceptWait();
	bool Accepted(_u32l _receivedBytes);
	void Sent(IPacket* _pSentPacket, _u32l _receivedBytes) override;
	Type GetType() const override { return eSession; }
	DetailType GetDetailType() const override { return eTcpSession; }
	const char* TypeName() const override { return "TCP 세션"; }

protected:
	TcpServer* pServer_;
};

using TcpSessionPtr = jc::SharedPtr<TcpSession>;

NS_END
