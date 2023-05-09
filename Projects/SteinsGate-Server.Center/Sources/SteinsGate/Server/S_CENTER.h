/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:00:25 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/ServerEnum.h>

struct S_CENTER : JNetwork::SendHelper<S_CENTER>
{
public:
	static bool SendWhoAreYou();
	static bool SendAlreadyConnected();
	static bool SendYouNeedToDoThis(CenterOrder_t order);


	static bool BroadcastYouNeedToDoThis(JNetwork::TcpServer* server, CenterOrder_t order);
};


