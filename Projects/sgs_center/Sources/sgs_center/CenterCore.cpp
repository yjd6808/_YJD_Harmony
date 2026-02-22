/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCore.h"

#include <sg/_Util/DescLoaderMgr.h>

#include "R_MESSAGE.h"
#include "R_PING.h"
#include "R_SETUP.h"
#include "sgs/CmdHost_MESSAGE.h"
#include "sgs/CmdHost_PING.h"
#include "sgs/CmdHost_SETUP.h"
#include "sgs/_Net/NetGroup_InterServ.h"
#include "sgs/_Net/NetGroup_Main.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeCenterCore() 
{
	// SETUP
	g_cNetGroup_Main.Parser().AddCommand<SCE_ItsMe>(R_SETUP::RECV_SCE_ItsMe);
	g_cNetGroup_Main.Parser().AddCommand<SCE_NotifyBootState>(R_SETUP::RECV_SCE_NotifyBootState);

	// MESSAGE
	g_cNetGroup_Main.Parser().AddCommand<SS_HostMessage>(R_MESSAGE::RECV_SS_HostMessage);

	// PING
	g_cNetGroup_Main.Parser().AddCommand<SCE_TimeSync>(R_PING::RECV_SCE_TimeSync);

}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeCenterCore() 
{
}