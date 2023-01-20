/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:28 PM
 * =====================
 *
 */


#pragma once



#pragma once

#include <SteinsGate/Research/SGActionInfo.h>
#include <JCore/Container/HashMap.h>

struct SGActionInfoLoader
{
public:
	static void LoadActionInfo(SGHashMap<int, SGActionInfo>& actionInfoMap);
};