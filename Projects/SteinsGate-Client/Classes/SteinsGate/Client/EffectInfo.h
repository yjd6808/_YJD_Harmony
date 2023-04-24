/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/AnimationInfo.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct EffectInfo : ConfigDataAbstract
{
	EffectInfo() {}
	EffectInfo(int animationFrameSize) :
		Animation(animationFrameSize) {}
	~EffectInfo() override = default;

	SGString Name;
	int SgaIndex;
	int ImgIndex;
	AnimationInfo Animation;
};