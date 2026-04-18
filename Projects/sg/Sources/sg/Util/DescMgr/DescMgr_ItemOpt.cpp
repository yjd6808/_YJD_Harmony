/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 6:38:03 AM
 * =====================
 *
 */


#include "DescMgr_ItemOpt.h"

#include "sg/Util/JsonUtil.h"

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfoLoader::ItemOptInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ItemOptInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& itemOptListRoot = root["item_opt"];

		int optCount = static_cast<int>(itemOptListRoot.size());
		dataMapByName_.ExpandIfNeeded(optCount);

		for (ArrayIndex i = 0; i < itemOptListRoot.size(); ++i)
		{
			Value& itemOptRoot = itemOptListRoot[i];
			int levelOptCount = itemOptRoot["lv"].size();
			jc_assert_msg(levelOptCount > 0, "아이템 옵트 카운트가 0입니다.");
			ItemOptInfo* pItemOpt = dbg_new ItemOptInfo(levelOptCount);
			ReadItemOptInfo(itemOptRoot, pItemOpt);
			AddData(pItemOpt);

			if (!dataMapByName_.Insert(pItemOpt->eName_, pItemOpt))
			{
				jc_assert_msg(false, "아이템 옵트 목록에 이미 %s 데이터가 존재하는데 넣을려고 했습니다.", pItemOpt->eName_.Source());
			}
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
void ItemOptInfoLoader::ReadItemOptInfo(Json::Value& _optRoot, ItemOptInfo* _pOptInfo)
{
	_pOptInfo->code_ = _optRoot["code"].asInt();
	_pOptInfo->format_ = JsonUtil::GetString(_optRoot["fmt"]);
	_pOptInfo->eName_ = JsonUtil::GetString(_optRoot["ename"]);
	_pOptInfo->isInteger_ = _optRoot["is_integer"].asBool();

	Value& optLevelListRoot = _optRoot["lv"];

	for (ArrayIndex i = 0; i < optLevelListRoot.size(); ++i)
	{
		int level;
		int minValue;
		int maxValue;
		JsonUtil::ParseIntNumber3(optLevelListRoot[i], level, minValue, maxValue);
		_pOptInfo->optLevelList_.PushBack({ level, minValue, maxValue });
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ItemOptInfo* ItemOptInfoLoader::GetData(const jc::String& _name)
{
	jc_assert_msg(dataMapByName_.Exist(_name), "아이템 옵트 목록에 %s라는 옵트의 정보는 존재하지 않습니다.", _name.Source());
	return dataMapByName_[_name];
}
