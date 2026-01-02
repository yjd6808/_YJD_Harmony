/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:23:41 PM
 * =====================
 *
 */


#pragma once


#include <sg/_Util/DescBase.h>
#include "SteinsGate_Item.h"

struct ItemOptLevel
{
	int level_;
	int min_; // 포함
	int max_; // 포함
};

struct ItemOptInfo : SDescBase
{
	ItemOptInfo(int _optLevelCount)
	: eName_(0)
	, format_(0)
	, isInteger_(false)
	, optLevelList_(_optLevelCount)
	{
	}

	~ItemOptInfo() override = default;

	jc::String eName_;
	jc::String format_;
	bool isInteger_;
	jc::Vector<ItemOptLevel> optLevelList_;
};
