/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:19:48 AM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Packet/CommandParser.h>

USING_NS_JC;

NS_JNET_BEGIN

void CommandParser::AddCommand(Int16 cmd, Action<Session*, ICommand*> fn) {
	if (m_CommandMap.Exist(cmd)) {
		DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
		return;
	}

	m_CommandMap.Insert(cmd, fn);
}

bool CommandParser::RunCommand(Session* session, ICommand* cmd) const {
	const Int16 uiCmd = cmd->GetCommand();

	if (!m_CommandMap.Exist(uiCmd)) {
		return false;
	}

	m_CommandMap.Get(uiCmd)(session, cmd);
	return true;
}

NS_JNET_END