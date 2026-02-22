/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "Core.h"
#include "R_SETUP.h"

#include <sgs/CmdHost.h>
#include <sgs_center/S_SETUP_IS.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_ItsMe(Session* _pSession, ICommand* _pCmd)
{
	S_SETUP_IS::SetInformation(_pSession, SendStrategy::SendAsync, LastFromId);
	SCE_ItsMe* pCmd = (SCE_ItsMe*)_pCmd;

	if (pCmd->ProcessType < ServerProcessType::Begin || pCmd->ProcessType > ServerProcessType::End) {
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		_pSession->Disconnect();
		return;
	}

	if (sg::Server->IsConnected(_pSession)) {
		// 이미 처리된 세션
		return;
	}

	if (sg::Server->GetCenterSession(pCmd->ServerId) != nullptr) {
		_LogWarn_("%s서버는 이미 접속중입니다.", ServerProcessType::Name[pCmd->ProcessType]);
		S_SETUP_IS::SEND_CES_AlreadyConnected();
		return;
	}

	_pSession->SetClientInformation(pCmd->ProcessType, pCmd->ServerId);
	sg::Server->AddSession(_pSession);
	S_SETUP_IS::SEND_CES_YouNeedToDoThis(CenterOrder::LaunchServer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_NotifyBootState(Session* _pSession, ICommand* _pCmd)
{
	SCE_NotifyBootState* pCmd = (SCE_NotifyBootState*)_pCmd;

	ServerProcessInfo* pInfo = sg::ServerProcessInfoPackage->GetServerProcessInfo(pCmd->ServerId);
	const String& serverName = pInfo == nullptr ? StringUtil::Format("알 수 없음(%d)", pCmd->ServerId) : pInfo->name_;
	_LogInfo_("%s 프로세스의 %s서버가 %s됨.", serverName.Source(), ServerType::Name[pCmd->ServerType], ServerBootState::Name[pCmd->State]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_SETUP::RECV_SCE_NotifyOrderFailed(jnet::Session* _pSession, jnet::ICommand* _pCmd)
{
	SCE_NotifyOrderFailed* pCmd = (SCE_NotifyOrderFailed*)_pCmd;

	ServerProcessInfo* pInfo = sg::ServerProcessInfoPackage->GetServerProcessInfo(pCmd->ServerId);
	const String& serverName = pInfo == nullptr ? StringUtil::Format("알 수 없음(%d)", pCmd->ServerId) : pInfo->name_;
	_LogInfo_("%s 프로세스의 %s서버가 %s에 실패했습니다. (EC: %u)",
		serverName.Source(),
		ServerType::Name[pCmd->ServerType],
		CenterOrder::Name[pCmd->Order],
		pCmd->ErrorCode);
}
