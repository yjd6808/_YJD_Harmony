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

//////////////////////////////////////////////////////////////////////////////////////////
CLIListener::CLIListener()
{
	table_.Insert("help", JCORE_CALLBACK_2(CLIListener::CLI_Help, this));
	table_.Insert("exit", JCORE_CALLBACK_2(CLIListener::CLI_Exit, this));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::OnInputProcessing(int _argc, String* _pArgv)
{
	if (!CLIListenerBase::OnInputProcessing(_argc, _pArgv))
	{
		return false;
	}

	return ExecuteCommand(_argc, _pArgv, table_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::CLI_Help(int _argc, String* _pArgv)
{
	String helpText{ 1024 };
	helpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(helpText.Source());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::CLI_Exit(int _argc, String* _pArgv)
{
	cocos2d::Director::getInstance()->end();
	return true;
}
