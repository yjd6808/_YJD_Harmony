/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/InterServerSendHelper.h>

struct S_AUTHENTICATION_IS : InterServerSendHelper<S_AUTHENTICATION_IS>
{
	static void SEND_AUS_AuthenticationCheckAck(int sessionHandle, bool success);
};

