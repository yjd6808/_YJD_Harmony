/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/InterServerSendHelper.h>

struct S_TEST_IS_COMMON : InterServerSendHelper<S_TEST_IS_COMMON>
{
	// HOST
	// RELAY
	static bool SEND_SS_P2PRelayStaticTest(const char* msg);
	static bool SEND_SS_P2PRelayDynamicTest(const char* msg);
};
