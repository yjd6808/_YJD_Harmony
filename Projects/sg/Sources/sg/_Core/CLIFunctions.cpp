/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "CLIFunctions_Linker.h"
#include "CLIListener.h"
#include "AppConfig.h"

USING_NS_JC;

void CLI_ForceLinkage() {}

REGISTER_CLI_FUNCTION("help", CLI_HelpBase,
	"도움말을 출력합니다.\n"
	"- help\n"
	"- help [함수명]"
)

REGISTER_CLI_FUNCTION(AppConfig::SEND_COMMAND_FILTER_KEY, CLI_SendCommandFilter,
	"송신 커맨드 필터를 설정합니다.\n"
	"- send_command_filter [add|remove] [커맨드 ID]\n"
	"- send_command_filter [show]"
)

REGISTER_CLI_FUNCTION(AppConfig::RECV_COMMAND_FILTER_KEY, CLI_RecvCommandFilter,
	"수신 커맨드 필터를 설정합니다.\n"
	"- recv_command_filter [add|remove] [커맨드 ID]\n"
	"- recv_command_filter [show]"
)

REGISTER_CLI_FUNCTION(AppConfig::SHOW_SEND_COMMAND_KEY, CLI_ShowSendCommand,
	"송신 커맨드 로그 출력 여부를 설정합니다.\n"
	"- show_send_command [1|0]"
)

REGISTER_CLI_FUNCTION(AppConfig::SHOW_RECV_COMMAND_KEY, CLI_ShowRecvCommand,
	"수신 커맨드 로그 출력 여부를 설정합니다.\n"
	"- show_recv_command [1|0]"
)

REGISTER_CLI_FUNCTION(AppConfig::SHOW_SEND_PACKET_HEX_KEY, CLI_ShowSendPacketHex,
	"송신 패킷 16진수 로그 출력 여부를 설정합니다.\n"
	" - show_send_packet_hex [1|0]"
)

REGISTER_CLI_FUNCTION(AppConfig::SHOW_RECV_PACKET_HEX_KEY, CLI_ShowRecvPacketHex,
	"수신 패킷 16진수 로그 출력 여부를 설정합니다.\n"
	" - show_recv_packet_hex [1|0]"
)

REGISTER_CLI_FUNCTION("console_log", CLI_ConsoleLog,
	"콘솔 로그 설정을 변경합니다.\n"
	"- console_log [debug|warn|error|info|normal] enable [1|0]"
	"- console_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]"
)

REGISTER_CLI_FUNCTION("console_net_log", CLI_ConsoleNetLog,
	"콘솔 네트워크 로그 설정을 변경합니다.\n"
	"- console_net_log [debug|warn|error|info|normal] enable [1|0]"
	"- console_net_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]"
)

REGISTER_CLI_FUNCTION("runtime_config", CLI_AppConfig,
	"런타임 설정을 저장 또는 삭제합니다.\n"
	"- runtime_config [save|delete]"
)


//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_SendCommandFilter(int _argc, String* _pArgv, OUT jc::String& _error)
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

		g_cAppConfig.FilterCommand(jnet::Transmission::Send, commandId);
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

		g_cAppConfig.UnfilterCommand(jnet::Transmission::Send, commandId);
		Console::WriteLine("송신 커맨드 필터에서 %d커맨드 제거완료", commandId);
	}
	else if (_pArgv[1] == "show")
	{
		g_cAppConfig.ShowCommandFilter(jnet::Transmission::Send);
	}
	else
	{
		Console::WriteLine("올바르지 않은 명령입니다.");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_RecvCommandFilter(int _argc, String* _pArgv, OUT jc::String& _error)
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

		g_cAppConfig.FilterCommand(jnet::Transmission::Recv, commandId);
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

		g_cAppConfig.UnfilterCommand(jnet::Transmission::Recv, commandId);
		Console::WriteLine("수신 커맨드 필터에서 %d커맨드 제거완료", commandId);
	}
	else if (_pArgv[1] == "show")
	{
		g_cAppConfig.ShowCommandFilter(jnet::Transmission::Recv);
	}
	else
	{
		Console::WriteLine("올바르지 않은 명령입니다.");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ShowSendCommand(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = CLIListener::ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	g_cAppConfig.showSendCommand_ = enable;

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

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ShowRecvCommand(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = CLIListener::ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	g_cAppConfig.showRecvCommand_ = enable;

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

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ShowSendPacketHex(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = CLIListener::ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	g_cAppConfig.showSendPacketHex_ = enable;

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

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ShowRecvPacketHex(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int enable = CLIListener::ToNumber(_pArgv[1], 1);
	if (enable == InvalidValue_v)
	{
		return false;
	}

	g_cAppConfig.showRecvPacketHex_ = enable;

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

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ConsoleLog(int _argc, jc::String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 3)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level logLevel = LoggerAbstract::ConvertLogLevel(_pArgv[1]);
	if (logLevel == LoggerAbstract::eMax)
	{
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[2] == "enable")
	{
		const int enable = CLIListener::ToNumber(_pArgv[3], 3);
		if (enable == InvalidValue_v)
		{
			return false;
		}
		g_cAppConfig.showConsoleLog_[logLevel] = enable;
		g_cAppConfig.ApplyLoggerOption();
		return false;
	}
	else if (_pArgv[2] == "color")
	{
		ConsoleColor consoleColor = Console::ConvertColorString(_pArgv[3]);
		if (consoleColor == ConsoleColor::Max)
		{
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		g_cAppConfig.consoleLogColor_[logLevel] = consoleColor;
		g_cAppConfig.ApplyLoggerOption();
		return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_ConsoleNetLog(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 3)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level logLevel = LoggerAbstract::ConvertLogLevel(_pArgv[1]);
	if (logLevel == LoggerAbstract::eMax)
	{
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[2] == "enable")
	{
		const int enable = CLIListener::ToNumber(_pArgv[3], 3);
		if (enable == InvalidValue_v)
		{
			return false;
		}
		g_cAppConfig.showConsoleNetLog_[logLevel] = enable;
		g_cAppConfig.ApplyNetLoggerOption();
		return false;
	}

	if (_pArgv[2] == "color")
	{
		ConsoleColor consoleColor = Console::ConvertColorString(_pArgv[3]);
		if (consoleColor == ConsoleColor::Max)
		{
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		g_cAppConfig.consoleNetLogColor_[logLevel] = consoleColor;
		g_cAppConfig.ApplyNetLoggerOption();
		return false;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_AppConfig(int _argc, String* _pArgv, OUT jc::String& _error)
{
	if (_argc <= 1)
	{
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (_pArgv[1] == "save")
	{
		g_cAppConfig.SaveConfiFile();
		return false;
	}

	if (_pArgv[1] == "delete")
	{
		g_cAppConfig.DeleteConfFile();
		return false;
	}

	return false;
}
