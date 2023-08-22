/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_CENTER : InterServerRecvHelper
{
public:
	static void RECV_SCE_ItsMe(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SS_HostMessage(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SCE_NotifyBootState(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SCE_NotifyOrderFailed(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SCE_TimeSync(JNetwork::Session* session, JNetwork::ICommand* cmd);
};