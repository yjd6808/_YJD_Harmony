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
	int frameIndex_;
	float delay_;
	int frameEventCode_;
};


struct AnimationInfo : ConfigDataAbstract
{
	AnimationInfo()
	{
		DebugAssertMsg(false, "호출 금지");
	}

	AnimationInfo(int _frameSize)
	: loop_(false)
	, name_{ 0 }
	, frames_(_frameSize)
	{
	}

	bool loop_;
	SGString name_;
	SGVector<FrameInfo> frames_;
};
