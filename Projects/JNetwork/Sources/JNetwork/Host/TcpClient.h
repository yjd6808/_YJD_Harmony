/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/EventListener/ClientEventListener.h>

NS_JNET_BEGIN

class TcpClient : public Session
{
public:
	TcpClient(
		const IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		PacketParser* parser = nullptr,
		int sendBufferSize = 0, 
		int recvBufferSize = 0
	);

	~TcpClient() override;

	void Initialize() override;
	bool Connect(const IPv4EndPoint& remoteAddr, int timeoutMiliseconds = 0);
	bool ConnectAsync(const IPv4EndPoint& destination);
	void Connected() override;
	void ConnectFailed(Int32U errorCode) override;
	void Disconnected(Int32U errorCode) override;
	void NotifyCommand(ICommand* cmd) override;
	void NotifyPacket(RecvedCommandPacket* packet) override;
	void NotifyRaw(char* data, int len) override;
	void Sent(IPacket* sentPacket, Int32UL sentBytes) override;
	Type GetType() const override { return eClient; }
	DetailType GetDetailType() const override { return eTcpClient; }
	const char* TypeName() override { return "TCP 클라"; }
	void SetEventListener(ClientEventListener* listener) { m_pEventListener = listener; }
protected:
	ClientEventListener* m_pEventListener;
};

using TcpClientPtr = JCore::SharedPtr<TcpClient>;
using TcpClientWPtr = JCore::WeakPtr<TcpClient>;

NS_JNET_END
