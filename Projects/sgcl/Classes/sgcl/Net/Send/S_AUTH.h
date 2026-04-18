/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 3:00:14 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Packet/SendHelper.h"

struct S_AUTH : jnet::SendHelper<S_AUTH>
{
	static void SEND_CAU_Login();
};
