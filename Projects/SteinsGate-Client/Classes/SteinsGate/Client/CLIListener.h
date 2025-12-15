/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CLIListenerBase.h>

class CLIListener : public CLIListenerBase
{
public:
	CLIListener();

	bool OnInputProcessing(int _argc, JCore::String* _pArgv) override;

private:
	bool CLI_Help(int _argc, SGString* _pArgv);
	bool CLI_Exit(int _argc, SGString* _pArgv);

	TCLI_Table table_;
};
