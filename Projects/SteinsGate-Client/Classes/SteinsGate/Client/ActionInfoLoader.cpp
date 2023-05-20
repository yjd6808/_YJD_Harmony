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


ActionInfoLoader::ActionInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ActionInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value gunnerRoot = root["gunner"];

		for (ArrayIndex i = 0; i < gunnerRoot.size(); ++i) {
			Value& actionRoot = gunnerRoot[i];
			ActionInfo* actionInfo = dbg_new ActionInfo;
			readActionInfo(actionRoot, actionInfo);
			addData(actionInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s\n", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void ActionInfoLoader::readActionInfo(Json::Value& actionRoot, JCORE_OUT ActionInfo* actionInfo) {
	actionInfo->ActionName = JsonUtilEx::getString(actionRoot["name"]);
	actionInfo->Code = actionRoot["code"].asInt();
	actionInfo->ForceCancelable = actionRoot["force_cancelable"].asBool();
	actionInfo->Moveable = actionRoot["moveable"].asBool();
	actionInfo->SpeedX = actionRoot.get("x_speed", 0.0f).asFloat();
	actionInfo->SpeedY = actionRoot.get("y_speed", 0.0f).asFloat();

	if (actionRoot.isMember("command")) {
		Value& commandRoot = actionRoot["command"];
		SGString command = JsonUtilEx::getString(commandRoot);
		ComboKeyList keyList;
		for (int k = 0; k < command.Length(); ++k) {
			switch (command[k]) {
			case 'r': keyList[k] = ControlKey::Right; break;
			case 'l': keyList[k] = ControlKey::Left; break;
			case 'u': keyList[k] = ControlKey::Up; break;
			case 'd': keyList[k] = ControlKey::Down; break;
			case 'x': keyList[k] = ControlKey::Attack; break;
			case 'c': keyList[k] = ControlKey::Jump; break;
			default: DebugAssertMsg(false, "올바르지 않은 조작키 입니다.");
			}
		}
		actionInfo->Command = keyList;
	}
}

