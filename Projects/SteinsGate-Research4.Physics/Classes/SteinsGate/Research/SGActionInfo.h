/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGAnimationInfo.h>
#include <SteinsGate/Research/Config.h>

struct SGActionInfo
{
	int Code;
	SGString ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;

	SGVector<SGString> Commands;
	SGVector<SGAnimationInfo> Animations;
};