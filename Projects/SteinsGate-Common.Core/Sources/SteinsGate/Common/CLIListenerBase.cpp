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

CLIListenerBase::CLIListenerBase() {
	m_Table.Insert("help",										JCORE_CALLBACK_2(CLIListenerBase::CLI_HelpBase, this));
	m_Table.Insert(RuntimeConfigBase::SendCommandFilterKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_SendCommandFilter, this));
	m_Table.Insert(RuntimeConfigBase::RecvCommandFilterKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_RecvCommandFilter, this));
	m_Table.Insert(RuntimeConfigBase::ShowSendCommandKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowSendCommand, this));
	m_Table.Insert(RuntimeConfigBase::ShowRecvCommandKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowRecvCommand, this));
	m_Table.Insert(RuntimeConfigBase::ShowSendPacketHexKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowSendPacketHex, this));
	m_Table.Insert(RuntimeConfigBase::ShowRecvPacketHexKey,		JCORE_CALLBACK_2(CLIListenerBase::CLI_ShowRecvPacketHex, this));
	m_Table.Insert("console_log",								JCORE_CALLBACK_2(CLIListenerBase::CLI_ConsoleLog, this));
	m_Table.Insert("console_net_log",							JCORE_CALLBACK_2(CLIListenerBase::CLI_ConsoleNetLog, this));
	m_Table.Insert("runtime_config",							JCORE_CALLBACK_2(CLIListenerBase::CLI_RuntimeConfig, this));
}

bool CLIListenerBase::OnInputProcessing(int argc, String* argv) {
	const TCLI_Callback* pCallback = m_Table.Find(argv[0].Source());
	if (pCallback) {
		return (*pCallback)(argc, argv);
	}

	return true;
}

bool CLIListenerBase::CLI_HelpBase(int argc, JCore::String* argv) {
	String szHelpText{ 2048 };
	szHelpText += " - send_command_filter [add|remove] [커맨드 ID]\n";
	szHelpText += " - recv_command_filter [add|remove] [커맨드 ID]\n";
	szHelpText += " - send_command_filter [show]\n";
	szHelpText += " - recv_command_filter [show]\n";
	szHelpText += "   송/수신 커맨드의 출력 로그에서 제외할 커맨드 ID를 등록하거나 제거합니다.\n";
	szHelpText += " - show_send_command [1|0]\n";
	szHelpText += " - show_recv_command [1|0]\n";
	szHelpText += "   송/수신한 커맨드 정보를 출력합니다.\n";
	szHelpText += " - show_send_packet_hex [1|0]\n";
	szHelpText += " - show_recv_packet_hex [1|0]\n";
	szHelpText += "   송/수신한 패킷의 바이너리 데이터를 16진수 형식으로 출력합니다.\n";
	szHelpText += " - console_log [debug|warn|error|info|normal] enable [1|0]\n";
	szHelpText += " - console_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]\n";
	szHelpText += " - console_net_log [debug|warn|error|info|normal] enable [1|0]\n";
	szHelpText += " - console_net_log [debug|warn|error|info|normal] color [콘솔 색상 영어 문자열]\n";
	szHelpText += "\t\t콘솔 색상 문자열\n";
	szHelpText += "\t\t* black\n";
	szHelpText += "\t\t* blue\n";
	szHelpText += "\t\t* green\n";
	szHelpText += "\t\t* cyan\n";
	szHelpText += "\t\t* red\n";
	szHelpText += "\t\t* magenta\n";
	szHelpText += "\t\t* yellow\n";
	szHelpText += "\t\t* lightgray\n";
	szHelpText += "\t\t* gray\n";
	szHelpText += "\t\t* lightblue\n";
	szHelpText += "\t\t* lightgreen\n";
	szHelpText += "\t\t* lightcyan\n";
	szHelpText += "\t\t* lightred\n";
	szHelpText += "\t\t* lightmagenta\n";
	szHelpText += "\t\t* lightyellow\n";
	szHelpText += "\t\t* white\n";
	szHelpText += " - runtime_config [save|delete]: 런타임 설정을 저장 또는 삭제합니다.";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListenerBase::CLI_SendCommandFilter(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (argv[1] == "add") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->FilterCommand(JNetwork::Transmission::Send, iCmd);
		Console::WriteLine("송신 커맨드 필터에 %d커맨드 추기완료", iCmd);
	} else if (argv[1] == "remove") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->UnfilterCommand(JNetwork::Transmission::Send, iCmd);
		Console::WriteLine("송신 커맨드 필터에서 %d커맨드 제거완료", iCmd);
	} else if (argv[1] == "show") {
		Core::RuntimeConfigBase->ShowCommandFilter(JNetwork::Transmission::Send);
	} else {
		Console::WriteLine("올바르지 않은 명령입니다.");
	}
	return false;
}

bool CLIListenerBase::CLI_RecvCommandFilter(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (argv[1] == "add") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->FilterCommand(JNetwork::Transmission::Recv, iCmd);
		Console::WriteLine("수신 커맨드 필터에 %d커맨드 추가완료", iCmd);

	} else if (argv[1] == "remove") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		Core::RuntimeConfigBase->UnfilterCommand(JNetwork::Transmission::Recv, iCmd);
		Console::WriteLine("수신 커맨드 필터에서 %d커맨드 제거완료", iCmd);
	} else if (argv[1] == "show") {
		Core::RuntimeConfigBase->ShowCommandFilter(JNetwork::Transmission::Recv);
	} else {
		Console::WriteLine("올바르지 않은 명령입니다.");
	}
	return false;
}

bool CLIListenerBase::CLI_ShowSendCommand(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int bEnable = ToNumber(argv[1], 1);
	if (bEnable == InvalidValue_v) {
		return false;
	}

	Core::RuntimeConfigBase->ShowSendCommand = bEnable;

	if (bEnable) {
		Console::WriteLine("송신 커맨드 보기 활성화");
	}
	else {
		Console::WriteLine("송신 커맨드 보기 비활성화");
	}
	return false;
}

bool CLIListenerBase::CLI_ShowRecvCommand(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int bEnable = ToNumber(argv[1], 1);
	if (bEnable == InvalidValue_v) {
		return false;
	}

	Core::RuntimeConfigBase->ShowRecvCommand = bEnable;

	if (bEnable) {
		Console::WriteLine("수신 커맨드 보기 활성화");
	}
	else {
		Console::WriteLine("수신 커맨드 보기 비활성화");
	}
	return false;
}

bool CLIListenerBase::CLI_ShowSendPacketHex(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int bEnable = ToNumber(argv[1], 1);
	if (bEnable == InvalidValue_v) {
		return false;
	}

	Core::RuntimeConfigBase->ShowSendPacketHex = bEnable;

	if (bEnable) {
		Console::WriteLine("송신 패킷 헥스 보기 활성화");
	} else {
		Console::WriteLine("송신 커맨드 헥스 보기 활성화");
	}

	return false;
}

bool CLIListenerBase::CLI_ShowRecvPacketHex(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const int bEnable = ToNumber(argv[1], 1);
	if (bEnable == InvalidValue_v) {
		return false;
	}

	Core::RuntimeConfigBase->ShowRecvPacketHex = bEnable;

	if (bEnable) {
		Console::WriteLine("수신 패킷 헥스 보기 활성화");
	} else {
		Console::WriteLine("수신 커맨드 헥스 보기 활성화");
	}
	return false;
}

bool CLIListenerBase::CLI_ConsoleLog(int argc, JCore::String* argv) {
	if (argc <= 3) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level eLevel = ConvertLogLevel(argv[1]);
	if (eLevel == LoggerAbstract::eMax) {
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (argv[2] == "enable") {
		const int bEnable = ToNumber(argv[3], 3);
		if (bEnable == InvalidValue_v) {
			return false;
		}
		Core::RuntimeConfigBase->ShowConsoleLog[eLevel] = bEnable;
		Core::RuntimeConfigBase->ApplyLoggerOption();
		return false;
	} 

	if (argv[2] == "color") {
		ConsoleColor eColor = Console::ConvertColorString(argv[3]);
		if (eColor == ConsoleColor::Max) {
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		Core::RuntimeConfigBase->ConsoleLogColor[eLevel] = eColor;
		Core::RuntimeConfigBase->ApplyLoggerOption();
		return false;
	}

	return false;
}

bool CLIListenerBase::CLI_ConsoleNetLog(int argc, String* argv) {
	if (argc <= 3) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	const LoggerAbstract::Level eLevel = ConvertLogLevel(argv[1]);
	if (eLevel == LoggerAbstract::eMax) {
		Console::WriteLine("두번째 인자가 올바르지 않습니다.");
		return false;
	}

	if (argv[2] == "enable") {
		const int bEnable = ToNumber(argv[3], 3);
		if (bEnable == InvalidValue_v) {
			return false;
		}
		Core::RuntimeConfigBase->ShowConsoleNetLog[eLevel] = bEnable;
		Core::RuntimeConfigBase->ApplyNetLoggerOption();
		return false;
	}

	if (argv[2] == "color") {
		ConsoleColor eColor = Console::ConvertColorString(argv[3]);
		if (eColor == ConsoleColor::Max) {
			Console::WriteLine("색상 문자열이 올바르지 않습니다.");
		}
		Core::RuntimeConfigBase->ConsoleNetLogColor[eLevel] = eColor;
		Core::RuntimeConfigBase->ApplyNetLoggerOption();
		return false;
	}

	return false;
}

bool CLIListenerBase::CLI_RuntimeConfig(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}


	if (argv[1] == "save") {
		Core::RuntimeConfigBase->Save();
		return false;
	}

	if (argv[1] == "delete") {
		Core::RuntimeConfigBase->Delete();
		return false;
	}
	
	return false;
}


// ===========================================================================================================

LoggerAbstract::Level CLIListenerBase::ConvertLogLevel(const SGString& logLevelString) {
	if (logLevelString == "debug") return LoggerAbstract::eDebug;
	if (logLevelString == "warn") return LoggerAbstract::eWarn;
	if (logLevelString == "error") return LoggerAbstract::eError;
	if (logLevelString == "info") return LoggerAbstract::eInfo;
	if (logLevelString == "normal") return LoggerAbstract::eInfo;
	return LoggerAbstract::eMax;
}

int CLIListenerBase::ToNumber(const SGString& numString, int argIndex) {
	int v;
	if (!StringUtil::TryToNumber(v, numString.Source())) {
		Console::WriteLine("%d번째 인자가 올바르지 않습니다.", argIndex + 1);
		return InvalidValue_v;
	}
	return v;
}
