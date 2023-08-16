/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 7:12:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCoreHeader.h"
#include "RuntimeConfigBase.h"

#include <SteinsGate/Common/JsonUtil.h>

#define SG_RUNTIME_CONFIG_FILENAME "runtime_config.json"

USING_NS_JC;
USING_NS_JS;

RuntimeConfigBase::RuntimeConfigBase()
	: RecvCommandFilter(512)
	, SendCommandFilter(512)
	, ViewRecvCommand(false)
	, ViewSendCommand(false)
	, ViewRecvPacketHex(false)
	, ViewSendPacketHex(false)
{}

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

void RuntimeConfigBase::Save() {
	const SGString szExeDirectoryPath = Env::CurrentDirectory();
	const SGString szRuntimeConfigPath = JCore::Path::Combine(szExeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	try {
		Value value;
		OnSaving(value);

		const std::string content = value.toStyledString();
		File::WriteAllText(content.c_str(), content.length(), szRuntimeConfigPath.Source());
		_LogInfo_("런타임 설정파일(%s) 저장완료", SG_RUNTIME_CONFIG_FILENAME);
	} catch (std::exception& ex) {
		_LogError_("런타임 설정파일(%s) 저장중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

void RuntimeConfigBase::ReadCore(Value& root) {

	for (Value& v : root[RecvCommandFilterKey]) {
		RecvCommandFilter.PushBack(v.asInt());
	}

	for (Value& v : root[SendCommandFilterKey]) {
		SendCommandFilter.PushBack(v.asInt());
	}

	ViewRecvCommand = root[ViewRecvCommandKey].asBool();
	ViewSendCommand = root[ViewSendCommandKey].asBool();

	ViewRecvPacketHex = root[ViewRecvPacketHexKey].asBool();
	ViewSendPacketHex = root[ViewSendPacketHexKey].asBool();
}

void RuntimeConfigBase::WriteCore(Value& root) {
	for (int i = 0; i < RecvCommandFilter.Size(); ++i) {
		root[RecvCommandFilterKey].append(RecvCommandFilter[i]);
	}

	for (int i = 0; i < SendCommandFilter.Size(); ++i) {
		root[SendCommandFilterKey].append(SendCommandFilter[i]);
	}

	root[ViewRecvCommandKey] = ViewRecvCommand;
	root[ViewSendCommandKey] = ViewSendCommand;

	root[ViewRecvPacketHexKey] = ViewRecvPacketHex;
	root[ViewSendPacketHexKey] = ViewSendPacketHex;
}



void RuntimeConfigBase::ShowCommandFilter(JNetwork::Transmission transmission) {
	SGString szCommands{ 1024 };
	const SGVector<Cmd_t>& vFilter = transmission == JNetwork::Transmission::Send ? SendCommandFilter : RecvCommandFilter;

	for (int i = 0; i < vFilter.Size(); ++i) {
		if (i != 0 && i % 10 == 0) {
			szCommands += '\n';
		}

		szCommands += vFilter[i];
	}

	Console::WriteLine(szCommands.Source());
}

