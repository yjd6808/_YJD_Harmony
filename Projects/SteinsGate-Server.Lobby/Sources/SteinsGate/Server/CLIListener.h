/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CLIListenerCommon.h>

class CLIListener : public CLIListenerCommon
{
public:
	CLIListener();

	bool OnInputProcessing(int argc, JCore::String* argv) override;
private:
	bool CLI_Example(int argc, JCore::String* argv);
};



