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
	void OnConnected() override;
	void OnDisconnected() override;
	void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(ICommand* cmd) override;
	void OnConnectFailed(Int32U errorMessage) override;
private:
	JCore::String m_Name;
};



NS_JNET_END
