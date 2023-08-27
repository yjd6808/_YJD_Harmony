/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_PING : InterServerRecvHelper
{
public:
	static void RECV_SCE_TimeSync(JNetwork::Session* session, JNetwork::ICommand* cmd);
};