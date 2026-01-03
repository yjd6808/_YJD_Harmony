/*
 * 
 * !F$ 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_Monster.h"

#include <sgcl/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfoLoader::MonsterInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MonsterInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value monsterListRoot = root["monsters"];

		for (ArrayIndex i = 0; i < monsterListRoot.size(); ++i)
		{
			Value& monterRoot = monsterListRoot[i];
			MonsterInfo* pMonsterInfo = dbg_new MonsterInfo();
			ReadMonsterBaseInfo(monterRoot, pMonsterInfo);
			ReadMonsterInfo(monterRoot, pMonsterInfo);
			AddData(pMonsterInfo);
		}
	}
	catch (std::exception& exception)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), exception.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MonsterInfoLoader::ReadMonsterInfo(Json::Value& _mobRoot, MonsterInfo* _pMobInfo)
{
	JsonUtilEx::ParseActorSpriteData(_mobRoot["actor_sprite_data"], &_pMobInfo->pSpriteData_);
}
