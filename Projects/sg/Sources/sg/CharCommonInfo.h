/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 12:15:28 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>

struct CharCommonInfo : SDescBase
{
	CharCommonInfo(int _maxLevel)
	: defaultInvenSlotCount_{}
	, maxLevel_(_maxLevel)
	, expTables_(_maxLevel)
	{
	}

	int defaultInvenSlotCount_[InvenItemType::Max];
	int maxLevel_;
	jc::Vector<int> expTables_;
};
