/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct MonsterBaseInfo : ConfigDataAbstract
{
	~MonsterBaseInfo() override = default;

	SGString Name;
	ThicknessBox ThicknessBox;
};
