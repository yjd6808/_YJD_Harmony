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
	bool CLI_ShowSendCommand(int argc, JCore::String* argv);
	bool CLI_ShowRecvCommand(int argc, JCore::String* argv);
	bool CLI_ShowSendPacketHex(int argc, JCore::String* argv);
	bool CLI_ShowRecvPacketHex(int argc, JCore::String* argv);
	bool CLI_ConsoleLog(int argc, JCore::String* argv);
	bool CLI_ConsoleNetLog(int argc, JCore::String* argv);
	bool CLI_RuntimeConfig(int argc, JCore::String* argv);

protected:
	/**
	 * \brief 커맨드 실행
	 * \param argc 커맨드 이름을 포함한 인자 갯수
	 * \param argv argv[0] 커맨드 이름, argv[1,2, ...] 커맨드 인자
	 * \return 이벤트 터널링 여부
	 */
	bool ExecuteCommand(int argc, SGString* argv, JCORE_REF_IN TCLI_Table& table);

	JCore::LoggerAbstract::Level ConvertLogLevel(const SGString& logLevelString);
	int ToNumber(const SGString& numString, int argIndex);
private:
	TCLI_Table m_Table;
};
