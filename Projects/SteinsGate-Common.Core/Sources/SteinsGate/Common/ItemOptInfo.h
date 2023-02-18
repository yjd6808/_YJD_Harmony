/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:23:41 PM
 * =====================
 *
 */


#pragma once


#include "Core.h"
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ItemOptInfo : ConfigDataAbstract
{
	SGString Name;
	int Min;
	int Max;
};

