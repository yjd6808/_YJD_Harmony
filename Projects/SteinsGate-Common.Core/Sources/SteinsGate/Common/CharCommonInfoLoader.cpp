/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:55 AM
 * =====================
 *
 */



#include "Core.h"
#include "CharCommonInfoLoader.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>


USING_NS_JS;
USING_NS_JC;

CharCommonInfoLoader::CharCommonInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool CharCommonInfoLoader::load() {
	Value root;

	if (!loadJson(root)) {
		return false;
	}

	try {
		Value& charCommonInfoListRoot = root["common"];

		for (int i = 0; i < charCommonInfoListRoot.size(); ++i) {
			Value& charCommonInfoRoot = charCommonInfoListRoot[i];
			CharCommonInfo* pCharCommonInfo = dbg_new CharCommonInfo(charCommonInfoRoot["exp"].size() + 1);
			readCharCommonInfo(charCommonInfoRoot, pCharCommonInfo);
			addData(pCharCommonInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void CharCommonInfoLoader::readCharCommonInfo(Value& charCommonRoot, CharCommonInfo* charCommonInfo) {
	charCommonInfo->Code = charCommonRoot["code"].asInt();
	charCommonInfo->DefaultInvenSlotCount[InvenItemType::Equip] = charCommonRoot["default_equip_slot_count"].asInt();
	charCommonInfo->DefaultInvenSlotCount[InvenItemType::Consume] = charCommonRoot["default_consume_slot_count"].asInt();
	charCommonInfo->DefaultInvenSlotCount[InvenItemType::Etc] = charCommonRoot["default_etc_slot_count"].asInt();
	charCommonInfo->DefaultInvenSlotCount[InvenItemType::Quest] = charCommonRoot["default_quest_slot_count"].asInt();
	charCommonInfo->DefaultInvenSlotCount[InvenItemType::Avatar] = charCommonRoot["default_avatar_slot_count"].asInt();

	Value& expRoot = charCommonRoot["exp"];
	for (int i = 0; i < expRoot.size(); ++i) {
		charCommonInfo->Exp.PushBack(expRoot[i].asInt());
	}
}

