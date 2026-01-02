/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:17:20 PM
 * =====================
 *
 */


#pragma once


#include <jnet/Packet/SendHelper.h>
#include <sg/Struct.h>

struct S_MESSAGE_COMMON : jnet::SendHelper<S_MESSAGE_COMMON>
{
	static void SEND_SC_ClientText(const char* _pClientTextKey);
};
