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
	: RecvCommandFilter(512)
	, SendCommandFilter(512)
	, ShowRecvCommand(true)
	, ShowSendCommand(true)
	, ShowRecvPacketHex(false)
	, ShowSendPacketHex(false)
	, ShowConsoleLog{ true, true, true, true, true }
	, ShowConsoleNetLog{ true, true, true, true, true } {

	Arrays::Copy(ConsoleLogColor, ConsoleLoggerOption::Default.LogColors);
	Arrays::Copy(ConsoleNetLogColor, ConsoleLoggerOption::Default.LogColors);

}

void RuntimeConfigBase::Load() {

	const SGString szExeDirectoryPath = Env::CurrentDirectory();
	const SGString szRuntimeConfigPath = JCore::Path::Combine(szExeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	if (!File::Exist(szRuntimeConfigPath)) {
		return;
	}

	try {
		Value root;
		JsonUtil::loadThrow(szRuntimeConfigPath, root);
		OnLoading(root);
		_LogInfo_("런타임 설정파일(%s) 로드완료", SG_RUNTIME_CONFIG_FILENAME);
	} catch (std::exception& ex) {
		_LogError_("런타임 설정파일(%s) 로드중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

void RuntimeConfigBase::Delete() {
	const SGString szExeDirectoryPath = Env::CurrentDirectory();
	const SGString szRuntimeConfigPath = JCore::Path::Combine(szExeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	if (!File::Exist(szRuntimeConfigPath)) {
		_LogInfo_("런타임 설정파일(%s)이 실행 디렉토리에 없습니다.", SG_RUNTIME_CONFIG_FILENAME);
		return;
	}

	bool bDeleted = File::Delete(szRuntimeConfigPath);
	if (bDeleted) _LogInfo_("런타임 설정파일(%s) 삭제완료", SG_RUNTIME_CONFIG_FILENAME);
	else _LogWarn_("런타임 설정파일(%s) 삭제실패", SG_RUNTIME_CONFIG_FILENAME);
	
}

void RuntimeConfigBase::Save() {
	const SGString szExeDirectoryPath = Env::CurrentDirectory();
	const SGString szRuntimeConfigPath = JCore::Path::Combine(szExeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	try {
		Value value;
		OnSaving(value);
		const std::string content = value.toStyledString();
		if (content.data() == nullptr) {
			throw std::exception("Value 문자열 변환중 오류 발생");
		}
		File::WriteAllText(content.c_str(), content.length(), szRuntimeConfigPath.Source());
		_LogInfo_("런타임 설정파일(%s) 저장완료", SG_RUNTIME_CONFIG_FILENAME);
	} catch (std::exception& ex) {
		_LogError_("런타임 설정파일(%s) 저장중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

void RuntimeConfigBase::ReadCore(Value& root) {
	for (Value& v : root[RecvCommandFilterKey]) {
		RecvCommandFilter.Insert(v.asInt());
	}

	for (Value& v : root[SendCommandFilterKey]) {
		SendCommandFilter.Insert(v.asInt());
	}

	ShowRecvCommand = root[ShowRecvCommandKey].asBool();
	ShowSendCommand = root[ShowSendCommandKey].asBool();

	ShowRecvPacketHex = root[ShowRecvPacketHexKey].asBool();
	ShowSendPacketHex = root[ShowSendPacketHexKey].asBool();

	for (int i = 0; Value & v : root[ConsoleLogColorKey]) {
		ConsoleLogColor[i++] = (ConsoleColor)v.asInt();
	}

	for (int i = 0; Value & v : root[ConsoleNetLogColorKey]) {
		ConsoleNetLogColor[i++] = (ConsoleColor)v.asInt();
	}

	for (int i = 0; Value& v : root[ShowConsoleLogKey]) {
		ShowConsoleLog[i++] = v.asInt();
	}

	for (int i = 0; Value & v : root[ShowConsoleNetLogKey]) {
		ShowConsoleNetLog[i++] = v.asInt();
	}

	ApplyLoggerOption();
	ApplyNetLoggerOption();
}

void RuntimeConfigBase::WriteCore(Value& root) {
	{
		// JCORE_LOCK_GUARD(FilterLock);
		RecvCommandFilter.ForEach([&root](Cmd_t cmd) {
			root[RecvCommandFilterKey].append(cmd);
		});

		SendCommandFilter.ForEach([&root](Cmd_t cmd) {
			root[SendCommandFilterKey].append(cmd);
		});
	}

	root[ShowRecvCommandKey] = ShowRecvCommand;
	root[ShowSendCommandKey] = ShowSendCommand;

	root[ShowRecvPacketHexKey] = ShowRecvPacketHex;
	root[ShowSendPacketHexKey] = ShowSendPacketHex;

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i) {
		root[ConsoleLogColorKey].append(ConsoleLogColor[i]);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i) {
		root[ConsoleNetLogColorKey].append(ConsoleNetLogColor[i]);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i) {
		root[ShowConsoleLogKey].append(ShowConsoleLog[i] ? 1 : 0);
	}

	for (int i = 0; i < LoggerAbstract::Level::eMax; ++i) {
		root[ShowConsoleNetLogKey].append(ShowConsoleLog[i] ? 1 : 0);
	}
}



void RuntimeConfigBase::ShowCommandFilter(JNetwork::Transmission transmission) {
	SGString szCommands{ 1024 };
	SGHashSet<Cmd_t>& filter = transmission == JNetwork::Transmission::Send ? SendCommandFilter : RecvCommandFilter;

	{
		// JCORE_LOCK_GUARD(FilterLock);
		int i = 0;
		filter.ForEach([&](Cmd_t cmd) {
			if (i != 0 && i % 10 == 0) {
				szCommands += '\n';
			}

			szCommands += cmd;
			++i;
		});
	}

	Console::WriteLine(szCommands.Source());
	
}

void RuntimeConfigBase::FilterCommand(JNetwork::Transmission transmission, Cmd_t cmd) {
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = transmission == JNetwork::Transmission::Send ? SendCommandFilter : RecvCommandFilter;
	filter.Insert(cmd);
}

void RuntimeConfigBase::UnfilterCommand(JNetwork::Transmission transmission, Cmd_t cmd) {
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = transmission == JNetwork::Transmission::Send ? SendCommandFilter : RecvCommandFilter;
	filter.Remove(cmd);
}

bool RuntimeConfigBase::IsFilteredCommand(JNetwork::Transmission transmission, Cmd_t cmd) {
	// JCORE_LOCK_GUARD(FilterLock);
	SGHashSet<Cmd_t>& filter = transmission == JNetwork::Transmission::Send ? SendCommandFilter : RecvCommandFilter;
	return filter.Exist(cmd);
}


void RuntimeConfigBase::ApplyLoggerOption() {

	if (Logger_v == nullptr) {
		return;
	}

	LoggerOption* pOption = Logger_v->GetLoggerOption();

	if (pOption->GetLoggerType() == LoggerType::Console) {
		ConsoleLoggerOption* pConsoleOption = dynamic_cast<ConsoleLoggerOption*>(pOption);

		if (pConsoleOption == nullptr) {
			return;
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i) {
			pConsoleOption->EnableLog[i] = ShowConsoleLog[i];
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i) {
			pConsoleOption->LogColors[i] = (ConsoleColor)ConsoleLogColor[i];
		}
	}
}

void RuntimeConfigBase::ApplyNetLoggerOption() {
	if (NetLogger_v == nullptr) {
		return;
	}

	LoggerOption* pOption = NetLogger_v->GetLoggerOption();

	if (pOption->GetLoggerType() == LoggerType::Console) {
		ConsoleLoggerOption* pConsoleOption = dynamic_cast<ConsoleLoggerOption*>(pOption);

		if (pConsoleOption == nullptr) {
			return;
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i) {
			pConsoleOption->EnableLog[i] = ShowConsoleNetLog[i];
		}

		for (int i = 0; i < LoggerAbstract::eMax; ++i) {
			pConsoleOption->LogColors[i] = (ConsoleColor)ConsoleNetLogColor[i];
		}
	}
}
