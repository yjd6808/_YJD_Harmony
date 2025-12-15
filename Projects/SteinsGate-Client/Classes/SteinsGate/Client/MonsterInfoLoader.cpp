/*
 * 
 * !F$ 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfoLoader::MonsterInfoLoader(DataManagerAbstract* _pManager)
: MonsterBaseInfoLoader(_pManager)
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

		for (int i = 0; i < monsterListRoot.size(); ++i)
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
		_LogError_("%s OeYj( uJm)uA$.)%s", GetConfigFileName(), exception.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MonsterInfoLoader::ReadMonsterInfo(Json::Value& _mobRoot, MonsterInfo* _pMobInfo)
{
	JsonUtilEx::ParseActorSpriteData(_mobRoot["actor_sprite_data"], &_pMobInfo->pSpriteData_);
}
