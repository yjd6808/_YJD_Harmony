/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 7:12:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "Config.h"

#include <jc/Logger/ConsoleLogger.h>

#include <sg/ClientInfo.h>
#include <sg/_API/sgapi_Util.h>


#define SG_RUNTIME_CONFIG_FILENAME "runtime_config.json"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigArgs::Load()
{
	const Vector<jc::String>& args = Env::GetArgs();

	jc::String arg = Env::GetArgs().At(1);
	jc::StringTokenizer tokenizer(arg);
	tokenizer.AddDelim('=');
	tokenizer.AddDelim(' ');

	jc::HashMap<jc::String, jc::String> argMap;
	while (tokenizer.HasNext())
	{
		jc::String key = tokenizer.Next();
		if (!tokenizer.HasNext())
			break;
		jc::String value = tokenizer.Next();
		argMap.Insert(key, value);
	}

	if (jc::String* pAssetPath = argMap.Find("assets"))
	{
		jc::String srcDataPath = jc::Path::Combine(*pAssetPath, "src_data");
		jc::String resDataPath = jc::Path::Combine(*pAssetPath, "res_data");

		CONF_SET(CONF_PROP_ASSET_PATH, *pAssetPath);
		CONF_SET(CONF_PROP_SRC_DATA_PATH, srcDataPath);
		CONF_SET(CONF_PROP_RES_DATA_PATH, resDataPath);
		CONF_SET(CONF_PROP_RES_DATA_FONT_PATH, jc::Path::Combine(resDataPath, "font"));
		CONF_SET(CONF_PROP_RES_DATA_IMAGE_PATH, jc::Path::Combine(resDataPath, "image"));
		CONF_SET(CONF_PROP_RES_DATA_SOUND_PATH, jc::Path::Combine(resDataPath, "sound"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigRuntime::ConfigRuntime()
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

//////////////////////////////////////////////////////////////////////////////////////////
ConfigRuntime::~ConfigRuntime()
{
	JCORE_DELETE_SAFE(pClientInfo_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::Load()
{
	if (!File::Exist(SG_RUNTIME_CONFIG_FILENAME))
	{
		return;
	}

	try
	{
		Value root;
		JsonUtil::LoadThrow(SG_RUNTIME_CONFIG_FILENAME, root);

		Value coreRoot = root["core"];
		if (coreRoot.isObject())
		{
			ReadCore(root);
		}
		
		if (sgapi_IsClient())
		{
			Value clientRoot = root["client"];
			if (clientRoot.isObject())
			{
				ReadClient(clientRoot);
			}
		}

		_LogInfo_("런타임 설정파일(%s) 로드완료", SG_RUNTIME_CONFIG_FILENAME);
	}
	catch (std::exception& ex)
	{
		_LogError_("런타임 설정파일(%s) 로드중 오류가 발생하였습니다.\n%s", SG_RUNTIME_CONFIG_FILENAME, ex.what());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::Delete()
{
	if (!File::Exist(SG_RUNTIME_CONFIG_FILENAME))
	{
		_LogInfo_("런타임 설정파일(%s)이 실행 디렉토리에 없습니다.", SG_RUNTIME_CONFIG_FILENAME);
		return;
	}

	bool isDeleted = File::Delete(SG_RUNTIME_CONFIG_FILENAME);
	if (isDeleted)
		_LogInfo_("런타임 설정파일(%s) 삭제완료", SG_RUNTIME_CONFIG_FILENAME);
	else
		_LogWarn_("런타임 설정파일(%s) 삭제실패", SG_RUNTIME_CONFIG_FILENAME);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::Save()
{
	const jc::String exeDirectoryPath = Env::CurrentDirectory();
	const jc::String runtimeConfigPath = jc::Path::Combine(exeDirectoryPath, SG_RUNTIME_CONFIG_FILENAME);

	try
	{
		Value root;

		Value coreRoot;
		WriteCore(coreRoot);
		root["core"] = coreRoot;

		if (sgapi_IsClient())
		{
			Value clientRoot;
			WriteClient(clientRoot);
			root["client"] = clientRoot;
		}
		const std::string content = root.toStyledString();
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

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::ReadCore(Value& _root)
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

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::ReadClient(Json::Value& _clientRoot)
{
	if (pClientInfo_ == nullptr)
	{
		pClientInfo_ = dbg_new ClientInfo();
	}

	pClientInfo_->frameWidth_ = JsonUtil::GetFloatDefault(_clientRoot["frame_width"], 1280);
	pClientInfo_->frameHeight_ = JsonUtil::GetFloatDefault(_clientRoot["frame_height"], 720);
	pClientInfo_->gameResolutionWidth_ = JsonUtil::GetFloatDefault(_clientRoot["game_resolution_width"], 1280);
	pClientInfo_->gameResolutionHeight_ = JsonUtil::GetFloatDefault(_clientRoot["game_resolution_height"], 720);
	pClientInfo_->gameResolutionPolicy_ = JsonUtil::GetIntDefault(_clientRoot["game_resolution_policy"], 0);
	pClientInfo_->uiResolutionWidth_ = JsonUtil::GetFloatDefault(_clientRoot["ui_resolution_width"], 640);
	pClientInfo_->uiResolutionHeight_ = JsonUtil::GetFloatDefault(_clientRoot["ui_resolution_height"], 480);
	pClientInfo_->resizable_ = JsonUtil::GetBooleanDefault(_clientRoot["resizable"]);
	pClientInfo_->fullScreen_ = JsonUtil::GetBooleanDefault(_clientRoot["fullscreen"]);

	// 접속 서버
	pClientInfo_->remoteAuth_ = jnet::IPv4EndPoint::Parse(JsonUtil::GetStringOrDefault(_clientRoot["remote_auth"]));
	pClientInfo_->remoteLobby_ = jnet::IPv4EndPoint::Parse(JsonUtil::GetStringOrDefault(_clientRoot["remote_lobby"]));
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::WriteCore(Value& _root)
{
	{
		// JCORE_LOCK_GUARD(FilterLock);
		recvCommandFilter_.ForEach([&_root](Cmd_t _cmd)
		{
			_root[RECV_COMMAND_FILTER_KEY].append(_cmd);
		});

		sendCommandFilter_.ForEach([&_root](Cmd_t _cmd)
		{
			_root[SEND_COMMAND_FILTER_KEY].append(_cmd);
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

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::WriteClient(Json::Value& _clientRoot)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::ShowCommandFilter(jnet::Transmission _transmission)
{
	jc::String commands{ 1024 };
	jc::HashSet<Cmd_t>& filter = _transmission == jnet::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;

	{
		// JCORE_LOCK_GUARD(FilterLock);
		int i = 0;
		filter.ForEach([&](Cmd_t _cmd)
		{
			if (i != 0 && i % 10 == 0)
			{
				commands += '\n';
			}

			commands += _cmd;
			++i;
		});
	}

	Console::WriteLine(commands.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::FilterCommand(jnet::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	jc::HashSet<Cmd_t>& filter = _transmission == jnet::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	filter.Insert(_cmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::UnfilterCommand(jnet::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	jc::HashSet<Cmd_t>& filter = _transmission == jnet::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	filter.Remove(_cmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConfigRuntime::IsFilteredCommand(jnet::Transmission _transmission, Cmd_t _cmd)
{
	// JCORE_LOCK_GUARD(FilterLock);
	jc::HashSet<Cmd_t>& filter = _transmission == jnet::Transmission::Send ? sendCommandFilter_ : recvCommandFilter_;
	return filter.Exist(_cmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::ApplyLoggerOption()
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

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigRuntime::ApplyNetLoggerOption()
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
