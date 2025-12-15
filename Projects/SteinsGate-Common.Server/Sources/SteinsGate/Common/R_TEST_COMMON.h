/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:54:54 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_TEST_COMMON : InterServerRecvHelper
{
public:
	// RELAY
	static void RECV_SS_P2PRelayStaticTest(JNetwork::Session* _pSession, JNetwork::ICommand* _pCmd);
	static void RECV_SS_P2PRelayDynamicTest(JNetwork::Session* _pSession, JNetwork::ICommand* _pCmd);
};
