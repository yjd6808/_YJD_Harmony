/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "CLIListenerCommon.h"

USING_NS_JC;

CLIListenerCommon::CLIListenerCommon() {
	m_Table.Insert("help",		JCORE_CALLBACK_2(CLIListenerCommon::CLI_HelpCommon, this));
	m_Table.Insert("exit",		JCORE_CALLBACK_2(CLIListenerCommon::CLI_ExitCommon, this));
}

bool CLIListenerCommon::OnInputProcessing(int argc, JCore::String* argv) {
	if (!CLIListenerBase::OnInputProcessing(argc, argv)) {
		return false;
	}

	return ExecuteCommand(argc, argv, m_Table);
}

bool CLIListenerCommon::CLI_HelpCommon(int argc, JCore::String* argv) {
	String szHelpText{ 1024 };
	szHelpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListenerCommon::CLI_ExitCommon(int argc, JCore::String* argv) {
	Core::CommonNetMaster->Terminate();
	return true;
}

