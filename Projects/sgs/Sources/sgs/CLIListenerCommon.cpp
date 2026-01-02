/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "CLIListenerCommon.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CLIListenerCommon::CLIListenerCommon()
{
	table_.Insert("help", JC_CALLBACK_2(CLIListenerCommon::CLI_HelpCommon, this));
	table_.Insert("exit", JC_CALLBACK_2(CLIListenerCommon::CLI_ExitCommon, this));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerCommon::OnInputProcessing(int _argc, jc::String* _pArgv)
{
	if (!CLIListener::OnInputProcessing(_argc, _pArgv))
	{
		return false;
	}

	return ExecuteCommand(_argc, _pArgv, table_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerCommon::CLI_HelpCommon(int _argc, jc::String* _pArgv)
{
	String helpText{ 1024 };
	helpText += " - exit: 애플리케이션을 종료합니다.\n";
	Console::WriteLine(helpText.Source());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerCommon::CLI_ExitCommon(int _argc, jc::String* _pArgv)
{
	sg::CommonNetGroupMgr->Terminate();
	return true;
}
