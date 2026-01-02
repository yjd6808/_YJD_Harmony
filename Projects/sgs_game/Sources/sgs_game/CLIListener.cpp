/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "CLIListener.h"

//////////////////////////////////////////////////////////////////////////////////////////
CLIListener::CLIListener()
{
	table_.Insert("example", JCORE_CALLBACK_2(CLIListener::CLI_Example, this));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::OnInputProcessing(int _argc, jc::String* _pArgv)
{
	if (!CLIListenerCommon::OnInputProcessing(_argc, _pArgv))
	{
		return false;
	}

	const TCLI_Callback* pCallback = table_.Find(_pArgv[0].Source());

	if (pCallback)
	{
		return (*pCallback)(_argc, _pArgv);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::CLI_Example(int _argc, jc::String* _pArgv)
{
	return true;
}
