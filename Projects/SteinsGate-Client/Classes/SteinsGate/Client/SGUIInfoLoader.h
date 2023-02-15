/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:10 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGUIInfo.h>


struct SGUIInfoLoader
{
public:
	static bool LoadUIInfo(
		SGHashMap<int, SGUIElementInfo*>& elementInfoMap
	);

	static void LoadElementCommon(Json::Value& value, SGUIElementInfo* info);
	static SGUIElementInfo* LoadElementButton(Json::Value& value);
	static SGUIElementInfo* LoadElementLabel(Json::Value& value);
	static SGUIElementInfo* LoadElementSprite(Json::Value& value);
};



