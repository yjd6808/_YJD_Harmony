/*
 * 윤정도
 * 작성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MonsterInfo.h>
#include <SteinsGate/Common/MonsterBaseInfoLoader.h>


struct MonsterInfoLoader : MonsterBaseInfoLoader
{
public:
	MonsterInfoLoader(DataManagerAbstract* _pManager);
	~MonsterInfoLoader() override = default;

	bool Load() override;

	static void ReadMonsterInfo(Json::Value& _mobRoot, JCORE_OUT MonsterInfo* _pMobInfo);
};
