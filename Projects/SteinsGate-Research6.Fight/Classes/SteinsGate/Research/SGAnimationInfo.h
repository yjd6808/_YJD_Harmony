/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:00:28 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/SGStruct.h>

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
	SGAnimationInfo& operator=(SGAnimationInfo&& other) noexcept;
	~SGAnimationInfo();

	int Code;
	bool Loop;
	SGString Name;

	SGVector<SGFrameInfo*> Frames;
};


