/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/Packet/SendHelper.h>

#include <SteinsGate/Common/ServerEnum.h>


struct S_CENTER_COMMON : JNetwork::SendHelper
{
public:
	static bool SendItsMe(CenterClientType_t clientType);
	static bool SendCenterMessage(const JCore::String& msg);
	static bool SendNotifyBootState(ServerBootState_t state);
};
