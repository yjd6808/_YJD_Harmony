/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:43:46 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/EventListener/ClientEventListener.h>
#include <JCore/Primitives/String.h>

NS_JNET_BEGIN
class ClientListener : public ClientEventListener
{
public:
	ClientListener(const JCore::String& name);
protected:
	void OnConnected(Session* session) override;
	void OnDisconnected(Session* session) override;
	void OnSent(Session* session, IPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(Session* session, ICommand* cmd) override;
	void OnConnectFailed(Session* session, Int32U errorMessage) override;
private:
	JCore::String m_Name;
};



NS_JNET_END
