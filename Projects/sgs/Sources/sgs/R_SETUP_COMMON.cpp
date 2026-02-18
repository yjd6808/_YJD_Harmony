/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:22:29 PM
 * =====================
 * 인증, 로비, 게임 서버의 공통 인터서버 커맨드 처리
 */


#include "Core.h"
#include "R_SETUP_COMMON.h"

#include <sgs/CmdHost.h>
#include <sgs/_Net/NetCore.h>
#include <sgs/S_SETUP_IS_COMMON.h>
#include <sgs/_Net/NetGroup_Main.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP_COMMON::RECV_CES_WhoAreYou(Session* _session, ICommand* _cmd)
{
	S_SETUP_IS_COMMON::SetInformation(_session, SendStrategy::SendAsync, LastFromId);
	S_SETUP_IS_COMMON::SEND_SCE_ItsMe((ServerProcessType_t)g_cNetCore.GetProcessType());
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP_COMMON::RECV_CES_AlreadyConnected(Session* _session, ICommand* _cmd)
{
	_LogWarn_("이미 중앙서버에 접속중입니다. 프로세스를 종료합니다.");
	g_cNetCore.Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP_COMMON::RECV_CES_YouNeedToDoThis(Session* _session, ICommand* _cmd)
{
	S_SETUP_IS_COMMON::AutoFlush _;
	S_SETUP_IS_COMMON::SetInformation(_session, SendStrategy::SendAlloc);
	const CES_YouNeedToDoThis* pCmd = (CES_YouNeedToDoThis*)_cmd;

	if (g_cNetCore.GetProcessType() == ServerProcessType::Center)
	{
		_LogWarn_("잘못된 요청입니다.");
		return;
	}

	jc::String args = pCmd->Args.Source;
	g_cNetGroup_Main.ProcessOrder(pCmd->Order, args);
}
