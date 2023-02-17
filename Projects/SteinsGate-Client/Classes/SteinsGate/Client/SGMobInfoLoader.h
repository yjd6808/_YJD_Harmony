/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:11 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGMobInfo.h>
#include <SteinsGate/Common/SGMobBaseInfoLoader.h>


struct SGMobInfoLoader : SGMobBaseInfoLoader
{
public:
	~SGMobInfoLoader() override = default;

	bool load() override;

	static void readMobInfo(Json::Value& mobRoot, Out_ SGMobInfo* mobInfo);
};
