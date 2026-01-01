/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:53:08 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "R_PING_COMMON.h"

#include <sgs/CmdHost_PING.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_PING_COMMON::RECV_CES_TimeSyncAck(jnet::Session* _pSession, jnet::ICommand* _pCommand)
{
	const CES_TimeSyncAck* pTimeSyncAck = (CES_TimeSyncAck*)_pCommand;
	sg::TimeManager->UpdateMasterServerTime(pTimeSyncAck->MasterServerTime);
}
