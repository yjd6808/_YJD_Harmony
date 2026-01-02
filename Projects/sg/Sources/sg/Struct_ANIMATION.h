/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:53:36 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>
#include <sg/_Sga/SgaSpriteRect.h>

struct FrameInfo
{
	int frameIndex_;
	float delay_;
	int frameEventCode_;
};


struct AnimationInfo : SDescBase
{
	AnimationInfo()
	{
		jc_assert_msg(false, "호출 금지");
	}

	AnimationInfo(int _frameSize)
	: loop_(false)
	, name_{ 0 }
	, frames_(_frameSize)
	{
	}

	bool loop_;
	jc::String name_;
	jc::Vector<FrameInfo> frames_;
};
