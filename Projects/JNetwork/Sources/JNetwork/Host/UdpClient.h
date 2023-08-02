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
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		int recvBufferSize = 6000,
		int sendBufferSize = 6000 
	);
	~UdpClient() override;

	void Initialize() override;
	void FlushSendBuffer() override;
	void SetRemoteEndpoint(const IPv4EndPoint& remoteEp);
	void Connected() override;
	void ConnectFailed(Int32U errorCode) override;
	void Disconnected() override;

	void NotifyCommand(ICommand* cmd) override;
	void NotifyPacket(IRecvPacket* packet) override;
	void Sent(ISendPacket* sentPacket, Int32UL sentBytes) override;
	Type GetType() const override { return eClient; }
	DetailType GetDetailType() const override { return eUdpClient; }
	const char* TypeName() override { return "UDP 클라"; }
	void SetEventListener(ClientEventListener* listener) { m_pEventListener = listener; }
protected:
	ClientEventListener* m_pEventListener;
};

using UdpClientPtr = JCore::SharedPtr<UdpClient>;
using UdpClientWPtr = JCore::WeakPtr<UdpClient>;

NS_JNET_END
