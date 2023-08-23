/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:44 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/AuthEnum.h>

struct S_AUTH : JNetwork::SendHelper<S_AUTH>
{
	static void SEND_AUC_LoginAck(LoginResult_t result);
};





