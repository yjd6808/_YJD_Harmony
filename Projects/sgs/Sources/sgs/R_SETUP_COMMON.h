/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:42:25 AM
 * =====================
 *
 */


#pragma once

#include "jnet/Host/Session.h"
#include "sgs/InterServerRecvHelper.h"

struct R_SETUP_COMMON : InterServerRecvHelper
{
	// HOST
	static void RECV_CES_WhoAreYou(jnet::Session* _session, jnet::ICommand* _cmd);
	static void RECV_CES_AlreadyConnected(jnet::Session* _session, jnet::ICommand* _cmd);
	static void RECV_CES_YouNeedToDoThis(jnet::Session* _session, jnet::ICommand* _cmd);

	// RELAY
};
