/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 4:14:29 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>

struct R_AUTHENTICATION
{
	static void RECV_AUS_AuthenticationCheckAck(UNUSED jnet::Session*, jnet::ICommand* _pCommand);
};
