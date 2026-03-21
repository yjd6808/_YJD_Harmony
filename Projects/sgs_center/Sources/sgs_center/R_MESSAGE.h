/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Host/Session.h"
#include "sgs/InterServerRecvHelper.h"

struct R_MESSAGE : InterServerRecvHelper
{
public:
	static void RECV_SS_HostMessage(jnet::Session* _pSession, jnet::ICommand* _pCmd);
};
