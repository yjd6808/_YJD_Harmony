/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:00 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"


struct SGFontLoader
{
public:
	static bool LoadFontInfo(
		SGHashMap<SGString, int>& fontNameToCodeMap, 
		SGHashMap<int, SGString>& fontCodeToNameMap
	);
};


