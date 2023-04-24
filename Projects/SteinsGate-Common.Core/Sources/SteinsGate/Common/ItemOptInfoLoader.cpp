/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:38:03 AM
 * =====================
 *
 */


#include "Core.h"
#include "ItemOptInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

ItemOptInfoLoader::ItemOptInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool ItemOptInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Json::Value& itemOptListRoot = root["item_opt"];

		int iOptCount = int(itemOptListRoot.size());
		m_DataMapByName.ExpandIfNeeded(iOptCount);

		for (int i = 0; i < itemOptListRoot.size(); ++i) {
			Value& itemOptRoot = itemOptListRoot[i];
			int iLevelOptCount = itemOptRoot["lv"].size();
			DebugAssertMsg(iLevelOptCount > 0, "아이템 옵트 카운트가 0입니다.");
			ItemOptInfo* pItemOpt = dbg_new ItemOptInfo(iLevelOptCount);
			readItemOptInfo(itemOptRoot, pItemOpt);
			addData(pItemOpt);

			if (!m_DataMapByName.Insert(pItemOpt->EName, pItemOpt)) {
				DebugAssertMsg(false, "아이템 옵트 목록에 이미 %s 데이터가 존재하는데 넣을려고 했습니다.", pItemOpt->EName.Source());
			}
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
	optInfo->Format = JsonUtil::getString(optRoot["fmt"]);
	optInfo->EName = JsonUtil::getString(optRoot["ename"]);
	optInfo->IsInteger = optRoot["is_integer"].asBool();

	Value& optLevelListRoot = optRoot["lv"];

	for (int i = 0; i < optLevelListRoot.size(); ++i) {
		int iLevel;
		int iMin;
		int iMax;
		JsonUtil::parseIntNumber3(optLevelListRoot[i], iLevel, iMin, iMax);
		optInfo->OptLevelList.PushBack({iLevel, iMin, iMax });
	}
}

ItemOptInfo* ItemOptInfoLoader::getData(const SGString& name) {
	DebugAssertMsg(m_DataMapByName.Exist(name), "아이템 옵트 목록에 %s라는 옵트의 정보는 존재하지 않습니다.", name.Source());
	return m_DataMapByName[name];
}

