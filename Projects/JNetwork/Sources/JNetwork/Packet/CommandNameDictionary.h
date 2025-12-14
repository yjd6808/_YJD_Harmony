/*
 * 작성자: 윤정도
 * 생성일: 7/3/2023 5:46:16 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/HashMap.h>
#include <JNetwork/Packet/Command.h>

NS_JNET_BEGIN

class CommandNameDictionary
{
public:
	CommandNameDictionary(int _capacity = 64)
		: commandNameMap_(_capacity)
	{
	}

	virtual ~CommandNameDictionary() = default;

	template <typename TCommand>
	void Add()
	{
		static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... template parameter is not derived from ICommand");

		if (commandNameMap_.Exist(TCommand::_Command()))
		{
			DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
			return;
		}

		commandNameMap_.Insert(TCommand::_Command(), TCommand::_Name());
	}

	const char* Get(Cmd_t _command) const;

protected:
	JCore::HashMap<Cmd_t, const char*> commandNameMap_;
};

NS_JNET_END
