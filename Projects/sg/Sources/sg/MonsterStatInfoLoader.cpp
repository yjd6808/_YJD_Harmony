/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */

#include "Core.h"
#include "MonsterStatInfoLoader.h"

#include <sg/DescLoaderMgr.h>
#include <sg/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MonsterStatInfoLoader::MonsterStatInfoLoader(DescLoaderMgr* _pManager)
: DescLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MonsterStatInfoLoader::Load()
{
	Json::Value configRoot;

	if (!LoadJson(configRoot))
	{
		return false;
	}

	try
	{
		Value& monsterListRoot = configRoot["monsters"];

		for (int index = 0; index < monsterListRoot.size(); ++index)
		{
			Value& monsterRoot = monsterListRoot[index];
			MonsterStatInfo* pMonsterStatInfo = dbg_new MonsterStatInfo;
			ReadMobStatInfo(monsterRoot, pMonsterStatInfo);
			AddData(pMonsterStatInfo);
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
void MonsterStatInfoLoader::ReadMobStatInfo(Json::Value& _monsterRoot, OUT MonsterStatInfo* _pMonsterStatInfo)
{
	_pMonsterStatInfo->code_ = _monsterRoot["code"].asInt();
	_pMonsterStatInfo->hp_ = _monsterRoot["hp"].asInt();
	_pMonsterStatInfo->mp_ = _monsterRoot.get("mp", 0).asInt();
	_pMonsterStatInfo->physicalDamage_ = _monsterRoot["physical_damage"].asInt();
	_pMonsterStatInfo->magicDamage_ = _monsterRoot["magical_damage"].asInt();
	_pMonsterStatInfo->armor_ = _monsterRoot.get("armor", 0).asInt();
	_pMonsterStatInfo->magicArmor_ = _monsterRoot.get("magic_armor", 0).asInt();
	_pMonsterStatInfo->moveSpeedX_ = _monsterRoot.get("move_speed_x", 0).asFloat();
	_pMonsterStatInfo->moveSpeedY_ = _monsterRoot["move_speed_y"].asFloat();
	_pMonsterStatInfo->downRecoverTime_ = _monsterRoot.get("down_recover_time", 1.0).asFloat();
	_pMonsterStatInfo->weight_ = _monsterRoot["weight"].asFloat();
}
