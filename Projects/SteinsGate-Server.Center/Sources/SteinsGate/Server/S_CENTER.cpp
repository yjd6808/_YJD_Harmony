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

bool S_CENTER::SendWhoAreYou() {
	auto sending = SendBegin<CmdWhoAreYou>();
	return true;
}

bool S_CENTER::SendAlreadyConnected() {
	auto sending = SendBegin<CmdAlreadyConnected>();
	return true;
}

bool S_CENTER::SendYouNeedToDoThis(CenterOrder_t order) {
	auto sending = SendBegin<CmdYouNeedToDoThis>();
	sending.Cmd.Order = order;
	return true;
}

bool S_CENTER::BroadcastYouNeedToDoThis(TcpServer* server, CenterOrder_t order) {
	auto pPacket = dbg_new SinglePacket<CmdYouNeedToDoThis>();
	pPacket->Cmd.Order = order;
	server->BroadcastAsync(pPacket);
	return true;
}
