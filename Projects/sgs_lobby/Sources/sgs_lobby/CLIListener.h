/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <sgs/CLIListenerCommon.h>

class CLIListener : public CLIListenerCommon
{
public:
	CLIListener();

	bool OnInputProcessing(int _argc, jc::String* _pArgv) override;
private:
	bool CLI_Example(int _argc, jc::String* _pArgv);

	TCLI_Table table_;
};



