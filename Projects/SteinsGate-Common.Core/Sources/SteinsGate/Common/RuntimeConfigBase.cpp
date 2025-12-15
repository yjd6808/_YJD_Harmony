/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 7:12:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCoreHeader.h"
#include "RuntimeConfigBase.h"

#include <JCore/Logger/ConsoleLogger.h>

#include <SteinsGate/Common/JsonUtil.h>

#define SG_RUNTIME_CONFIG_FILENAME "runtime_config.json"

USING_NS_JC;
USING_NS_JS;

RuntimeConfigBase::RuntimeConfigBase()
: showRecvCommand_(true)
, showSendCommand_(true)
, showRecvPacketHex_(false)
, showSendPacketHex_(false)
, showConsoleLog_{ true, true, true, true, true }
, showConsoleNetLog_{ true, true, true, true, true }
, recvCommandFilter_(512)
, sendCommandFilter_(512)
{
	Arrays::Copy(consoleLogColor_, ConsoleLoggerOption::Default.LogColors);
	Arrays::Copy(consoleNetLogColor_, ConsoleLoggerOption::Default.LogColors);
}

void RuntimeConfigBase::Load()
{
	const SGString exeDirectoryPath = Env::CurrentDirectory();
	const SGString runtimeConfigPath = JCore::Path::Combine(exeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	if (!File::Exist(runtimeConfigPath))
	{
		return;
	}

	try
	{
		Value root;
		JsonUtil::LoadThrow(runtimeConfigPath, root);
		OnLoading(root);
		_LogInfo_("런타임 설정파일(%s) 로드완료", SG_RUNTIME_CONFIG_FILENAME);
	}
	catch (std::exception& ex)
	{
		_LogError_("런타임 설정파일(%s) 로드중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

void RuntimeConfigBase::Delete()
{
	const SGString exeDirectoryPath = Env::CurrentDirectory();
	const SGString runtimeConfigPath = JCore::Path::Combine(exeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	if (!File::Exist(runtimeConfigPath))
	{
		_LogInfo_("런타임 설정파일(%s)이 실행 디렉토리에 없습니다.", SG_RUNTIME_CONFIG_FILENAME);
		return;
	}

	bool isDeleted = File::Delete(runtimeConfigPath);
	if (isDeleted)
		_LogInfo_("런타임 설정파일(%s) 삭제완료", SG_RUNTIME_CONFIG_FILENAME);
	else
		_LogWarn_("런타임 설정파일(%s) 삭제실패", SG_RUNTIME_CONFIG_FILENAME);
}

void RuntimeConfigBase::Save()
{
	const SGString exeDirectoryPath = Env::CurrentDirectory();
	const SGString runtimeConfigPath = JCore::Path::Combine(exeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	try
	{
		Value value;
		OnSaving(value);
		const std::string content = value.toStyledString();
		if (content.data() == nullptr)
		{
			throw std::exception("Value 문자열 변환중 오류 발생");
		}
		File::WriteAllText(content.c_str(), content.length(), runtimeConfigPath.Source());
		_LogInfo_("런타임 설정파일(%s) 저장완료", SG_RUNTIME_CONFIG_FILENAME);
	}
	catch (std::exception& ex)
	{
		_LogError_("런타임 설정파일(%s) 저장중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

void RuntimeConfigBase::ReadCore(Value& _root)
{
	for (Value& v : _root[RECV_COMMAND_FILTER_KEY])
	{
		recvCommandFilter_.Insert(v.asInt());
	}

	for (Value& v : _root[SEND_COMMAND_FILTER_KEY])
	{
		sendCommandFilter_.Insert(v.asInt());
	}

	showRecvCommand_ = _root[SHOW_RECV_COMMAND_KEY].asBool();
	showSendCommand_ = _root[SHOW_SEND_COMMAND_KEY].asBool();

	showRecvPacketHex_ = _root[SHOW_RECV_PACKET_HEX_KEY].asBool();
	showSendPacketHex_ = _root[SHOW_SEND_PACKET_HEX_KEY].asBool();

	for (int i = 0; Value& v : _root[CONSOLE_LOG_COLOR_KEY])
	{
		consoleLogColor_[i++] = (ConsoleColor)v.asInt();
	}

	for (int i = 0; Value& v : _root[CONSOLE_NET_LOG_COLOR_KEY])
	{
		consoleNetLogColor_[i++] = (ConsoleColor)v.asInt();
	}

	for (int i = 0; Value& v : _root[SHOW_CONSOLE_LOG_KEY])
	{
		showConsoleLog_[i++] = v.asInt();
	}

	for (int i = 0; Value& v : _root[SHOW_CONSOLE_NET_LOG_KEY])
	{
		showConsoleNetLog_[i++] = v.asInt();
	}

	ApplyLoggerOption();
	ApplyNetLoggerOption();
}

void RuntimeConfigBase::WriteCore(Value& _root)
{
	{
		// JCORE_LOCK_GUARD(FilterLock);
		recvCommandFilter_.ForEach([&_root](Cmd_t cmd)
		{
			_root[RECV_COMMAND_FILTER_KEY].append(cmd);
		});

		sendCommandFilter_.ForEach([&_root](Cmd_t cmd)
		{
			_root[SEND_COMMAND_FILTER_KEY].append(cmd);
		});
	}

	_root[SHOW_RECV_COMMAND_KEY] = showRecvCommand_;
	_root[SHOW_SEND_COMMAND_KEY] = showSendCommand_;

	_root[SHOW_RECV_PACKET_HEX_KEY] = showRecvPacketHex_;
	_root[SHOW_SEND_PACKET_HEX_KEY] = showSendPacketHex_;

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i)
	{
		_root[CONSOLE_LOG_COLOR_KEY].append(consoleLogColor_[i]);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i)
	{
		_root[CONSOLE_NET_LOG_COLOR_KEY].append(consoleNetLogColor_[i]);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i)
	{
		_root[SHOW_CONSOLE_LOG_KEY].append(showConsoleLog_[i] ? 1 : 0);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i)
	{
		_root[SHOW_CONSOLE_NET_LOG_KEY].append(showConsoleLog_[i] ? 1 : 0);
	}
}

void RuntimeConfigBase::ShowCommandFilter(JNetwork::Transmission _transmission)
{
	SGString commands{ 1024 };
	SGHashSet<Cmd_t>& filter = _transmission == JNetwork::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;

	{
		// JCORE_LOCK_GUARD(FilterLock);
		int i = 0;
		filter.ForEach([&](Cmd_t cmd)
		{
			if (i != 0 && i % 10 == 0)
			{
				commands += '\n';
			}

			commands += cmd;
			++i;
		});
	}

	Console::WriteLine(commands.Source());
}

void RuntimeConfigBase::FilterCommand(JNetwork::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = _transmission == JNetwork::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	filter.Insert(_cmd);
}

void RuntimeConfigBase::UnfilterCommand(JNetwork::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = _transmission == JNetwork::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	filter.Remove(_cmd);
}

bool RuntimeConfigBase::IsFilteredCommand(JNetwork::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = _transmission == JNetwork::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	return filter.Exist(_cmd);
}

void RuntimeConfigBase::ApplyLoggerOption()
{
	if (Logger_v == nullptr)
	{
		return;
	}

	LoggerOption* pOption = Logger_v->GetLoggerOption();

	if (pOption->GetLoggerType() == LoggerType::Console)
	{
		ConsoleLoggerOption* pConsoleOption = dynamic_cast<ConsoleLoggerOption*>(pOption);

		if (pConsoleOption == nullptr)
		{
			return;
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i)
		{
			pConsoleOption->EnableLog[i] = showConsoleLog_[i];
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i)
		{
			pConsoleOption->LogColors[i] = consoleLogColor_[i];
		}
	}
}

void RuntimeConfigBase::ApplyNetLoggerOption()
{
	if (NetLogger_v == nullptr)
	{
		return;
	}

	LoggerOption* pOption = NetLogger_v->GetLoggerOption();

	if (pOption->GetLoggerType() == LoggerType::Console)
	{
		ConsoleLoggerOption* pConsoleOption = dynamic_cast<ConsoleLoggerOption*>(pOption);

		if (pConsoleOption == nullptr)
		{
			return;
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i)
		{
			pConsoleOption->EnableLog[i] = showConsoleNetLog_[i];
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i)
		{
			pConsoleOption->LogColors[i] = consoleNetLogColor_[i];
		}
	}
}
