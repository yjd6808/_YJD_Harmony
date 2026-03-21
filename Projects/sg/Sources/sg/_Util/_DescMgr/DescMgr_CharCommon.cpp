/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:55 AM
 * =====================
 *
 */


#include "DescMgr_CharCommon.h"

#include "sg/_Util/JsonUtil.h"

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CharCommonInfoLoader::CharCommonInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CharCommonInfoLoader::Load()
{
	Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Value& charCommonInfoListRoot = root["common"];

		for (ArrayIndex i = 0; i < charCommonInfoListRoot.size(); ++i)
		{
			Value& charCommonInfoRoot = charCommonInfoListRoot[i];
			CharCommonInfo* pCharCommonInfo = dbg_new CharCommonInfo(charCommonInfoRoot["exp"].size() + 1);
			ReadCharCommonInfo(charCommonInfoRoot, pCharCommonInfo);
			AddData(pCharCommonInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CharCommonInfoLoader::ReadCharCommonInfo(Value& _charCommonRoot, CharCommonInfo* _pCharCommonInfo)
{
	_pCharCommonInfo->code_ = _charCommonRoot["code"].asInt();
	_pCharCommonInfo->defaultInvenSlotCount_[InvenItemType::Equip] = _charCommonRoot["default_equip_slot_count"].asInt();
	_pCharCommonInfo->defaultInvenSlotCount_[InvenItemType::Consume] = _charCommonRoot["default_consume_slot_count"].asInt();
	_pCharCommonInfo->defaultInvenSlotCount_[InvenItemType::Etc] = _charCommonRoot["default_etc_slot_count"].asInt();
	_pCharCommonInfo->defaultInvenSlotCount_[InvenItemType::Quest] = _charCommonRoot["default_quest_slot_count"].asInt();
	_pCharCommonInfo->defaultInvenSlotCount_[InvenItemType::Avatar] = _charCommonRoot["default_avatar_slot_count"].asInt();

	Value& expRoot = _charCommonRoot["exp"];
	for (ArrayIndex i = 0; i < expRoot.size(); ++i)
	{
		_pCharCommonInfo->expTables_.PushBack(expRoot[i].asInt());
	}
}
