/*
 * 윤정도
 * 작성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <sg/_Struct/SteinsGate_Monster.h>
#include <sg/_Util/_DescMgr/DescMgr_MonsterBase.h>

struct MonsterInfoLoader : MonsterBaseInfoLoader
{
public:
	MonsterInfoLoader();
	~MonsterInfoLoader() override = default;

	bool Load() override;

	static void ReadMonsterInfo(Json::Value& _mobRoot, OUT MonsterInfo* _pMobInfo);
};
