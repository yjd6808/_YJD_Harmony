/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:54:54 PM
 * =====================
 *
 */


#pragma once


#include <jnet/Host/Session.h>
#include <sgs/InterServerRecvHelper.h>

struct R_TEST_COMMON : InterServerRecvHelper
{
public:
	// RELAY
	static void RECV_SS_P2PRelayStaticTest(jnet::Session* _pSession, jnet::ICommand* _pCmd);
	static void RECV_SS_P2PRelayDynamicTest(jnet::Session* _pSession, jnet::ICommand* _pCmd);
};
