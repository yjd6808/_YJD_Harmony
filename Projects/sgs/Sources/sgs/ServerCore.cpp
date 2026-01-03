/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCore.h"
#include "ServerCoreHeader.h"

#include <sgs/CmdHost.h>
#include <sgs/CmdRelay.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::CommonNetGroupMgr* CommonNetGroupMgr; // 메인 서버 프로그램에서 주입해줄 것
::CommonNetGroup* CommonNetGroup; // 메인 서버 프로그램에서 주입해줄 것
::CommonServer* CommonServer; // 메인 서버 프로그램에서 주입해줄 것
::ServerContents* CommonContents; // 메인 서버 프로그램에서 주입해줄 것
::InterServerClientNetGroup* InterServerClientNetGroup; // 메인 서버 프로그램에서 주입해줄 것
::TcpClient* InterServerClientTcp; // 메인 서버 프로그램에서 주입해줄 것
::UdpClient* InterServerClientUdp; // 메인 서버 프로그램에서 주입해줄 것
::ServerProcessInfo* ServerProcessInfo; // 메인 서버 프로그램에서 주입해줄 것
::ServerProcessInfoPackage* ServerProcessInfoPackage; // 메인 서버 프로그램에서 주입해줄 것
::TimeManager* TimeManager; // 메인 서버 프로그램에서 주입해줄 것
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeServerCore()
{
	// 공통 커맨드 이름 등록
	// [ INTER_SERVER - HOST ]
	// ============================================================
	// MESSAGE
	// ============================================================
	sg::CommandNameDictionary.Add<SS_HostMessage>();

	// ============================================================
	// SETUP
	// ============================================================
	sg::CommandNameDictionary.Add<CES_WhoAreYou>();
	sg::CommandNameDictionary.Add<SCE_ItsMe>();
	sg::CommandNameDictionary.Add<CES_AlreadyConnected>();
	sg::CommandNameDictionary.Add<CES_YouNeedToDoThis>();
	sg::CommandNameDictionary.Add<SCE_NotifyBootState>();
	sg::CommandNameDictionary.Add<SCE_NotifyOrderFailed>();

	// ============================================================
	// PING
	// ============================================================
	sg::CommandNameDictionary.Add<SCE_TimeSync>();
	sg::CommandNameDictionary.Add<CES_TimeSyncAck>();

	// [ INTER_SERVER - RELAY ]
	sg::CommandNameDictionary.Add<SS_P2PRelayStaticTest>();
	sg::CommandNameDictionary.Add<SS_P2PRelayDynamicTest>();
	// ============================================================
	// AUTHENTICATION
	// ============================================================
	sg::CommandNameDictionary.Add<SAU_AuthenticationCheck>();
	sg::CommandNameDictionary.Add<AUS_AuthenticationCheckAck>();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeServerCore()
{
}
