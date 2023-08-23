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
TimeManager* Core::Contents.TimeManager;							// 메인 서버 프로그램에서 주입해줄 것
RuntimeConfigCommon* CoreRuntimeConfigCommon_v;			// 메인 서버 프로그램에서 주입해줄 것

void InitializeServerCore() {
	// 공통 커맨드 이름 등록
	// [ INTER_SERVER - HOST ]
	Core::CommandNameDictionary.Add<CES_WhoAreYou>();
	Core::CommandNameDictionary.Add<SCE_ItsMe>();
	Core::CommandNameDictionary.Add<CES_AlreadyConnected>();
	Core::CommandNameDictionary.Add<CES_YouNeedToDoThis>();
	Core::CommandNameDictionary.Add<SS_HostMessage>();
	Core::CommandNameDictionary.Add<SCE_NotifyBootState>();
	Core::CommandNameDictionary.Add<SCE_NotifyOrderFailed>();
	Core::CommandNameDictionary.Add<SCE_TimeSync>();
	Core::CommandNameDictionary.Add<CES_TimeSyncAck>();
	
	// [ INTER_SERVER - RELAY ]
	Core::CommandNameDictionary.Add<SS_P2PRelayStaticTest>();
	Core::CommandNameDictionary.Add<SS_P2PRelayDynamicTest>();
}

void FinalizeServerCore() {
}