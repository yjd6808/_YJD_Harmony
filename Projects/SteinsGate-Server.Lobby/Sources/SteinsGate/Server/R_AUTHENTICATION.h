/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 4:14:29 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

struct R_AUTHENTICATION
{
	static void RECV_AUS_AuthenticationCheckAck(JCORE_UNUSED JNetwork::Session* NOT_USE, JNetwork::ICommand* cmd);
};