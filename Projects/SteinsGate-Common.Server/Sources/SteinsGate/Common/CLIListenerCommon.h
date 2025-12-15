/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CLIListenerBase.h>

class CLIListenerCommon : public CLIListenerBase
{
public:
	CLIListenerCommon();

	bool OnInputProcessing(int _argc, JCore::String* _pArgv) override;

private:
	bool CLI_HelpCommon(int _argc, JCore::String* _pArgv);
	bool CLI_ExitCommon(int _argc, JCore::String* _pArgv);

	TCLI_Table table_;
};
