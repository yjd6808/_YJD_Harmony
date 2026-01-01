/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>

struct MonsterBaseInfo : SDescBase
{
	~MonsterBaseInfo() override = default;

	jc::String name_;
	ThicknessBox thicknessBox_;
};
