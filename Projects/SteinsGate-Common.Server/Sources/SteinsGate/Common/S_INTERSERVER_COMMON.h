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


struct S_INTERSERVER_COMMON : InterServerSendHelper<S_INTERSERVER_COMMON>
{
public:

	// HOST
	static bool SendItsMe(InterServerClientType_t clientType, int serverId);
	static bool SendCenterMessage(const JCore::String& msg);
	static bool SendNotifyBootState(ServerBootState_t state);
	static bool SendTimeSync();
	

	// RELAY
	static bool SendP2PRelayStaticTest(const char* msg);
	static bool SendP2PRelayDynamicTest(const char* msg);
};
