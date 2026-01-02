/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 9:54:20 AM
 * =====================
 *
 */


#pragma once

JC_SENUM_BEGIN(CenterOrder)
	LaunchServer,
	StopServer,
	Max
JC_SENUM_MIDDLE(CenterOrder)

static constexpr const char* Name[Max]{
	"서버 시작",
	"서버 중지"
};
JC_SENUM_MIDDLE_END(CenterOrder)

JC_SENUM_BEGIN(ServerBootState)
	Stopping,
	Stopped,
	Launching,
	Launched,
	Error,
	Max
JC_SENUM_MIDDLE(ServerBootState)

static constexpr const char* Name[Max]{
	"중지중",
	"중지",
	"시작중",
	"시작",
	"오류발생",
};
JC_SENUM_MIDDLE_END(ServerBootState)
