/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once

#include <jnet/Packet/CommandParser.h>
#include <jnet/EventListener/ClientEventListener.h>

NS_SG_BEGIN

class NetClientListener : public jnet::ClientEventListener
{
public:
	NetClientListener(jnet::CommandParser* _pParser);

protected:
	virtual void OnConnected(jnet::Session* _pSession) override;
	virtual void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode) override;
	virtual void OnDisconnected(jnet::Session* _pSession, Int32U _errorCode) override;
	virtual void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	virtual void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	virtual void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;

private:
	jnet::CommandParser* pParser_ = nullptr;
};

NS_END