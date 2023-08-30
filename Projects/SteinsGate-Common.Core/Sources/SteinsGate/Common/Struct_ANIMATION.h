/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:53:36 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Common/SgaSpriteRect.h>


struct FrameInfo
{
	int FrameIndex;
	float Delay;
	int FrameEventCode;
};


struct AnimationInfo : ConfigDataAbstract
{
	AnimationInfo() { DebugAssertMsg(false, "호출 금지"); }
	AnimationInfo(int frameSize)
		: Loop(false)
		, Name{ 0 }
		, Frames(frameSize)
	{}

	bool Loop;
	SGString Name;
	SGVector<FrameInfo> Frames;
};
