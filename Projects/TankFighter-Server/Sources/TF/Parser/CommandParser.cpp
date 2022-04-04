
#define _WINSOCKAPI_

#include <TF/Parser/CommandParser.h>
#include <TF/Game/Player.h>
#include <TF/Util/Console.h>

using namespace JCore;
using namespace JNetwork;

void CommandParser::AddCommand(Int16 cmd, Action<Player*, ICommand*> fn) {
	if (m_CommandMap.Exist(cmd)) {
		DebugAssert(false, "�ش� Ŀ�ǵ尡 �̹� �ֽ��ϴ�.");
		return;
	}

	m_CommandMap.Insert(cmd, fn);
}

bool CommandParser::RunCommand(Player* session, ICommand* cmd) const {
	const Int16 uiCmd = cmd->GetCommand();

	if (!m_CommandMap.Exist(uiCmd)) {
		return false;
	}

	Console::WriteLine(ConsoleColor::LIGHTCYAN, "Ŀ�ǵ� ���� (Command ID : %d)", cmd->GetCommand());
	m_CommandMap.Get(uiCmd)(session, cmd);


	return true;
}
