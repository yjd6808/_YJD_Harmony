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

void R_CENTER::RecvItsMe(Session* session, ICommand* cmd) {
	S_CENTER::SetInformation(session, eSendAsync, LastFromId);
	SCE_ItsMe* pCmd = (SCE_ItsMe*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	if (pCmd->ClientType < InterServerClientType::Begin || pCmd->ClientType > InterServerClientType::End) {
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		pSession->Disconnect();
		return;
	}

	if (CoreServer_v->IsConnected(pSession)) {
		// 이미 처리된 세션
		return;
	}

	if (CoreServer_v->GetCenterSession(pCmd->ClientType) != nullptr) {
		_LogWarn_("%s서버는 이미 접속중입니다.", InterServerClientType::Name[pCmd->ClientType]);
		S_CENTER::SendAlreadyConnected();
		return;
	}

	pSession->SetClientInformation(pCmd->ClientType, pCmd->ServerId);
	CoreServer_v->AddSession(pSession);
	S_CENTER::SendYouNeedToDoThis(CenterOrder::LaunchServer);

	/*if (CoreServer_v->IsAllCenterSessionConnected()) {
		S_CENTER::BroadcastYouNeedToDoThis(CoreServer_v, CenterOrder::LaunchServer);
	}*/
	
}


void R_CENTER::RecvCenterMessage(Session* session, ICommand* cmd) {
	SS_HostMessage* pCmd = (SS_HostMessage*)cmd;
	_LogInfo_(pCmd->Msg.Source);
}

void R_CENTER::RecvNotifyBootState(Session* session, ICommand* cmd) {
	SCE_NotifyBootState* pCmd = (SCE_NotifyBootState*)cmd;
	CenterSession* pSession = (CenterSession*)session;
	pSession->SetBootState(pCmd->State);
}

void R_CENTER::RecvTimeSync(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	SCE_TimeSync* pCmd = (SCE_TimeSync*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	S_CENTER::AutoFlush _;
	S_CENTER::SetInformation(session, eSendAsync);
	S_CENTER::SendTimeSyncAck(*pCmd);
}




