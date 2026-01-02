/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:43:46 PM
 * =====================
 *
 */


#pragma once


#include <jnet/EventListener/ClientEventListener.h>
#include <jc/Primitives/String.h>

NS_JNET_BEGIN
class ClientListener : public ClientEventListener
{
public:
	ClientListener(const jc::String& _name);
protected:
	void OnConnected(Session* _pSession) override;
	void OnDisconnected(Session* _pSession, Int32U _errorCode) override;
	void OnSent(Session* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(Session* _pSession, ICommand* _pCommand) override;
	void OnConnectFailed(Session* _pSession, Int32U _errorMessage) override;
private:
	jc::String name_;
};



NS_JNET_END
