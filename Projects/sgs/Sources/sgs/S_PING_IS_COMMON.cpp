/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */


#include "S_PING_IS_COMMON.h"

#include "sgs/CmdHost_PING.h"

USING_NS_JC;
USING_NS_JNET;

bool S_PING_IS_COMMON::SEND_SCE_TimeSync()
{
	auto sending = SendBegin<SCE_TimeSync>();
	sending.cmd_.PeerServerTime.Tick = DateTime::Now().Tick;
	return true;
}
