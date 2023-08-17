/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:25:23 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ICLIListener.h>

class CLIListenerBase : public ICLIListener
{
public:
	using TCLI_Callback = SGFuncFn<bool, int, SGString*>;
	using TCLI_Table = SGHashMap<const char*, TCLI_Callback>;

	CLIListenerBase();

	bool OnInputProcessing(int argc, JCore::String* argv) override;
private:
	bool CLI_HelpBase(int argc, JCore::String* argv);
	bool CLI_SendCommandFilter(int argc, JCore::String* argv);
	bool CLI_RecvCommandFilter(int argc, JCore::String* argv);
	bool CLI_ViewSendCommand(int argc, JCore::String* argv);
	bool CLI_ViewRecvCommand(int argc, JCore::String* argv);
	bool CLI_ViewSendPacketHex(int argc, JCore::String* argv);
	bool CLI_ViewRecvPacketHex(int argc, JCore::String* argv);
	bool CLI_SaveRuntimeConfig(int argc, JCore::String* argv);

	TCLI_Table m_Table;
};