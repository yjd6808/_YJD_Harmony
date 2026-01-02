/*
 * 윤정도
 * 작성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <sgcl/MonsterInfo.h>
#include <sg/MonsterBaseInfoLoader.h>


struct MonsterInfoLoader : MonsterBaseInfoLoader
{
public:
	MonsterInfoLoader(DescLoaderMgr* _pManager);
	~MonsterInfoLoader() override = default;

	bool Load() override;

	static void ReadMonsterInfo(Json::Value& _mobRoot, OUT MonsterInfo* _pMobInfo);
};
