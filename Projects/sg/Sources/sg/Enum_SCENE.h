/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:19:09 PM
 * =====================
 *
 */


#pragma once

#include <jc/Define.h>

JC_SENUM_BEGIN(SceneType)
		Login,
		ChannelSelect,
		Game,
		None,
		Max = None
	JC_SENUM_MIDDLE(SceneType)
	static constexpr const char* Name[Max]{
		"로그인",
		"채널 선택",
		"게임",
	};
JC_SENUM_MIDDLE_END(SceneType)
