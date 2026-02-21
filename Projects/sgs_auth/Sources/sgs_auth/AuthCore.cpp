/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCore.h"

#include <jdb/MySQL/MysqlDatabase.h>

#include <sgs/CmdRelay_AUTHENTICATION.h>
#include <sgs/_Net/NetCore.h>
#include <sgs/_Net/NetGroup_InterServ.h>
#include <sgs_auth/R_AUTHENTICATION.h>
#include <sgs_auth/AuthenticationManager.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_JDB;

////////////////////////////////////////////////////////////////////////////////////////
void InitializeAuthCore()
{
	g_cScheduler.AddFirstTask(dbg_new AuthenticationManager::Schedule);
	g_cNetGroup_InterServ.Parser().AddCommand<SAU_AuthenticationCheck>(R_AUTHENTICATION::RECV_SAU_AuthenticationCheck);
}

////////////////////////////////////////////////////////////////////////////////////////
void FinalizeAuthCore()
{
}
