/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:55:04 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "R_TEST_COMMON.h"

#include <sgs/S_SETUP_IS_COMMON.h>
#include <sgs/CmdRelay_TEST.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_TEST_COMMON::RECV_SS_P2PRelayStaticTest(Session* _pSession, ICommand* _pCmd)
{
	SS_P2PRelayStaticTest* pCmd = (SS_P2PRelayStaticTest*)_pCmd;
	Console::WriteLine("%s", pCmd->Msg.Source);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_TEST_COMMON::RECV_SS_P2PRelayDynamicTest(Session* _pSession, ICommand* _pCmd)
{
	SS_P2PRelayDynamicTest* pCmd = (SS_P2PRelayDynamicTest*)_pCmd;
	Console::WriteLine("%s", pCmd->Msg()->Source);
}
