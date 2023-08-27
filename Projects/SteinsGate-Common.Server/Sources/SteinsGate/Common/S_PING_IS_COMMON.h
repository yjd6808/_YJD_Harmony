/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/InterServerSendHelper.h>

struct S_PING_IS_COMMON : InterServerSendHelper<S_PING_IS_COMMON>
{
	// HOST
	static bool SEND_SCE_TimeSync();
	
	// RELAY
};
