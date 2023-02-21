/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:38:03 AM
 * =====================
 *
 */


#include "Core.h"
#include "ItemOptInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/SGJson.h>

USING_NS_JS;
USING_NS_JC;

bool ItemOptInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value itemOptListRoot = root["monsters"];

		for (int i = 0; i < itemOptListRoot.size(); ++i) {
			Value& itemOptRoot = itemOptListRoot[i];
			int iLevelOptCount = itemOptRoot["lv"].asInt();
			DebugAssertMsg(iLevelOptCount > 0, "아이템 옵트 카운트가 0입니다.");
			ItemOptInfo* pItemOpt = dbg_new ItemOptInfo(iLevelOptCount);
			readItemOptInfo(itemOptRoot, pItemOpt);
			addData(pItemOpt);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void ItemOptInfoLoader::readItemOptInfo(Json::Value& optRoot, ItemOptInfo* optInfo) {
	optInfo->Code = optRoot["code"].asInt();
	optInfo->Format = SGJson::getString(optRoot["fmt"]);
	optInfo->EName = SGJson::getString(optRoot["ename"]);
	optInfo->IsInteger = optRoot["is_integer"].asBool();

	Value& optLevelListRoot = optRoot["lv"];

	for (int i = 0; i < optLevelListRoot.size(); ++i) {
		int iLevel;
		int iMin;
		int iMax;
		SGJson::parseIntNumber3(optLevelListRoot[i], iLevel, iMin, iMax);
		optInfo->OptLevelList.PushBack({iLevel, iMin, iMax });
	}
}

