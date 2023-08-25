/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:19:09 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(SceneType)
Login,
ChannelSelect,
Game,
None,
Max = None
JCORE_SENUM_MIDDLE(SceneType)
static constexpr const char* Name[Max]{
	"로그인",
	"채널 선택",
	"게임",
};
JCORE_SENUM_MIDDLE_END(SceneType)

