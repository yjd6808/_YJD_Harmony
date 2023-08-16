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

bool CLIListenerCommon::OnInputProcessing(int argc, JCore::String* argv) {
	if (!CLIListenerBase::OnInputProcessing(argc, argv)) {
		return false;
	}

	if (argv[0] == "help") {
		return CLI_HelpCommon();
	}

	if (argv[0] == "exit") {
		return CLI_ExitCommon();
	}

	return true;
}

bool CLIListenerCommon::CLI_HelpCommon() {
	String szHelpText{ 1024 };
	szHelpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListenerCommon::CLI_ExitCommon() {
	CoreCommonNetMaster_v->Terminate();
	return true;
}

