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
	SGSize FrameSize;
	SGSize GameResolutionSize;
	SGSize UIResolutionSize = { 640, 480 };
	ResolutionPolicy GameResolutionPolicy;
	float UIScaleXFactor;
	float UIScaleYFactor;
	bool Resizable;
	bool FullScreen;

	SGRect getFrameRect() { return { 0, 0, FrameSize.width, FrameSize.height }; }
	SGRect getGameResolutionRect() { return { 0, 0, GameResolutionSize.width, GameResolutionSize.height }; }
	SGRect getUIResolutionRect() { return { 0, 0, UIResolutionSize.width, UIResolutionSize.height }; }
};