/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MobInfo.h>
#include <SteinsGate/Common/MobBaseInfoLoader.h>


struct MobInfoLoader : MobBaseInfoLoader
{
public:
	MobInfoLoader(DataManagerAbstract* manager);
	~MobInfoLoader() override = default;

	bool load() override;

	static void readMobInfo(Json::Value& mobRoot, Out_ MobInfo* mobInfo);
};
