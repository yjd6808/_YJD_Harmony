/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "Center.h"
#include "CenterCoreHeader.h"
#include "R_CENTER.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Server/S_CENTER.h>

USING_NS_JC;
USING_NS_JNET;

void R_CENTER::RECV_SCE_ItsMe(Session* session, ICommand* cmd) {
	S_CENTER::SetInformation(session, eSendAsync, LastFromId);
	SCE_ItsMe* pCmd = (SCE_ItsMe*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	if (pCmd->ClientServerType < ServerProcessType::Begin || pCmd->ClientServerType > ServerProcessType::End) {
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		pSession->Disconnect();
		return;
	}

	if (CoreServer_v->IsConnected(pSession)) {
		// 이미 처리된 세션
		return;
	}

	if (CoreServer_v->GetCenterSession(pCmd->ServerId) != nullptr) {
		_LogWarn_("%s서버는 이미 접속중입니다.", ServerProcessType::Name[pCmd->ClientServerType]);
		S_CENTER::SEND_CES_AlreadyConnected();
		return;
	}

	pSession->SetClientInformation(pCmd->ClientServerType, pCmd->ServerId);
	CoreServer_v->AddSession(pSession);
	S_CENTER::SEND_CES_YouNeedToDoThis(CenterOrder::LaunchServer);
}


void R_CENTER::RECV_SS_HostMessage(Session* session, ICommand* cmd) {
	SS_HostMessage* pCmd = (SS_HostMessage*)cmd;
	_LogInfo_(pCmd->Msg.Source);
}

void R_CENTER::RECV_SCE_NotifyBootState(Session* session, ICommand* cmd) {
	SCE_NotifyBootState* pCmd = (SCE_NotifyBootState*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	ServerProcessInfo* pInfo = Core::ServerProcessInfoPackage->getServerProcessInfo(pCmd->ServerId);
	const String& szName = pInfo == nullptr ? StringUtil::Format("알 수 없음(%d)", pCmd->ServerId) : pInfo->Name;
	_LogInfo_("%s 프로세스의 %s서버가 %s됨.", szName.Source(), ServerType::Name[pCmd->ServerType], ServerBootState::Name[pCmd->State]);
}

void R_CENTER::RECV_SCE_NotifyOrderFailed(JNetwork::Session* session, JNetwork::ICommand* cmd) {
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

void R_CENTER::RECV_SCE_TimeSync(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	SCE_TimeSync* pCmd = (SCE_TimeSync*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	S_CENTER::AutoFlush _;
	S_CENTER::SetInformation(session, eSendAsync);
	S_CENTER::SEND_CES_TimeSyncAck(*pCmd);
}




