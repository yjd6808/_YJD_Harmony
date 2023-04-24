/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 *
 */



#include "Center.h"
#include "CenterCoreHeader.h"
#include "R_CENTER.h"

#include <SteinsGate/Common/CenterCmd.h>
#include <SteinsGate/Server/S_CENTER.h>

USING_NS_JC;
USING_NS_JNET;

void R_CENTER::RecvItsMe(Session* session, ICommand* cmd) {
	S_CENTER::SetInformation(session, eSendAsync);
	CmdItsMe* pItsMe = (CmdItsMe*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	if (pItsMe->ClientType < CenterClientType::Begin || pItsMe->ClientType > CenterClientType::End) {
		_LogWarn_("누군지 알 수 없는 세션이 접속을 시도하였습니다.");
		pSession->Disconnect();
		return;
	}

	if (CoreServer_v->IsConnected(pSession)) {
		// 이미 처리된 세션
		return;
	}

	if (CoreServer_v->GetCenterSession(pItsMe->ClientType) != nullptr) {
		_LogWarn_("%s서버는 이미 접속중입니다.", CenterClientType::Name[pItsMe->ClientType]);
		S_CENTER::SendAlreadyConnected();
		return;
	}

	pSession->SetClientType(pItsMe->ClientType);
	CoreServer_v->SetCenterSession(pItsMe->ClientType, pSession);
	S_CENTER::SendYouNeedToDoThis(CenterOrder::LaunchServer);

	/*if (CoreServer_v->IsAllCenterSessionConnected()) {
		S_CENTER::BroadcastYouNeedToDoThis(CoreServer_v, CenterOrder::LaunchServer);
	}*/
	
}


void R_CENTER::RecvCenterMessage(Session* session, ICommand* cmd) {
	CmdCenterMessage* pCmd = (CmdCenterMessage*)cmd;
	_LogInfo_(pCmd->Msg.Source);
}

void R_CENTER::RecvNotifyBootState(Session* session, ICommand* cmd) {
	CmdNotifyBootState* pCmd = (CmdNotifyBootState*)cmd;
	CenterSession* pSession = (CenterSession*)session;
	pSession->SetBootState(pCmd->State);
}




