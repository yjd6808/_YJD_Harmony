/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 9:54:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>


JCORE_SENUM_BEGIN(CenterOrder)
LaunchServer,
StopServer,
Max
JCORE_SENUM_MIDDLE(CenterOrder)
static constexpr const char* Name[Max] {
	"서버 시작",
	"서버 중지"
};
JCORE_SENUM_MIDDLE_END(CenterOrder)

JCORE_SENUM_BEGIN(ServerBootState)
Stopping,
Stopped,
Launching,
Launched,
Error,
Max
JCORE_SENUM_MIDDLE(ServerBootState)

static constexpr const char* Name[Max]{
	"중지중",
	"중지",
	"시작중",
	"시작",
	"오류발생",
};

JCORE_SENUM_MIDDLE_END(ServerBootState)

