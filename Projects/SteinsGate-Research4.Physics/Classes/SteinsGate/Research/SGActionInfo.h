/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGAnimationInfo.h>
#include <SteinsGate/Research/ComboKeyList.h>

struct SGActionInfo
{
	SGActionInfo(int animationSize) : Animations(animationSize) {}

	int Code;
	SGString ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;
	int CommandCount;
	ComboKeyList Commands[MaxCommandCount_v];
	SGVector<SGAnimationInfo> Animations;
};