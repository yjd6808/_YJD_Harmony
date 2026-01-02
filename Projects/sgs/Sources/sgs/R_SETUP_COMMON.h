/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:42:25 AM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>
#include <sgs/InterServerRecvHelper.h>

struct R_SETUP_COMMON : InterServerRecvHelper
{
	// HOST
	static void RECV_CES_WhoAreYou(jnet::Session* session, jnet::ICommand* cmd);
	static void RECV_CES_AlreadyConnected(jnet::Session* session, jnet::ICommand* cmd);
	static void RECV_CES_YouNeedToDoThis(jnet::Session* session, jnet::ICommand* cmd);

	// RELAY
};
