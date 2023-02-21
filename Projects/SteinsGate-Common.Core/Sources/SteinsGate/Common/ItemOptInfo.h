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
	int Level;
	int Min;		// 포함
	int Max;		// 포함
};

struct ItemOptInfo : ConfigDataAbstract
{
	ItemOptInfo(int optLevelCount) : OptLevelList(optLevelCount) {}
	~ItemOptInfo() override = default;

	SGString EName;
	SGString Format;
	bool IsInteger;
	SGVector<ItemOptLevel> OptLevelList;
};

