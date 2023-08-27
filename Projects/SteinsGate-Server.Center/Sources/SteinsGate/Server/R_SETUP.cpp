/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "Center.h"
#include "CenterCoreHeader.h"
#include "R_SETUP.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Server/S_SETUP_IS.h>

USING_NS_JC;
USING_NS_JNET;

void R_SETUP::RECV_SCE_ItsMe(Session* session, ICommand* cmd) {
	S_SETUP_IS::SetInformation(session, SendStrategy::SendAsync, LastFromId);
	SCE_ItsMe* pCmd = (SCE_ItsMe*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	if (pCmd->ClientServerType < ServerProcessType::Begin || pCmd->ClientServerType > ServerProcessType::End) {
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		pSession->Disconnect();
		return;
	}

	if (Core::Server->IsConnected(pSession)) {
		// 이미 처리된 세션
		return;
	}

	if (Core::Server->GetCenterSession(pCmd->ServerId) != nullptr) {
		_LogWarn_("%s서버는 이미 접속중입니다.", ServerProcessType::Name[pCmd->ClientServerType]);
		S_SETUP_IS::SEND_CES_AlreadyConnected();
		return;
	}

	pSession->SetClientInformation(pCmd->ClientServerType, pCmd->ServerId);
	Core::Server->AddSession(pSession);
	S_SETUP_IS::SEND_CES_YouNeedToDoThis(CenterOrder::LaunchServer);
}

void R_SETUP::RECV_SCE_NotifyBootState(Session* session, ICommand* cmd) {
	SCE_NotifyBootState* pCmd = (SCE_NotifyBootState*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	ServerProcessInfo* pInfo = Core::ServerProcessInfoPackage->getServerProcessInfo(pCmd->ServerId);
	const String& szName = pInfo == nullptr ? StringUtil::Format("알 수 없음(%d)", pCmd->ServerId) : pInfo->Name;
	_LogInfo_("%s 프로세스의 %s서버가 %s됨.", szName.Source(), ServerType::Name[pCmd->ServerType], ServerBootState::Name[pCmd->State]);
}

void R_SETUP::RECV_SCE_NotifyOrderFailed(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	SCE_NotifyOrderFailed* pCmd = (SCE_NotifyOrderFailed*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	ServerProcessInfo* pInfo = Core::ServerProcessInfoPackage->getServerProcessInfo(pCmd->ServerId);
	const String& szName = pInfo == nullptr ? StringUtil::Format("알 수 없음(%d)", pCmd->ServerId) : pInfo->Name;
	_LogInfo_("%s 프로세스의 %s서버가 %s에 실패했습니다. (EC: %u)", 
		szName.Source(), 
		ServerType::Name[pCmd->ServerType], 
		CenterOrder::Name[pCmd->Order], 
		pCmd->ErrorCode
	);
}

