/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:17:20 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/Struct.h>

struct S_MESSAGE_COMMON : JNetwork::SendHelper<S_MESSAGE_COMMON>
{
	static void SEND_SC_ClientText(const char* clientTextKey);
};
