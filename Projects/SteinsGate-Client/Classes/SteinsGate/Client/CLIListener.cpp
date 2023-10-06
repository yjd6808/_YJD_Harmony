/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "CLIListener.h"

USING_NS_JC;

CLIListener::CLIListener() {
	m_Table.Insert("help",		JCORE_CALLBACK_2(CLIListener::CLI_Help, this));
	m_Table.Insert("exit",		JCORE_CALLBACK_2(CLIListener::CLI_Exit, this));
}

bool CLIListener::OnInputProcessing(int argc, String* argv) {
	if (!CLIListenerBase::OnInputProcessing(argc, argv)) {
		return false;
	}

	return ExecuteCommand(argc, argv, m_Table);
}

bool CLIListener::CLI_Help(int argc, String* argv) {
	String szHelpText{ 1024 };
	szHelpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListener::CLI_Exit(int argc, String* argv) {
	cocos2d::Director::getInstance()->end();
	return true;
}
