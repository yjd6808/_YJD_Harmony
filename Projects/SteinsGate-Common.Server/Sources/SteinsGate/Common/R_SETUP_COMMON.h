/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:42:25 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_SETUP_COMMON : InterServerRecvHelper
{
	// HOST
	static void RECV_CES_WhoAreYou(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CES_AlreadyConnected(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CES_YouNeedToDoThis(JNetwork::Session* session, JNetwork::ICommand* cmd);

	// RELAY
};
