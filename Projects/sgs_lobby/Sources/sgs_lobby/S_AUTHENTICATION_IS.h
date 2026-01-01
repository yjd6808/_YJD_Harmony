/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <sgs/InterServerSendHelper.h>

struct S_AUTHENTICATION_IS : InterServerSendHelper<S_AUTHENTICATION_IS>
{
	static void SEND_SAU_AuthenticationCheck(int _sessionHandle, const char* _pAccountId, AuthenticationSerial_t _serial);
};




