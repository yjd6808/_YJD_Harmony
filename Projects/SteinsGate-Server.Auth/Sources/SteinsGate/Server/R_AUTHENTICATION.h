/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:41:27 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

struct R_AUTHENTICATION
{
	static void RECV_CAU_Login(JNetwork::Session* session, JNetwork::ICommand* cmd);


	// IS
	static void RECV_SAU_AuthenticationCheck(JNetwork::Session* session, JNetwork::ICommand* cmd);
};