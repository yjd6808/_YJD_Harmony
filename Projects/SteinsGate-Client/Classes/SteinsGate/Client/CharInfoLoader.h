/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CharBaseInfoLoader.h>
#include <SteinsGate/Client/CharInfo.h>

struct CharInfoLoader : CharBaseInfoLoader
{
public:
	CharInfoLoader(DataManagerAbstract* manager);

	bool load() override;
	static void readDefaultVisualInfo(Json::Value& charRoot, JCORE_OUT CharInfo* charInfo);
};
