/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCore.h"
#include "ServerCoreHeader.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/CmdRelay.h>

USING_NS_JC;
USING_NS_JNET;

NS_CORE_BEGIN
::CommonNetMaster*				CommonNetMaster;			// 메인 서버 프로그램에서 주입해줄 것
::CommonNetGroup*				CommonNetGroup;				// 메인 서버 프로그램에서 주입해줄 것
::CommonServer*					CommonServer;				// 메인 서버 프로그램에서 주입해줄 것
::RuntimeConfigCommon*			CommonRuntimeConfig;		// 메인 서버 프로그램에서 주입해줄 것
::ServerContents*				CommonContents;				// 메인 서버 프로그램에서 주입해줄 것
::InterServerClientNetGroup*	InterServerClientNetGroup;	// 메인 서버 프로그램에서 주입해줄 것
::TcpClient*					InterServerClientTcp;		// 메인 서버 프로그램에서 주입해줄 것
::UdpClient*					InterServerClientUdp;		// 메인 서버 프로그램에서 주입해줄 것
::ServerProcessInfo*			ServerProcessInfo;			// 메인 서버 프로그램에서 주입해줄 것
::TimeManager*					TimeManager;				// 메인 서버 프로그램에서 주입해줄 것
NS_CORE_END

void InitializeServerCore() {
	// 공통 커맨드 이름 등록
	// [ INTER_SERVER - HOST ]
	// ============================================================
	// MESSAGE
	// ============================================================
	Core::CommandNameDictionary.Add<SS_HostMessage>();


	// ============================================================
	// SETUP
	// ============================================================
	Core::CommandNameDictionary.Add<CES_WhoAreYou>();
	Core::CommandNameDictionary.Add<SCE_ItsMe>();
	Core::CommandNameDictionary.Add<CES_AlreadyConnected>();
	Core::CommandNameDictionary.Add<CES_YouNeedToDoThis>();
	Core::CommandNameDictionary.Add<SCE_NotifyBootState>();
	Core::CommandNameDictionary.Add<SCE_NotifyOrderFailed>();

	// ============================================================
	// PING
	// ============================================================
	Core::CommandNameDictionary.Add<SCE_TimeSync>();
	Core::CommandNameDictionary.Add<CES_TimeSyncAck>();


	// [ INTER_SERVER - RELAY ]
	Core::CommandNameDictionary.Add<SS_P2PRelayStaticTest>();
	Core::CommandNameDictionary.Add<SS_P2PRelayDynamicTest>();
	// ============================================================
	// AUTHENTICATION
	// ============================================================
	Core::CommandNameDictionary.Add<SAU_AuthenticationCheck>();
	Core::CommandNameDictionary.Add<AUS_AuthenticationCheckAck>();
}

void FinalizeServerCore() {
}