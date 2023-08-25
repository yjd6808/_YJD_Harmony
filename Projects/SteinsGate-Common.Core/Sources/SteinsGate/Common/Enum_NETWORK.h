/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:09:41 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(GamePlayMode)
Single,
Lan,
Wan,
Max
JCORE_SENUM_MIDDLE(GamePlayMode)

static constexpr bool IsNetworkMode[Max]{
	false,
	true,
};

JCORE_SENUM_MIDDLE_END(GamePlayMode)

// ==========================================================================================
