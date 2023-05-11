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


bool CommandParser::RunCommand(Session* session, ICommand* cmd) const {
	const Int16 uiCmd = cmd->GetCommand();

	if (!m_CommandActionMap.Exist(uiCmd)) {
		return false;
	}

	m_CommandActionMap.Get(uiCmd)(session, cmd);
	return true;
}

const char* CommandParser::GetCommandName(Int16 cmd) {
	if (!m_CommandActionMap.Exist(cmd)) {
		return "";
	}

	return m_CommandNameMap.Get(cmd);
}

NS_JNET_END
