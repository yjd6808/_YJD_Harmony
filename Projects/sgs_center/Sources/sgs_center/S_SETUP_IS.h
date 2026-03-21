/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:00:25 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Host/Session.h"
#include "sgs/ServerEnum.h"
#include "sgs/InterServerSendHelper.h"

#include "sgs/CmdHost.h"

struct S_SETUP_IS : InterServerSendHelper<S_SETUP_IS>
{
	static bool SEND_CES_WhoAreYou();
	static bool SEND_CES_AlreadyConnected();
	static bool SEND_CES_YouNeedToDoThis(CenterOrder_t _order);
	static bool SEND_CES_TimeSyncAck(SCE_TimeSync& _time);
};

