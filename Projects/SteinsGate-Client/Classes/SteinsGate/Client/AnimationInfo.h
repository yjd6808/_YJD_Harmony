/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:00:28 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct FrameInfo
{
	int FrameIndex;
	float Delay;
	int FrameEvent;
	int FrameEventId;
};

struct FrameInfoAttackBoxInstant : FrameInfo
{
	SGActorRect Rect;
};

struct AnimationInfo : ConfigDataAbstract
{
	AnimationInfo() {}
	AnimationInfo(int frameSize) : Frames(frameSize) {}

	AnimationInfo(const AnimationInfo&);
	AnimationInfo(AnimationInfo&& other) noexcept;

	AnimationInfo& operator=(const AnimationInfo&);
	AnimationInfo& operator=(AnimationInfo&& other) noexcept;
	~AnimationInfo() override;

	bool Loop;
	SGString Name;

	SGVector<FrameInfo*> Frames;
};


