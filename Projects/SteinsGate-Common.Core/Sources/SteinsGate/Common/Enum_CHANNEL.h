/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:44:51 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(EnteranceType)
Newbie,
Begin = Newbie,
Auction,
Normal,
End = Normal,
Max
JCORE_SENUM_MIDDLE(EnteranceType)
JCORE_SENUM_MIDDLE_END(EnteranceType)

// ==========================================================================================

JCORE_SENUM_BEGIN(ChannelDensity)
Free,
Normal,
Busy,
Full,
Max
JCORE_SENUM_MIDDLE(ChannelDensity)

float Density[Max]{
	25.0f,
	50.0f,
	75.0f,
	100.0f
};

static constexpr const char* Name[Max]{
	"쾌적",
	"보통",
	"혼잡",
	"포화"
};

JCORE_SENUM_MIDDLE_END(ChannelDensity)

// ==========================================================================================