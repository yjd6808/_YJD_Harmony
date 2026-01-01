/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:19:48 AM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jnet/Packet/CommandParser.h>

USING_NS_JC;

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool CommandParser::RunCommand(Session* _pSession, ICommand* _pCommand) const
{
	const Int16 commandId = _pCommand->GetId();

	if (!commandActionMap_.Exist(commandId))
	{
		return false;
	}

	commandActionMap_.Get(commandId)(_pSession, _pCommand);
	return true;
}

NS_JNET_END
