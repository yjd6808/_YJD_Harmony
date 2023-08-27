/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:55:04 PM
 * =====================
 *
 */




#include "Server.h"
#include "ServerCoreHeader.h"
#include "R_TEST_COMMON.h"

#include <SteinsGate/Common/S_SETUP_IS_COMMON.h>
#include <SteinsGate/Common/CmdRelay_TEST.h>

USING_NS_JC;
USING_NS_JNET;

void R_TEST_COMMON::RECV_SS_P2PRelayStaticTest(Session* session, ICommand* cmd) {
	SS_P2PRelayStaticTest* pCmd = (SS_P2PRelayStaticTest*)cmd;
	Console::WriteLine("%s", pCmd->Msg.Source);
}

void R_TEST_COMMON::RECV_SS_P2PRelayDynamicTest(Session* session, ICommand* cmd) {
	SS_P2PRelayDynamicTest* pCmd = (SS_P2PRelayDynamicTest*)cmd;
	Console::WriteLine("%s", pCmd->Msg.Source);
}
