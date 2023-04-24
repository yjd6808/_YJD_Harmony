/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:22:29 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "R_CENTER_COMMON.h"

#include <SteinsGate/Common/CenterCmd.h>
#include <SteinsGate/Common/S_CENTER_COMMON.h>

#include "JCore/RAII/AutoPtr.h"

USING_NS_JC;
USING_NS_JNET;

void R_CENTER_COMMON::RecvWhoAreYou(Session* session, ICommand* cmd) {
	S_CENTER_COMMON::SetInformation(session, eSendAsync);
	S_CENTER_COMMON::SendItsMe(CoreCommonNetMaster_v->GetClientType());
}

void R_CENTER_COMMON::RecvAlreadyConnected(Session* session, ICommand* cmd) {
	_LogWarn_("이미 중앙서버에 접속중입니다. 프로세스를 종료합니다.");
	CoreCommonNetMaster_v->Terminate();
}

void R_CENTER_COMMON::RecvYouNeedToDoThis(Session* session, ICommand* cmd) {
	S_CENTER_COMMON::SetInformation(session, eSendAlloc);
	const CmdYouNeedToDoThis* pCmd = (CmdYouNeedToDoThis*)cmd;

	if (CoreCommonNetMaster_v->GetProcessType() == ServerProcessType::Center) {
		_LogWarn_("잘못된 요청입니다.");
		return;
	}

	if (CoreCommonServer_v->ProcessOrder(pCmd->Order)) {
		S_CENTER_COMMON::SendCenterMessage("성공적으로 요청을 수행하였습니다.");
		S_CENTER_COMMON::SendNotifyBootState(CoreCommonServer_v->GetBootState());
		return;
	}

	S_CENTER_COMMON::SendCenterMessage("요청을 수행할 수 없는 상태입니다.");
}
