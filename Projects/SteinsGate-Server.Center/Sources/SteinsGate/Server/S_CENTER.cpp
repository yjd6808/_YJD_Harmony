/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:02:16 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "S_CENTER.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

USING_NS_JC;
USING_NS_JNET;

bool S_CENTER::SEND_CES_WhoAreYou() {
	auto sending = SendBegin<CES_WhoAreYou>();
	return true;
}

bool S_CENTER::SEND_CES_AlreadyConnected() {
	auto sending = SendBegin<CES_AlreadyConnected>();
	return true;
}

bool S_CENTER::SEND_CES_YouNeedToDoThis(CenterOrder_t order) {
	auto sending = SendBegin<CES_YouNeedToDoThis>();
	sending.Cmd.Order = order;
	return true;
}

bool S_CENTER::SEND_CES_TimeSyncAck(SCE_TimeSync& time) {
	auto sending = SendBegin<CES_TimeSyncAck>();
	sending.Cmd.PeerServerTime = time.PeerServerTime;
	sending.Cmd.MasterServerTime = DateTime::Now().Tick - TimeSpan::FromHour(3).Tick;
	return true;
}
//
//bool S_CENTER::BroadcastYouNeedToDoThis(TcpServer* server, CenterOrder_t order) {
//	auto pPacket = dbg_new SinglePacket<CES_YouNeedToDoThis>();
//	pPacket->Cmd.Order = order;
//	server->BroadcastAsync(pPacket);
//	return true;
//}
