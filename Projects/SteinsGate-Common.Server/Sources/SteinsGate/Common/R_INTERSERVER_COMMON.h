/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:42:25 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_RELAY.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_INTERSERVER_COMMON : InterServerRecvHelper
{
public:
	// HOST
	static void RECV_CES_WhoAreYou(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CES_AlreadyConnected(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CES_YouNeedToDoThis(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CES_TimeSyncAck(JNetwork::Session* session, JNetwork::ICommand* cmd);

	// RELAY
	static void RECV_SS_P2PRelayStaticTest(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SS_P2PRelayDynamicTest(JNetwork::Session* session, JNetwork::ICommand* cmd);
};
