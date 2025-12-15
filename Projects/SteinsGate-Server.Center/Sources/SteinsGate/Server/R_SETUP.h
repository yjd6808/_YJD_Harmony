/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/InterServerRecvHelper.h>

struct R_SETUP : InterServerRecvHelper
{
public:
	static void RECV_SCE_ItsMe(JNetwork::Session* _pSession, JNetwork::ICommand* _pCmd);
	static void RECV_SCE_NotifyBootState(JNetwork::Session* _pSession, JNetwork::ICommand* _pCmd);
	static void RECV_SCE_NotifyOrderFailed(JNetwork::Session* _pSession, JNetwork::ICommand* _pCmd);
};
