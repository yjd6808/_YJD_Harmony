/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:43:46 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/EventListener/ServerEventListener.h>
#include <JCore/Primitives/String.h>

NS_JNET_BEGIN

class ServerListener : public ServerEventListener
{
public:
	ServerListener(const JCore::String& name);
protected:
	void OnStarted() override;
	void OnConnected(Session* connectedSession) override;
	void OnDisconnected(Session* disconnetedSession, Int32U errorCode) override;
	void OnSent(Session* sender, IPacket* packet, Int32UL sentBytes) override;
	void OnReceived(Session* receiver, ICommand* cmd) override;
	void OnStopped() override;
private:
	JCore::String m_Name;
};



NS_JNET_END
