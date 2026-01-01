/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:02:16 PM
 * =====================
 *
 */


#include "Core.h"
#include "CenterCoreHeader.h"
#include "S_SETUP_IS.h"

#include <sgs/CmdHost.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS::SEND_CES_WhoAreYou()
{
	auto sending = SendBegin<CES_WhoAreYou>();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS::SEND_CES_AlreadyConnected()
{
	auto sending = SendBegin<CES_AlreadyConnected>();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS::SEND_CES_YouNeedToDoThis(CenterOrder_t _order)
{
	auto sending = SendBegin<CES_YouNeedToDoThis>();
	sending.cmd_.Order = _order;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS::SEND_CES_TimeSyncAck(SCE_TimeSync& _time)
{
	auto sending = SendBegin<CES_TimeSyncAck>();
	sending.cmd_.PeerServerTime = _time.PeerServerTime;
	sending.cmd_.MasterServerTime = DateTime::Now().Tick - TimeSpan::FromHour(3).Tick;
	return true;
}

