/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:21:23 PM
 * =====================
 *
 */


#pragma once


#include "jnet/Host/Session.h"

struct R_MESSAGE
{
	static void RECV_SC_ClientText(jnet::Session* _pSession, jnet::ICommand* _pCmd);
};
