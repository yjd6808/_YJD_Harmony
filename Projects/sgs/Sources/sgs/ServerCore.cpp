/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCore.h"

#include <sgs/CmdHost.h>
#include <sgs/CmdRelay.h>

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Util/_DescMgr/DescMgr_Database.h>
#include <sg/_Util/_DescMgr/DescMgr_ServerInfo.h>

#include <sgs/_Net/NetCore.h>
#include <sgs/UnauthenticatedSessionManager.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeServerCore()
{
	g_cDescMgr.AddLoader(dbg_new ServerInfoLoader());
	g_cDescMgr.AddLoader(dbg_new DatabaseInfoLoader());
	g_cDescMgr.LoadAll();
	g_cNetCore.Initialize();

	g_cUnauthenticatedSessionManager; // 생성


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
	g_cUnauthenticatedSessionManager.Free();

	g_cNetCore.Finalize();
	g_cNetCore.Free();

	g_cDescMgr.Clear();
	g_cDescMgr.Free();
}
