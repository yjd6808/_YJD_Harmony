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
	bool OnInputProcessing(int argc, JCore::String* argv) override;

	bool CLI_Help();
	bool CLI_Exit();
};



