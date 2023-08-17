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

	bool OnInputProcessing(int argc, JCore::String* argv) override;
private:
	bool CLI_Help(int argc, SGString* argv);
	bool CLI_Exit(int argc, SGString* argv);

	TCLI_Table m_Table;
};



