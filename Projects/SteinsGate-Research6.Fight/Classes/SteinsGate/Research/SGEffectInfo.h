/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGAnimationInfo.h>

struct SGEffectInfo
{
	SGEffectInfo() {}
	SGEffectInfo(int animationFrameSize) :
		Animation(animationFrameSize) {}

	int Code;
	SGString Name;
	int NpkIndex;
	int ImgIndex;
	SGAnimationInfo Animation;
};