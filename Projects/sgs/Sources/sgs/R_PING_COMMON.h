/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:52:10 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>
#include <sgs/InterServerRecvHelper.h>

struct R_PING_COMMON : InterServerRecvHelper
{
public:
	// HOST
	static void RECV_CES_TimeSyncAck(jnet::Session* _pSession, jnet::ICommand* _pCommand);

	// RELAY
};
