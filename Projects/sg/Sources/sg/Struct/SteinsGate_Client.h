/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include "sg/Util/DescBase.h"

// frame_width, height: 윈도우 크기,
// resulution_width, height: 게임 해상도,
// resolution_policy: 윈도우에 대해서 게임 해상도가 어떻게 적용될지에 대한 정책 EXACT_FIT(0), NO_BORDER(1), SHOW_ALL(2),
struct ClientInfo : SDescBase
{
	~ClientInfo() override = default;
	float frameWidth_ = 1280;
	float frameHeight_ = 720;
	float gameResolutionWidth_ = 1280;
	float gameResolutionHeight_ = 720;
	int gameResolutionPolicy_ = 0;
	float uiResolutionWidth_ = 640;
	float uiResolutionHeight_ = 480;
	bool resizable_ = false;
	bool fullScreen_ = false;

	// 서버 연결 정보
	jnet::IPv4EndPoint remoteAuth_;
	jnet::IPv4EndPoint remoteLobby_;
};
