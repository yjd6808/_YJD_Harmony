/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jnet/Host/Session.h>
#include <jnet/EventListener/ClientEventListener.h>

NS_JNET_BEGIN

class TcpClient : public Session
{
public:
	TcpClient(
		const IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		PacketParser* _pParser = nullptr,
		int _sendBufferSize = 0,
		int _recvBufferSize = 0);

	~TcpClient() override;

	void Initialize() override;
	bool Connect(const IPv4EndPoint& _remoteEndPoint, int _timeoutMilliseconds = 0);
	bool ConnectAsync(const IPv4EndPoint& _destination);
	void Connected() override;
	void ConnectFailed(Int32U _errorCode) override;
	void Disconnected(Int32U _errorCode) override;
	void NotifyCommand(ICommand* _pCmd) override;
	void NotifyPacket(RecvedCommandPacket* _pPacket) override;
	void NotifyRaw(char* _pData, int _len) override;
	void Sent(IPacket* _pSentPacket, Int32UL _sentBytes) override;
	Type GetType() const override { return eClient; }
	DetailType GetDetailType() const override { return eTcpClient; }
	const char* TypeName() override { return "TCP 클라"; }
	void SetEventListener(ClientEventListener* _pListener) { pEventListener_ = _pListener; }

protected:
	ClientEventListener* pEventListener_;
};

using TcpClientPtr = jc::SharedPtr<TcpClient>;
using TcpClientWPtr = jc::WeakPtr<TcpClient>;

NS_JNET_END
