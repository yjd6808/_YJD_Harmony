/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:44 PM
 * =====================
 *
 */


#pragma once


#include <jnet/Packet/SendHelper.h>
#include <sg/Struct.h>

struct S_AUTHENTICATION : jnet::SendHelper<S_AUTHENTICATION>
{
	static void SEND_AUC_LoginAck(LoginResult_t _result, GameServerType_t _lastServer = GameServerType::Max, AuthenticationSerial_t _serial = InvalidValue_v);
};




