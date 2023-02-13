/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"


struct SGAnimationInfo;
struct SGCharAnimationInfoLoader
{
public:
	static bool LoadCharAnimationInfo(
		SGHashMap<int, SGAnimationInfo>(&charAnimationInfoMap)[CharType::Max],
		SGVector<SGAnimationInfo*> (&charAnimationList)[CharType::Max]
	);
};