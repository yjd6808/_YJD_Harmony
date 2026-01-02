/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Core.h"
#include "MonsterBaseInfoLoader.h"

#include <sg/DescLoaderMgr.h>
#include <sg/_Util/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MonsterBaseInfoLoader::MonsterBaseInfoLoader(DescLoaderMgr* _pManager)
: DescLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MonsterBaseInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& monsterListRoot = root["monsters"];

		for (int i = 0; i < monsterListRoot.size(); ++i)
		{
			Value& monsterRoot = monsterListRoot[i];
			MonsterBaseInfo* pMobInfo = dbg_new MonsterBaseInfo;
			ReadMonsterBaseInfo(monsterRoot, pMobInfo);
			AddData(pMobInfo);
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
void MonsterBaseInfoLoader::ReadMonsterBaseInfo(Json::Value& _monsterRoot, OUT MonsterBaseInfo* _pMobInfo)
{
	_pMobInfo->code_ = _monsterRoot["code"].asInt();
	_pMobInfo->name_ = JsonUtil::GetString(_monsterRoot["kor_name"]);
	JsonUtil::ParseThicknessInfo(_monsterRoot["thickness_box"], _pMobInfo->thicknessBox_);
}
