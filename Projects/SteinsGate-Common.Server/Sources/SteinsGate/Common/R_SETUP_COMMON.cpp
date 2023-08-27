/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:22:29 PM
 * =====================
 * 인증, 로비, 게임 서버의 공통 인터서버 커맨드 처리
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "R_SETUP_COMMON.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/S_SETUP_IS_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

void R_SETUP_COMMON::RECV_CES_WhoAreYou(Session* session, ICommand* cmd) {
	S_SETUP_IS_COMMON::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync, LastFromId);
	S_SETUP_IS_COMMON::SEND_SCE_ItsMe(Core::CommonNetMaster->GetProcessType(), Core::ServerProcessInfo->ServerId);
}

void R_SETUP_COMMON::RECV_CES_AlreadyConnected(Session* session, ICommand* cmd) {
	_LogWarn_("이미 중앙서버에 접속중입니다. 프로세스를 종료합니다.");
	Core::CommonNetMaster->Terminate();
}

void R_SETUP_COMMON::RECV_CES_YouNeedToDoThis(Session* session, ICommand* cmd) {
	S_SETUP_IS_COMMON::AutoFlush _;
	S_SETUP_IS_COMMON::SetInformation(session, SendStrategy::SendAlloc);
	const CES_YouNeedToDoThis* pCmd = (CES_YouNeedToDoThis*)cmd;

	if (Core::CommonNetMaster->GetProcessType() == ServerProcessType::Center) {
		_LogWarn_("잘못된 요청입니다.");
		return;
	}

	Core::CommonNetGroup->ProcessOrder(pCmd->Order);
}
