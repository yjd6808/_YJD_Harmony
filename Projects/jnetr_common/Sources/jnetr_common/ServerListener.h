/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:43:46 PM
 * =====================
 *
 */


#pragma once


#include <jnet/EventListener/ServerEventListener.h>
#include <jc/Primitives/String.h>

NS_JNET_BEGIN
class ServerListener : public ServerEventListener
{
public:
	ServerListener(const jc::String& _name);

protected:
	void OnStarted() override;
	void OnConnected(Session* _pConnectedSession) override;
	void OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(Session* _pSender, IPacket* _pPacket, Int32UL _sentBytes) override;
	void OnReceived(Session* _pReceiver, ICommand* _pCmd) override;
	void OnStopped() override;

private:
	jc::String name_;
};


NS_END
