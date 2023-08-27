/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/InterServerSendHelper.h>

struct S_AUTHENTICATION_IS : InterServerSendHelper<S_AUTHENTICATION_IS>
{
	static void SEND_SAU_AuthenticationCheck(int sessionHandle, AuthenticationSerial_t serial);
};




