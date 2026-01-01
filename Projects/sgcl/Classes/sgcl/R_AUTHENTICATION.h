/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 1:57:31 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>

struct R_AUTHENTICATION
{
public:
	static void RECV_AUC_LoginAck(jnet::Session* _pSession, jnet::ICommand* _pCommand);
};
