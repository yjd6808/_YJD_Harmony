#pragma once

class CLIListener
{
public:
	using TCLI_Callback = jc::Func<bool, int, jc::String*>;
	using TCLI_CallbackExternal = jc::Func<bool, CLIListener*, int, jc::String*>;
	using TCLI_Table = jc::HashMap<jc::String, TCLI_Callback>;

	CLIListener();

	void AddCallback(const jc::String& _key, const TCLI_Callback& _callback);

	bool OnInputProcessing(int _argc, jc::String* _pArgv);
private:
	bool CLI_HelpBase(int _argc, jc::String* _argv);
	bool CLI_SendCommandFilter(int _argc, jc::String* _argv);
	bool CLI_RecvCommandFilter(int _argc, jc::String* _argv);
	bool CLI_ShowSendCommand(int _argc, jc::String* _argv);
	bool CLI_ShowRecvCommand(int _argc, jc::String* _argv);
	bool CLI_ShowSendPacketHex(int _argc, jc::String* _argv);
	bool CLI_ShowRecvPacketHex(int _argc, jc::String* _argv);
	bool CLI_ConsoleLog(int _argc, jc::String* _argv);
	bool CLI_ConsoleNetLog(int _argc, jc::String* _argv);
	bool CLI_ConfigRuntime(int _argc, jc::String* _argv);

protected:
	/**
	 * \brief 커맨드 실행
	 * \param _argc 커맨드 이름을 포함한 인자 갯수
	 * \param _pArgv _pArgv[0] 커맨드 이름, _pArgv[1,2, ...] 커맨드 인자
	 * \param _table
	 * \return 이벤트 터널링 여부
	 */
	bool ExecuteCommand(int _argc, jc::String* _pArgv,  TCLI_Table& _table);

	jc::LoggerAbstract::Level ConvertLogLevel(const jc::String& _logLevelString);
	int ToNumber(const jc::String& _numString, int _argIndex);

private:
	TCLI_Table cliTable_;
};
