/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:53:08 PM
 * =====================
 *
 */



#include "Server.h"
#include "ServerCoreHeader.h"
#include "R_PING_COMMON.h"

#include <SteinsGate/Common/CmdHost_PING.h>

USING_NS_JC;
USING_NS_JNET;

void R_PING_COMMON::RECV_CES_TimeSyncAck(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	const CES_TimeSyncAck* pCmd = (CES_TimeSyncAck*)cmd;
	Core::TimeManager->UpdateMasterServerTime(pCmd->MasterServerTime);
}
