/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:00:28 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGStruct.h>

struct SGFrameInfo
{
	int FrameIndex;
	float Delay;
	int FrameEvent;
	int FrameEventId;
};

struct SGFrameInfoAttackBoxInstant : SGFrameInfo
{
	SGActorRect Rect;
};

struct SGAnimationInfo
{
	SGAnimationInfo() {}
	SGAnimationInfo(int frameSize) : Frames(frameSize) {}

	SGAnimationInfo(const SGAnimationInfo&);
	SGAnimationInfo(SGAnimationInfo&& other) noexcept;

	SGAnimationInfo& operator=(const SGAnimationInfo&);
	SGAnimationInfo& operator=(SGAnimationInfo&& other) noexcept;
	~SGAnimationInfo();

	int Code;
	bool Loop;
	SGString Name;

	SGVector<SGFrameInfo*> Frames;
};


