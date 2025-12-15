/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

class ClientInfo : public ConfigDataAbstract
{
public:
	~ClientInfo() override = default;
	SGSize frameSize_;
	SGSize gameResolutionSize_;
	SGSize uiResolutionSize_ = { 640, 480 };
	ResolutionPolicy gameResolutionPolicy_;
	float uiScaleXFactor_;
	float uiScaleYFactor_;
	bool resizable_;
	bool fullScreen_;

	SGRect GetFrameRect()
	{
		return { 0, 0, frameSize_.width, frameSize_.height };
	}

	SGRect GetGameResolutionRect()
	{
		return { 0, 0, gameResolutionSize_.width, gameResolutionSize_.height };
	}

	SGRect GetUiResolutionRect()
	{
		return { 0, 0, uiResolutionSize_.width, uiResolutionSize_.height };
	}
};
