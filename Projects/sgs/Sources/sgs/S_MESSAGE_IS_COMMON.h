/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */

#pragma once

#include "sgs/InterServerSendHelper.h"
#include "sgs/ServerEnum.h"
#include "jnet/Packet/SendHelper.h"

struct S_MESSAGE_IS_COMMON : InterServerSendHelper<S_MESSAGE_IS_COMMON>
{
	// HOST
	static bool SEND_SS_HostMessage(const jc::String& _msg);

	// RELAY
};
