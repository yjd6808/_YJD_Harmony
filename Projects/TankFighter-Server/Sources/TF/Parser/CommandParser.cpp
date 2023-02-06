/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Parser/CommandParser.h>
#include <TF/Game/Player.h>
#include <Common/Command.h>

#include <JCore/Utils/Console.h>

using namespace JCore;
using namespace JNetwork;

void CommandParser::AddCommand(Int16 cmd, Action<Player*, ICommand*> fn) {
	if (m_CommandMap.Exist(cmd)) {
		DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
		return;
	}

	m_CommandMap.Insert(cmd, fn);
}

bool CommandParser::RunCommand(Player* session, ICommand* cmd) const {
	const Int16 uiCmd = cmd->GetCommand();

	if (!m_CommandMap.Exist(uiCmd)) {
		return false;
	}

	if (cmd->GetCommand() != BATTLE_FIELD_TANK_MOVE_SYN && cmd->GetCommand() != TCP_RTT_SYN) {
		SafeConsole::WriteLine(ConsoleColor::LightCyan, "커맨드 실행 (Command ID : %d)", cmd->GetCommand());
	}
	
	m_CommandMap.Get(uiCmd)(session, cmd);

	return true;
}
