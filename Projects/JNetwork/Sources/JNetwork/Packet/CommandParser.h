/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:14:16 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Functional.h>

#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

class CommandParser
{
public:
	virtual ~CommandParser() = default;

	virtual void Initialize() {}

	virtual void AddCommand(Int16 cmd, JCore::Action<Session*, JNetwork::ICommand*> fn);
	virtual bool RunCommand(Session* session, JNetwork::ICommand* cmd) const;
private:
	// JCore::Action<Session*, ICommand*> m_fnPreRun;
	// JCore::Action<Session*, ICommand*> m_fnPostRun;

	JCore::HashMap<Int16, JCore::Action<Session*, ICommand*>> m_CommandActionMap;
};


NS_JNET_END