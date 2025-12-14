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

	virtual void Initialize()
	{
	}

	template <typename TCommand>
	void AddCommand(JCore::Action<Session*, ICommand*> _fn)
	{
		static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... template parameter is not derived from ICommand");

		if (commandActionMap_.Exist(TCommand::_Command()))
		{
			DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
			return;
		}

		commandActionMap_.Insert(TCommand::_Command(), _fn);
	}

	virtual bool RunCommand(Session* _pSession, JNetwork::ICommand* _pCommand) const;

private:
	// JCore::Action<Session*, ICommand*> m_fnPreRun;
	// JCore::Action<Session*, ICommand*> m_fnPostRun;

	JCore::HashMap<Cmd_t, JCore::Action<Session*, ICommand*>> commandActionMap_;
};

NS_JNET_END
