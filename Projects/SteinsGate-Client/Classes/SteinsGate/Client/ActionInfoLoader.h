/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "ActionInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

#include <json.h>
#include <fstream>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ActionInfoLoader::ActionInfoLoader(DataManagerAbstract* _pManager)
	: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionInfoLoader::load()
{
	Json::Value root;

	if (!loadJson(root))
		return false;

	try
	{
		Json::Value gunnerRoot = root["gunner"];

		for (ArrayIndex index = 0; index < gunnerRoot.size(); ++index)
		{
			Value& actionRoot = gunnerRoot[index];
			ActionInfo* pActionInfo = dbg_new ActionInfo;
			readActionInfo(actionRoot, pActionInfo);
			addData(pActionInfo);
		}
	}
	catch (std::exception& _exception)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s\n", getConfigFileName(), _exception.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionInfoLoader::readActionInfo(Json::Value& _actionRoot, JCORE_OUT ActionInfo* _pActionInfo)
{
	_pActionInfo->ActionName = JsonUtilEx::getString(_actionRoot["name"]);
	_pActionInfo->Code = _actionRoot["code"].asInt();
	_pActionInfo->ForceCancelable = _actionRoot["force_cancelable"].asBool();
	_pActionInfo->Moveable = _actionRoot["moveable"].asBool();
	_pActionInfo->SpeedX = _actionRoot.get("x_speed", 0.0f).asFloat();
	_pActionInfo->SpeedY = _actionRoot.get("y_speed", 0.0f).asFloat();

	if (_actionRoot.isMember("command"))
	{
		Value& commandRoot = _actionRoot["command"];
		SGString command = JsonUtilEx::getString(commandRoot);
		ComboKeyList keyList;
		for (int keyIndex = 0; keyIndex < command.Length(); ++keyIndex)
		{
			switch (command[keyIndex])
			{
			case 'r': keyList[keyIndex] = ControlKey::Right; break;
			case 'l': keyList[keyIndex] = ControlKey::Left; break;
			case 'u': keyList[keyIndex] = ControlKey::Up; break;
			case 'd': keyList[keyIndex] = ControlKey::Down; break;
			case 'x': keyList[keyIndex] = ControlKey::Attack; break;
			case 'c': keyList[keyIndex] = ControlKey::Jump; break;
			default: DebugAssertMsg(false, "올바르지 않은 조작키 입니다.");
			}
		}
		_pActionInfo->Command = keyList;
	}
}
