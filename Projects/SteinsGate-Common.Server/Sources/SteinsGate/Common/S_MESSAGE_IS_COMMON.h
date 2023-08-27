/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/InterServerSendHelper.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <JNetwork/Packet/SendHelper.h>


struct S_MESSAGE_IS_COMMON : InterServerSendHelper<S_MESSAGE_IS_COMMON>
{
	// HOST
	static bool SEND_SS_HostMessage(const JCore::String& msg);
	
	// RELAY
};
