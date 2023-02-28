﻿/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGCharBaseInfoLoader.h>
#include <SteinsGate/Client/SGCharInfo.h>

struct SGCharInfoLoader : SGCharBaseInfoLoader
{
public:
	SGCharInfoLoader(DataManagerAbstract* manager);

	bool load() override;
	static void readDefaultVisualInfo(Json::Value& charRoot, Out_ SGCharInfo* charInfo);
};