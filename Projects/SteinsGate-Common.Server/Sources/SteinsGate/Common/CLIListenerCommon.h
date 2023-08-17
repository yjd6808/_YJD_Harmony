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

	bool OnInputProcessing(int argc, JCore::String* argv) override;
private:
	bool CLI_HelpCommon(int argc, JCore::String* argv);
	bool CLI_ExitCommon(int argc, JCore::String* argv);

	TCLI_Table m_Table;
};



