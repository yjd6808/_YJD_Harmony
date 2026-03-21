/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "DescMgr_Action.h"

#include "sg/_Util/JsonUtil.h"

#include "json.h"
#include "fstream"

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
// ActionInfoLoader
ActionInfoLoader::ActionInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigFileType_t ActionInfoLoader::GetConfigFileType()
{
	return ConfigFileType::Action;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Json::Value gunnerRoot = root["gunner"];

		for (ArrayIndex i = 0; i < gunnerRoot.size(); ++i)
		{
			Value& actionRoot = gunnerRoot[i];
			ActionInfo* pActionInfo = dbg_new ActionInfo;
			ReadActionInfo(actionRoot, pActionInfo);
			AddData(pActionInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s\n", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionInfoLoader::ReadActionInfo(Json::Value& _actionRoot, OUT ActionInfo* _pActionInfo)
{
	_pActionInfo->ActionName = JsonUtil::GetString(_actionRoot["name"]);
	_pActionInfo->code_ = _actionRoot["code"].asInt();
	_pActionInfo->ForceCancelable = _actionRoot["force_cancelable"].asBool();
	_pActionInfo->Moveable = _actionRoot["moveable"].asBool();
	_pActionInfo->SpeedX = _actionRoot.get("x_speed", 0.0f).asFloat();
	_pActionInfo->SpeedY = _actionRoot.get("y_speed", 0.0f).asFloat();

	if (_actionRoot.isMember("command"))
	{
		Value& commandRoot = _actionRoot["command"];
		jc::String command = JsonUtil::GetString(commandRoot);
		ComboKeyList keyList;

		for (int k = 0; k < command.Length(); ++k)
		{
			switch (command[k])
			{
			case 'r': keyList[k] = ControlKey::Right;
				break;
			case 'l': keyList[k] = ControlKey::Left;
				break;
			case 'u': keyList[k] = ControlKey::Up;
				break;
			case 'd': keyList[k] = ControlKey::Down;
				break;
			case 'x': keyList[k] = ControlKey::Attack;
				break;
			case 'c': keyList[k] = ControlKey::Jump;
				break;
			default: jc_assert_msg(false, "올바르지 않은 조작키 입니다.");
			}
		}

		_pActionInfo->Command = keyList;
	}
}
