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

bool CLIListener::OnInputProcessing(int argc, JCore::String* argv) {
	if (!CLIListenerBase::OnInputProcessing(argc, argv)) {
		return false;
	}

	if (argv[0] == "help") {
		return CLI_Help();
	}

	if (argv[0] == "exit") {
		return CLI_Exit();
	}

	return true;
}

bool CLIListener::CLI_Help() {
	String szHelpText{ 1024 };
	szHelpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListener::CLI_Exit() {
	cocos2d::Director::getInstance()->end();
	return true;
}
