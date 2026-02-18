/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:37 PM
 * =====================
 *
 */


#pragma once


#include <jnet/Host/Session.h>
#include <jnet/EventListener/ClientEventListener.h>


NS_JNET_BEGIN

class IOCPOverlappedRecvFrom;
class UdpClient : public Session
{
public:
	UdpClient(
		const IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		PacketParser* _pParser = nullptr,
		int _recvBufferSize = 0,
		int _sendBufferSize = 0);
	~UdpClient() override;

	void Initialize() override;
	void FlushSendBuffer() override;
	void SetRemoteEndpoint(const IPv4EndPoint& _remoteEndPoint);
	void Connected() override;
	void ConnectFailed(Int32U _errorCode) override;
	void Disconnected(Int32U _errorCode) override;

	void NotifyCommand(ICommand* _pCmd) override;
	void NotifyPacket(RecvedCommandPacket* _pPacket) override;
	void NotifyRaw(char* _pData, int _len) override;

	void Sent(IPacket* _pSentPacket, Int32UL _sentBytes) override;
	Type GetType() const override { return eClient; }
	DetailType GetDetailType() const override { return eUdpClient; }
	const char* TypeName() const override { return "UDP 클라"; }
	void SetEventListener(ClientEventListener* _pListener) { pEventListener_ = _pListener; }

protected:
	ClientEventListener* pEventListener_;
};

using UdpClientPtr = jc::SharedPtr<UdpClient>;
using UdpClientWPtr = jc::WeakPtr<UdpClient>;

NS_END
