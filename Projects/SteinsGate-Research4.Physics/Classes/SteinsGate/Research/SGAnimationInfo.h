/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:00:28 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Engine.h>

struct SGFrameInfo
{
	int FrameIndex;
	float Delay;
	int Event;
	int EventId;
};

struct SGAnimationInfo
{
	int PackIndex;
	int ImgIndex;

	SGString Name;
	SGVector<SGFrameInfo> Frames;
};