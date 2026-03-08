/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <jnet/EventListener/ServerEventListener.h>

NS_SG_BEGIN

class NetServerListener : public jnet::ServerEventListener
{
public:
	NetServerListener(jnet::TcpServer* _pServer, jnet::CommandParser* _pParser);
	~NetServerListener() override = default;
protected:
	virtual void OnStarted() override;
	virtual void OnConnected(jnet::Session* _pConnectedSession) override;
	virtual void OnDisconnected(jnet::Session* _pDisconnectedSession, _u32 _errorCode) override;
	virtual void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, _u32l _sentBytes) override;
	virtual void OnReceivedCmd(jnet::Session* _pReceiver, jnet::ICommand* _pRecvCmd) override;	// 커맨드 하나
	virtual void OnReceivedPacket(jnet::Session* _pReceiver, jnet::RecvedPacket* _pRecvPacket) override; // 패킷 (최소 1개 이상의 커맨드를 담고있음)
	virtual void OnStopped() override;

	jnet::TcpServer* pServer_ = nullptr;
	jnet::CommandParser* pParser_;
};

NS_END
