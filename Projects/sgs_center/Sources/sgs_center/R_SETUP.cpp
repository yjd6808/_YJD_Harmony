/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "R_SETUP.h"

#include "sgs/CmdHost.h"
#include "sgs_center/S_SETUP_IS.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_ItsMe(Session* _pSession, ICommand* _pCmd)
{
	S_SETUP_IS::SetInformation(_pSession, SendStrategy::SendAsync, LastFromId);
	SCE_ItsMe* pCmd = (SCE_ItsMe*)_pCmd;

	if (pCmd->ProcessType < ServerProcessType::Begin || pCmd->ProcessType > ServerProcessType::End) 
	{
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		_pSession->Disconnect();
		return;
	}

	S_SETUP_IS::SEND_CES_YouNeedToDoThis(CenterOrder::LaunchServer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_NotifyBootState(Session* _pSession, ICommand* _pCmd)
{
	SCE_NotifyBootState* pCmd = (SCE_NotifyBootState*)_pCmd;
	_LogInfo_("%s서버가 %s됨.", ServerType::Name[pCmd->ServerType], ServerBootState::Name[pCmd->State]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_NotifyOrderFailed(jnet::Session* _pSession, jnet::ICommand* _pCmd)
{
	SCE_NotifyOrderFailed* pCmd = (SCE_NotifyOrderFailed*)_pCmd;

	_LogInfo_("%s서버가 %s에 실패했습니다. (EC: %u)",
		ServerType::Name[pCmd->ServerType],
		CenterOrder::Name[pCmd->Order],
		pCmd->ErrorCode);
}
