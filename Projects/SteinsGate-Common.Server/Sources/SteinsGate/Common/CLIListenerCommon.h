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
	bool OnInputProcessing(int argc, JCore::String* argv) override;

	bool CLI_HelpCommon();
	bool CLI_ExitCommon();
};



