/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:44 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/Struct.h>

struct S_AUTHENTICATION : JNetwork::SendHelper<S_AUTHENTICATION>
{
	static void SEND_AUC_LoginAck(LoginResult_t result, GameServerType_t lastServer = GameServerType::Max, AuthenticationSerial_t serial = InvalidValue_v);
};




