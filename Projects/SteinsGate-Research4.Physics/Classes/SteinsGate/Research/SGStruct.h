/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Config.h>

struct SGThicknessBox
{
	float Width;
	float Height;
	float RelativeY;

	cocos2d::Size getSize() const { return { Width, Height }; }
};