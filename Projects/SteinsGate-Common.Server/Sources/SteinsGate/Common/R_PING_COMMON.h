/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:52:10 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_PING_COMMON : InterServerRecvHelper
{
public:
	// HOST
	static void RECV_CES_TimeSyncAck(JNetwork::Session* session, JNetwork::ICommand* cmd);

	// RELAY
};

