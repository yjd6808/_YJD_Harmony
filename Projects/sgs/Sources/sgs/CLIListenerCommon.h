/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <sg/CLIListener.h>

class CLIListenerCommon : public CLIListener
{
public:
	CLIListenerCommon();

	bool OnInputProcessing(int _argc, jc::String* _pArgv) override;

private:
	bool CLI_HelpCommon(int _argc, jc::String* _pArgv);
	bool CLI_ExitCommon(int _argc, jc::String* _pArgv);

	TCLI_Table table_;
};
