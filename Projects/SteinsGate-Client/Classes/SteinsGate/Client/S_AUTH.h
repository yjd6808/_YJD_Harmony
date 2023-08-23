/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:00:14 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/SendHelper.h>

struct S_AUTH : JNetwork::SendHelper<S_AUTH>
{
	static void SEND_CAU_Login(const char* id, const char* pw);
};


