/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:00:25 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/InterServerSendHelper.h>

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_Relay.h>

struct S_CENTER : InterServerSendHelper<S_CENTER>
{
public:
	static bool SEND_CES_WhoAreYou();
	static bool SEND_CES_AlreadyConnected();
	static bool SEND_CES_YouNeedToDoThis(CenterOrder_t order);
	static bool SEND_CES_TimeSyncAck(SCE_TimeSync& time);

	// static bool BroadcastYouNeedToDoThis(JNetwork::TcpServer* server, CenterOrder_t order);
};


