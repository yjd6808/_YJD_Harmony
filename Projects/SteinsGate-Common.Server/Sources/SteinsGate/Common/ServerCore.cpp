/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCore.h"
#include "ServerCoreHeader.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_RELAY.h>

USING_NS_JC;
USING_NS_JNET;

CommonNetMaster* CoreCommonNetMaster_v;					// 메인 서버 프로그램에서 주입해줄 것
CommonNetGroup* CoreCommonNetGroup_v;					// 메인 서버 프로그램에서 주입해줄 것
CommonServer* CoreCommonServer_v;						// 메인 서버 프로그램에서 주입해줄 것
ServerProcessInfo* CoreServerProcessInfo_v;				// 메인 서버 프로그램에서 주입해줄 것
InterServerClientNetGroup* CoreInterServerClientNetGroup_v;	// 메인 서버 프로그램에서 주입해줄 것
TcpClient* CoreInterServerClientTcp_v;					// 메인 서버 프로그램에서 주입해줄 것
UdpClient* CoreInterServerClientUdp_v;					// 메인 서버 프로그램에서 주입해줄 것
TimeManager* CoreTimeManager_v;							// 메인 서버 프로그램에서 주입해줄 것
RuntimeConfigCommon* CoreRuntimeConfigCommon_v;			// 메인 서버 프로그램에서 주입해줄 것

void InitializeServerCore() {
	// 공통 커맨드 이름 등록
	// [ INTER_SERVER - HOST ]
	CoreCommandNameDictionary_v.Add<CES_WhoAreYou>();
	CoreCommandNameDictionary_v.Add<SCE_ItsMe>();
	CoreCommandNameDictionary_v.Add<CES_AlreadyConnected>();
	CoreCommandNameDictionary_v.Add<CES_YouNeedToDoThis>();
	CoreCommandNameDictionary_v.Add<SS_HostMessage>();
	CoreCommandNameDictionary_v.Add<SCE_NotifyBootState>();
	CoreCommandNameDictionary_v.Add<SCE_NotifyOrderFailed>();
	CoreCommandNameDictionary_v.Add<SCE_TimeSync>();
	CoreCommandNameDictionary_v.Add<CES_TimeSyncAck>();
	
	// [ INTER_SERVER - RELAY ]
	CoreCommandNameDictionary_v.Add<SS_P2PRelayStaticTest>();
	CoreCommandNameDictionary_v.Add<SS_P2PRelayDynamicTest>();
}

void FinalizeServerCore() {
}