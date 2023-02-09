/*
	작성자 : 윤정도
	씬에서 받아서 처리할 것 같아서 딱히 쓸일이 없을 것 같다.
*/

#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Functional.h>

#include <JNetwork/Packet.h>

class CommandParser
{
public:
	virtual ~CommandParser() = default;
public:
	void AddCommand(Int16 cmd, JCore::Action<JNetwork::ICommand*> fn);
	bool RunCommand(JNetwork::ICommand* cmd) const;
private:
	JCore::HashMap<Int16, JCore::Action<JNetwork::ICommand*>> m_CommandMap;
	
};



