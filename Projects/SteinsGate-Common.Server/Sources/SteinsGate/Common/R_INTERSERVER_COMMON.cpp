/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:22:29 PM
 * =====================
 * 인증, 로비, 게임 서버의 공통 인터서버 커맨드 처리
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "R_INTERSERVER_COMMON.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/S_INTERSERVER_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

void R_INTERSERVER_COMMON::RecvWhoAreYou(Session* session, ICommand* cmd) {
	S_INTERSERVER_COMMON::SetInformation(CoreInterServerClientTcp_v, eSendAsync, LastFromId);
	S_INTERSERVER_COMMON::SendItsMe(CoreCommonNetMaster_v->GetClientType(), InterServerSendHelperBase::GetSenderId());
}

void R_INTERSERVER_COMMON::RecvAlreadyConnected(Session* session, ICommand* cmd) {
	_LogWarn_("이미 중앙서버에 접속중입니다. 프로세스를 종료합니다.");
	CoreCommonNetMaster_v->Terminate();
}

void R_INTERSERVER_COMMON::RecvYouNeedToDoThis(Session* session, ICommand* cmd) {
	S_INTERSERVER_COMMON::AutoFlush _;
	S_INTERSERVER_COMMON::SetInformation(session, eSendAlloc);
	const CES_YouNeedToDoThis* pCmd = (CES_YouNeedToDoThis*)cmd;

	if (CoreCommonNetMaster_v->GetProcessType() == ServerProcessType::Center) {
		_LogWarn_("잘못된 요청입니다.");
		return;
	}

	if (CoreCommonServer_v->ProcessOrder(pCmd->Order)) {
		S_INTERSERVER_COMMON::SendCenterMessage("성공적으로 요청을 수행하였습니다.");
		S_INTERSERVER_COMMON::SendNotifyBootState(CoreCommonServer_v->GetBootState());
		return;
	}

	S_INTERSERVER_COMMON::SendCenterMessage("요청을 수행할 수 없는 상태입니다.");
}

void R_INTERSERVER_COMMON::RecvTimeSyncAck(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	const CES_TimeSyncAck* pCmd = (CES_TimeSyncAck*)cmd;
	CoreTimeManager_v->UpdateMasterServerTime(pCmd->MasterServerTime);
}

void R_INTERSERVER_COMMON::RecvP2PRelayStaticTest(Session* session, ICommand* cmd) {
	SS_P2PRelayStaticTest* pCmd = (SS_P2PRelayStaticTest*)cmd;
	Console::WriteLine("%s", pCmd->Msg.Source);
}

void R_INTERSERVER_COMMON::RecvP2PRelayDynamicTest(Session* session, ICommand* cmd) {
	SS_P2PRelayDynamicTest* pCmd = (SS_P2PRelayDynamicTest*)cmd;
	Console::WriteLine("%s", pCmd->Msg.Source);
}
