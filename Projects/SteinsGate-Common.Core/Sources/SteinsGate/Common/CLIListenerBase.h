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
	bool OnInputProcessing(int argc, JCore::String* argv) override;

	bool CLI_HelpBase();
	bool CLI_SendCommandFilter(int argc, JCore::String* argv);
	bool CLI_RecvCommandFilter(int argc, JCore::String* argv);
	bool CLI_ViewSendCommand(int argc, JCore::String* argv);
	bool CLI_ViewRecvCommand(int argc, JCore::String* argv);
	bool CLI_ViewSendPacketHex(int argc, JCore::String* argv);
	bool CLI_ViewRecvPacketHex(int argc, JCore::String* argv);
};