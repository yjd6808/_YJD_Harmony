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
	void OnDisconnected(Session* _pSession, _u32 _errorCode) override;
	void OnSent(Session* _pSession, IPacket* _pSentPacket, _u32l _sentBytes) override;
	void OnReceivedCmd(Session* _pSession, ICommand* _pCommand) override;
	void OnReceivedMsg(Session* _pSession, jc::CMessage _msg) override;
	void OnConnectFailed(Session* _pSession, _u32 _errorMessage) override;
private:
	jc::String name_;
};



NS_END
