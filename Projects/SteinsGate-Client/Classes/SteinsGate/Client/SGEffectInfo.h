/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct SGEffectInfo : ConfigDataAbstract
{
	SGEffectInfo() {}
	SGEffectInfo(int animationFrameSize) :
		Animation(animationFrameSize) {}
	~SGEffectInfo() override = default;

	SGString Name;
	int NpkIndex;
	int ImgIndex;
	SGAnimationInfo Animation;
};