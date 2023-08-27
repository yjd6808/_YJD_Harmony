/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_TEST_IS_COMMON.h"

#include <SteinsGate/Common/CmdRelay_TEST.h>


USING_NS_JC;
USING_NS_JNET;

// ===============================================================================
// 
// ===============================================================================
bool S_TEST_IS_COMMON::SEND_SS_P2PRelayStaticTest(const char* msg) {
	auto sending = SendBegin<SS_P2PRelayStaticTest>();
	sending.Cmd.Msg = msg;
	return true;
}

bool S_TEST_IS_COMMON::SEND_SS_P2PRelayDynamicTest(const char* msg) {
	auto sending = SendBegin<SS_P2PRelayDynamicTest>(64);
	sending.Cmd.Msg.SetStringUnsafe(msg);
	return true;
}
