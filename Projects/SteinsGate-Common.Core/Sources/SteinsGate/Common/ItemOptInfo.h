/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:23:41 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ItemOptLevel
{
	int level_;
	int min_; // 포함
	int max_; // 포함
};

struct ItemOptInfo : ConfigDataAbstract
{
	ItemOptInfo(int _optLevelCount)
	: eName_(0)
	, format_(0)
	, isInteger_(false)
	, optLevelList_(_optLevelCount)
	{
	}

	~ItemOptInfo() override = default;

	SGString eName_;
	SGString format_;
	bool isInteger_;
	SGVector<ItemOptLevel> optLevelList_;
};
