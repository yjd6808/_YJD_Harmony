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
		ClientEventListener* listener, 
		int sendBufferSize = 6000, 
		int recvBufferSize = 6000
	);

	~TcpClient() override;

	void Initialize() override;
	bool ConnectAsync(const IPv4EndPoint& destination);
	void Connected() override;
	void ConnectFailed(Int32U errorCode);
	void Disconnected() override;
	void NotifyCommand(ICommand* cmd) override;
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
	Type GetType() const override { return eClient; }
protected:
	ClientEventListener* m_pClientEventListener;
};

using TcpClientPtr = JCore::SharedPtr<TcpClient>;
using TcpClientWPtr = JCore::WeakPtr<TcpClient>;

NS_JNET_END
