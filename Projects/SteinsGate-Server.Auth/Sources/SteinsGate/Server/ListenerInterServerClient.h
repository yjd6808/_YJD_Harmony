/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 8:44:27 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>
#include <JNetwork/Packet/CommandParser.h>

class ListenerInterServerClient : public JNetwork::ClientEventListener
{
public:
	ListenerInterServerClient();
protected:
	void OnReceived(JNetwork::ICommand* cmd) override;
private:
	JNetwork::CommandParser m_Parser;
};