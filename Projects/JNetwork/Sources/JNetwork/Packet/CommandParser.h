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

	template <typename Cmd>
	void AddCommand(JCore::Action<Session*, ICommand*> fn) {
		static_assert(JCore::IsBaseOf_v<ICommand, Cmd>, "... template parameter is not derived from ICommand");

		if (m_CommandActionMap.Exist(Cmd::Command())) {
			DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
			return;
		}

		m_CommandActionMap.Insert(Cmd::Command(), fn);
		m_CommandNameMap.Insert(Cmd::Command(), Cmd::Name());
	}
	virtual bool RunCommand(Session* session, JNetwork::ICommand* cmd) const;
	virtual const char* GetCommandName(Int16 cmd);
private:
	// JCore::Action<Session*, ICommand*> m_fnPreRun;
	// JCore::Action<Session*, ICommand*> m_fnPostRun;

	JCore::HashMap<Int16, JCore::Action<Session*, ICommand*>> m_CommandActionMap;
	JCore::HashMap<Int16, const char*> m_CommandNameMap;
};


NS_JNET_END