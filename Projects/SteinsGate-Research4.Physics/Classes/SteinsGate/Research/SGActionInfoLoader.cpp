/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */



#include "SGActionInfoLoader.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>

using namespace Json;
using namespace JCore;


void SGActionInfoLoader::LoadActionInfo(SGHashMap<int, SGActionInfo>& actionInfoMap) {
	SGString path = JCore::Path::Combine(ConfigDirectory_v, "action.json");
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMessage(reader.is_open(), "action.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;
	} catch (std::exception& ex) {
		Log("action.json 파싱중 오류가 발생하였습니다. %s\n", ex.what());
		return;
	}
	
	Json::Value gunnerRoot = root["gunner"];

	for (ArrayIndex i = 0; i < gunnerRoot.size(); ++i) {
		Value& actionRoot = gunnerRoot[i];
		Value& animationListRoot = actionRoot["animation"];

		DebugAssertMessage(animationListRoot.size() > 0, "애니메이션이 없는 액션입니다.");
		SGActionInfo actionInfo(animationListRoot.size());

		actionInfo.ActionName = SGJson::getString(actionRoot["name"]);
		actionInfo.Code = actionRoot["code"].asInt();
		actionInfo.ForceCancelable = actionRoot["force_cancelable"].asBool();
		actionInfo.Moveable = actionRoot["moveable"].asBool();
		actionInfo.SpeedX = actionRoot.get("x_speed", 0.0f).asFloat();
		actionInfo.SpeedY = actionRoot.get("y_speed", 0.0f).asFloat();

		if (actionRoot.isMember("command")) {
			Value& commandRoot = actionRoot["command"];
			actionInfo.CommandCount = commandRoot.size();
			for (ArrayIndex j = 0; j < commandRoot.size(); ++j) {
				SGString command = SGJson::getString(commandRoot[j]);
				ComboKeyList keyList;
				 for (int k = 0; k < command.Length(); ++k) {
					switch (command[k]) {
					case 'r': keyList[k] = ControlKey::Right; break;
					case 'l': keyList[k] = ControlKey::Left; break;
					case 'u': keyList[k] = ControlKey::Up; break;
					case 'd': keyList[k] = ControlKey::Down; break;
					case 'x': keyList[k] = ControlKey::Attack; break;
					case 'c': keyList[k] = ControlKey::Jump; break;
					default: DebugAssert(false, "올바르지 않은 조작키 입니다.");
					}
				 }
				 actionInfo.Commands[j] = keyList;
			}
		}


		for (ArrayIndex j = 0; j < animationListRoot.size(); ++j) {
			Value& animationRoot = animationListRoot[j];
			SGAnimationInfo animationInfo(animationRoot["frames"].size());
			SGJson::parseAnimationInfo(animationRoot, animationInfo);
			actionInfo.Animations.PushBack(Move(animationInfo));
		}

		actionInfoMap.Insert(actionInfo.Code, Move(actionInfo));
	}

	Log("SGActionInfoLoader :: 로딩완료\n");
}

