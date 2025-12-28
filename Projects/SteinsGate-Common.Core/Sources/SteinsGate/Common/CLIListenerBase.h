#pragma once

#include <SteinsGate/Common/ICLIListener.h>

class CLIListenerBase : public ICLIListener
{
public:
	using TCLI_Callback = SGFuncFn<bool, int, SGString*>;
	using TCLI_Table = SGHashMap<const char*, TCLI_Callback>;

	CLIListenerBase();

	bool OnInputProcessing(int _argc, JCore::String* _pArgv) override;
private:
	bool CLI_HelpBase(int _argc, JCore::String* _argv);
	bool CLI_SendCommandFilter(int _argc, JCore::String* _argv);
	bool CLI_RecvCommandFilter(int _argc, JCore::String* _argv);
	bool CLI_ShowSendCommand(int _argc, JCore::String* _argv);
	bool CLI_ShowRecvCommand(int _argc, JCore::String* _argv);
	bool CLI_ShowSendPacketHex(int _argc, JCore::String* _argv);
	bool CLI_ShowRecvPacketHex(int _argc, JCore::String* _argv);
	bool CLI_ConsoleLog(int _argc, JCore::String* _argv);
	bool CLI_ConsoleNetLog(int _argc, JCore::String* _argv);
	bool CLI_RuntimeConfig(int _argc, JCore::String* _argv);

protected:
	/**
	 * \brief 커맨드 실행
	 * \param _argc 커맨드 이름을 포함한 인자 갯수
	 * \param _pArgv _pArgv[0] 커맨드 이름, _pArgv[1,2, ...] 커맨드 인자
	 * \param _table
	 * \return 이벤트 터널링 여부
	 */
	bool ExecuteCommand(int _argc, SGString* _pArgv,  TCLI_Table& _table);

	JCore::LoggerAbstract::Level ConvertLogLevel(const SGString& _logLevelString);
	int ToNumber(const SGString& _numString, int _argIndex);

private:
	TCLI_Table cliTable_;
};
