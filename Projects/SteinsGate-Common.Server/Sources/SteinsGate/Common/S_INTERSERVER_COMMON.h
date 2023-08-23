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
	static bool SEND_SCE_ItsMe(ServerProcessType_t clientType, int serverId);
	static bool SEND_SS_HostMessage(const JCore::String& msg);
	static bool SEND_SCE_NotifyBootState(int serverId, ServerType_t serverType, ServerBootState_t state);
	static bool SEND_SCE_NotifyOrderFailed(int serverId, ServerType_t serverType, CenterOrder_t failedOrder, Int32U errorCode);
	static bool SEND_SCE_TimeSync();
	

	// RELAY
	static bool SEND_SS_P2PRelayStaticTest(const char* msg);
	static bool SEND_SS_P2PRelayDynamicTest(const char* msg);
};
