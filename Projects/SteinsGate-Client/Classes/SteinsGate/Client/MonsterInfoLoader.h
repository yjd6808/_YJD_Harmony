/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MonsterInfo.h>
#include <SteinsGate/Common/MonsterBaseInfoLoader.h>


struct MonsterInfoLoader : MonsterBaseInfoLoader
{
public:
	MonsterInfoLoader(DataManagerAbstract* manager);
	~MonsterInfoLoader() override = default;

	bool load() override;

	static void readMonsterInfo(Json::Value& mobRoot, JCORE_OUT MonsterInfo* mobInfo);
};
