/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGEnums.h>

struct SGCharBaseInfo;
struct SGCharBaseInfoLoader
{
public:
	static bool LoadCharBaseInfo(SGCharBaseInfo* (&characterInfoMap)[CharType::Max]);
};