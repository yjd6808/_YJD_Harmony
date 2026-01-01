/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:14:16 AM
 * =====================
 *
 */


#pragma once

#include <jc/Container/HashMap.h>
#include <jc/Functional.h>

#include <jnet/Host/Session.h>

NS_JNET_BEGIN

class CommandParser
{
public:
	virtual ~CommandParser() = default;

	virtual void Initialize()
	{
	}

	template <typename TCommand>
	void AddCommand(jc::Action<Session*, ICommand*> _fn)
	{
		static_assert(jc::IsBaseOf_v<ICommand, TCommand>, "... template parameter is not derived from ICommand");

		if (commandActionMap_.Exist(TCommand::_Command()))
		{
			jc_assert_msg(false, "해당 커맨드가 이미 있습니다.");
			return;
		}

		commandActionMap_.Insert(TCommand::_Command(), _fn);
	}

	virtual bool RunCommand(Session* _pSession, jnet::ICommand* _pCommand) const;

private:
	// jc::Action<Session*, ICommand*> m_fnPreRun;
	// jc::Action<Session*, ICommand*> m_fnPostRun;

	jc::HashMap<Cmd_t, jc::Action<Session*, ICommand*>> commandActionMap_;
};

NS_JNET_END
