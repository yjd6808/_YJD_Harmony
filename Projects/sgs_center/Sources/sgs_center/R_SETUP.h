/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>
#include <sgs/InterServerRecvHelper.h>

struct R_SETUP : InterServerRecvHelper
{
public:
	static void RECV_SCE_ItsMe(jnet::Session* _pSession, jnet::ICommand* _pCmd);
	static void RECV_SCE_NotifyBootState(jnet::Session* _pSession, jnet::ICommand* _pCmd);
	static void RECV_SCE_NotifyOrderFailed(jnet::Session* _pSession, jnet::ICommand* _pCmd);
};
