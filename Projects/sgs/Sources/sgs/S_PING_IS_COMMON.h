/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#pragma once

#include <sgs/InterServerSendHelper.h>

struct S_PING_IS_COMMON : InterServerSendHelper<S_PING_IS_COMMON>
{
	// HOST
	static bool SEND_SCE_TimeSync();

	// RELAY
};
