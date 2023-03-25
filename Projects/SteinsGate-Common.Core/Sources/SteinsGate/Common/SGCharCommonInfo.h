/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:28 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct SGCharCommonInfo : ConfigDataAbstract
{
	SGCharCommonInfo(int maxLevel)
		: DefaultInvenSlotCount{}
		, MaxLevel(maxLevel)
		, Exp(maxLevel)
	{}
	int DefaultInvenSlotCount[InvenItemType::Max];
	int MaxLevel;
	SGVector<int> Exp;
};
