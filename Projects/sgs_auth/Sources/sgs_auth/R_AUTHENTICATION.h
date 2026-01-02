/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:41:27 AM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>

struct R_AUTHENTICATION
{
	static void RECV_CAU_Login(jnet::Session* session, jnet::ICommand* cmd);


	// IS
	static void RECV_SAU_AuthenticationCheck(jnet::Session* session, jnet::ICommand* cmd);
};