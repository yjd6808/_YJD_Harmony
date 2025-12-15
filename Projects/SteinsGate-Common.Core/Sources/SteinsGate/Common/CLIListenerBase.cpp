/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "CLIListenerBase.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
CLIListenerBase::CLIListenerBase()
{
	cliTable_.Insert("help", JCORE_CALLBACK_2(CLIListenerBase::CLI_HelpBase, this));
	cliTable_.Insert(RuntimeConfigBase::SEND_COMMAND_FILTER_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_SendCommandFilter, this));
	cliTable_.Insert(RuntimeConfigBase::RECV_COMMAND_FILTER_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_RecvCommandFilter, this));
	cliTable_.Insert(RuntimeConfigBase::SHOW_SEND_COMMAND_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowSendCommand, this));
	cliTable_.Insert(RuntimeConfigBase::SHOW_RECV_COMMAND_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowRecvCommand, this));
	cliTable_.Insert(RuntimeConfigBase::SHOW_SEND_PACKET_HEX_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowSendPacketHex, this));
	cliTable_.Insert(RuntimeConfigBase::SHOW_RECV_PACKET_HEX_KEY,
	                 JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowRecvPacketHex, this));
	cliTable_.Insert("console_log", JCORE_CALLBACK_2(CLIListenerBase::CLI_ConsoleLog, this));
	cliTable_.Insert("console_net_log", JCORE_CALLBACK_2(CLIListenerBase::CLI_ConsoleNetLog, this));
	cliTable_.Insert("runtime_config", JCORE_CALLBACK_2(CLIListenerBase::CLI_RuntimeConfig, this));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::ExecuteCommand(int _argc, String* _pArgv, JCORE_REF_IN TCLI_Table& _table)
{
	const TCLI_Callback* pCallback = _table.Find(_pArgv[0].Source());
	if (pCallback)
	{
		return (*pCallback)(_argc, _pArgv);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::OnInputProcessing(int _argc, String* _pArgv)
{
	return ExecuteCommand(_argc, _pArgv, cliTable_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_HelpBase(int /*_argc*/, JCore::String* /*_pArgv*/)
{
	String helpText{ 2048 };
	helpText += " - send_command_filter [add|remove] [커맨드 ID]\n";
	helpText += " - recv_command_filter [add|remove] [커맨드 ID]\n";
	helpText += " - send_command_filter [show]\n";
	helpText += " - recv_command_filter [show]\n";
	helpText += "   송/수신 커맨드의 출력 로그에서 제외할 커맨드 ID를 등록하거나 제거합니다.\n";
	helpText += " - show_send_command [1|0]\n";
	helpText += " - show_recv_command [1|0]\n";
	helpText += "   송/수신한 커맨드 정보를 출력합니다.\n";
	helpText += " - show_send_packet_hex [1|0]\n";
	helpText += " - show_recv_packet_hex [1|0]\n";
	helpText += "   송/수신한 패킷의 바이너리 데이터를 16진수 형식으로 출력합니다.\n";
	helpText += " - console_log [debug|warn|error|info|normal] enable [1|0]\n";
	helpText += " - console_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]\n";
	helpText += " - console_net_log [debug|warn|error|info|normal] enable [1|0]\n";
	helpText += " - console_net_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]\n";
	helpText += "\t\t콘솔 색상 문자열\n";
	helpText += "\t\t* black\n";
	helpText += "\t\t* blue\n";
	helpText += "\t\t* green\n";
	helpText += "\t\t* cyan\n";
	helpText += "\t\t* red\n";
	helpText += "\t\t* magenta\n";
	helpText += "\t\t* yellow\n";
	helpText += "\t\t* lightgray\n";
	helpText += "\t\t* gray\n";
	helpText += "\t\t* lightblue\n";
	helpText += "\t\t* lightgreen\n";
	helpText += "\t\t* lightcyan\n";
	helpText += "\t\t* lightred\n";
	helpText += "\t\t* lightmagenta\n";
	helpText += "\t\t* lightyellow\n";
	helpText += "\t\t* white\n";
	helpText += " - runtime_config [save|delete]: 런타임 설정을 저장 또는 삭제합니다.";
	Console::WriteLine(helpText.Source());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_SendCommandFilter(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[1] == "add")
	{
		if (_argc <= 2)
		{
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t commandId;
		if (!StringUtil::TryToNumber(commandId, _pArgv[2].Source()))
		{
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->FilterCommand(JNetwork::Transmission::Send, commandId);
		Console::WriteLine("송신 커맨드 필터에 %d커맨드 추기완료", commandId);
	}
	else if (_pArgv[1] == "remove")
	{
		if (_argc <= 2)
		{
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t commandId;
		if (!StringUtil::TryToNumber(commandId, _pArgv[2].Source()))
		{
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->UnfilterCommand(JNetwork::Transmission::Send, commandId);
		Console::WriteLine("송신 커맨드 필터에서 %d커맨드 제거완료", commandId);
	}
	else if (_pArgv[1] == "show")
	{
		Core::RuntimeConfigBase->ShowCommandFilter(JNetwork::Transmission::Send);
	}
	else
	{
		Console::WriteLine("올바르지 않은 명령입니다.");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_RecvCommandFilter(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[1] == "add")
	{
		if (_argc <= 2)
		{
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t commandId;
		if (!StringUtil::TryToNumber(commandId, _pArgv[2].Source()))
		{
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->FilterCommand(JNetwork::Transmission::Recv, commandId);
		Console::WriteLine("수신 커맨드 필터에 %d커맨드 추가완료", commandId);
	}
	else if (_pArgv[1] == "remove")
	{
		if (_argc <= 2)
		{
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t commandId;
		if (!StringUtil::TryToNumber(commandId, _pArgv[2].Source()))
		{
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->UnfilterCommand(JNetwork::Transmission::Recv, commandId);
		Console::WriteLine("수신 커맨드 필터에서 %d커맨드 제거완료", commandId);
	}
	else if (_pArgv[1] == "show")
	{
		Core::RuntimeConfigBase->ShowCommandFilter(JNetwork::Transmission::Recv);
	}
	else
	{
		Console::WriteLine("올바르지 않은 명령입니다.");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ShowSendCommand(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	Core::RuntimeConfigBase->showSendCommand_ = enable;

	if (enable)
	{
		Console::WriteLine("송신 커맨드 보기 활성화");
	}
	else
	{
		Console::WriteLine("송신 커맨드 보기 비활성화");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ShowRecvCommand(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	Core::RuntimeConfigBase->showRecvCommand_ = enable;

	if (enable)
	{
		Console::WriteLine("수신 커맨드 보기 활성화");
	}
	else
	{
		Console::WriteLine("수신 커맨드 보기 비활성화");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ShowSendPacketHex(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	Core::RuntimeConfigBase->showSendPacketHex_ = enable;

	if (enable)
	{
		Console::WriteLine("송신 패킷 헥스 보기 활성화");
	}
	else
	{
		Console::WriteLine("송신 커맨드 헥스 보기 활성화");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ShowRecvPacketHex(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	Core::RuntimeConfigBase->showRecvPacketHex_ = enable;

	if (enable)
	{
		Console::WriteLine("수신 패킷 헥스 보기 활성화");
	}
	else
	{
		Console::WriteLine("수신 커맨드 헥스 보기 활성화");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ConsoleLog(int _argc, JCore::String* _pArgv)
{
	if (_argc <= 3)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level logLevel = ConvertLogLevel(_pArgv[1]);
	if (logLevel == LoggerAbstract::eMax)
	{
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[2] == "enable")
	{
		const int enable = ToNumber(_pArgv[3], 3);
		if (enable == InvalidValue_v)
		{
			return false;
		}
		Core::RuntimeConfigBase->showConsoleLog_[logLevel] = enable;
		Core::RuntimeConfigBase->ApplyLoggerOption();
		return false;
	}

	if (_pArgv[2] == "color")
	{
		ConsoleColor consoleColor = Console::ConvertColorString(_pArgv[3]);
		if (consoleColor == ConsoleColor::Max)
		{
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		Core::RuntimeConfigBase->consoleLogColor_[logLevel] = consoleColor;
		Core::RuntimeConfigBase->ApplyLoggerOption();
		return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_ConsoleNetLog(int _argc, String* _pArgv)
{
	if (_argc <= 3)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level logLevel = ConvertLogLevel(_pArgv[1]);
	if (logLevel == LoggerAbstract::eMax)
	{
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[2] == "enable")
	{
		const int enable = ToNumber(_pArgv[3], 3);
		if (enable == InvalidValue_v)
		{
			return false;
		}
		Core::RuntimeConfigBase->showConsoleNetLog_[logLevel] = enable;
		Core::RuntimeConfigBase->ApplyNetLoggerOption();
		return false;
	}

	if (_pArgv[2] == "color")
	{
		ConsoleColor consoleColor = Console::ConvertColorString(_pArgv[3]);
		if (consoleColor == ConsoleColor::Max)
		{
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		Core::RuntimeConfigBase->consoleNetLogColor_[logLevel] = consoleColor;
		Core::RuntimeConfigBase->ApplyNetLoggerOption();
		return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLIListenerBase::CLI_RuntimeConfig(int _argc, String* _pArgv)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[1] == "save")
	{
		Core::RuntimeConfigBase->Save();
		return false;
	}

	if (_pArgv[1] == "delete")
	{
		Core::RuntimeConfigBase->Delete();
		return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
LoggerAbstract::Level CLIListenerBase::ConvertLogLevel(const SGString& _logLevelString)
{
	if (_logLevelString == "debug") return LoggerAbstract::eDebug;
	if (_logLevelString == "warn") return LoggerAbstract::eWarn;
	if (_logLevelString == "error") return LoggerAbstract::eError;
	if (_logLevelString == "info") return LoggerAbstract::eInfo;
	if (_logLevelString == "normal") return LoggerAbstract::eInfo;
	return LoggerAbstract::eMax;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int CLIListenerBase::ToNumber(const SGString& _numString, int _argIndex)
{
	int value;
	if (!StringUtil::TryToNumber(value, _numString.Source()))
	{
		Console::WriteLine("%d번째 인자가 올바르지 않습니다.", _argIndex + 1);
		return InvalidValue_v;
	}
	return value;
}
