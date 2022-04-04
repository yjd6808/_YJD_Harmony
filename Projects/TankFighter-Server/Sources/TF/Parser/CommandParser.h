/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Functional.h>

#include <JNetwork/Packet.h>
#include <JNetwork/Host/TcpSession.h>

class Player;
class CommandParser
{
public:
	virtual ~CommandParser() = default;
public:
	void AddCommand(Int16 cmd, JCore::Action<Player*, JNetwork::ICommand*> fn);
	bool RunCommand(Player* session, JNetwork::ICommand* cmd) const;
private:
	JCore::HashMap<Int16, JCore::Action<Player*, JNetwork::ICommand*>> m_CommandMap;
};



