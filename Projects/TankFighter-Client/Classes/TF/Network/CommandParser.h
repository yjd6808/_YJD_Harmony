/*
	�ۼ��� : ������
	������ �޾Ƽ� ó���� �� ���Ƽ� ���� ������ ���� �� ����.
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



