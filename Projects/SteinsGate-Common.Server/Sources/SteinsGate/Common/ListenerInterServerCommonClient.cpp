/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 4:02:21 AM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerInterServerCommonClient.h"

#include <SteinsGate/Common/InterServerCmdDefine.h>
#include <SteinsGate/Common/R_INTERSERVER_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

void ListenerInterServerCommonClient::OnReceivedInterServerCmd(Session* sender, ICommand* cmd) {
	if (!IsInterServerCommand(cmd)) {
		_LogWarn_("인터서버 커맨드가 아닌 커맨드를 수신했습니다. (%d)", cmd->Cmd);
		return;
	}

	
	if (IsRelayCommand(cmd)) {
		// 릴레이 커맨드인 경우
		RelayCommandBase* pInterServerCmd = static_cast<RelayStaticCommand*>(cmd);
		R_INTERSERVER_COMMON::LastFromId = pInterServerCmd->From;

		// 클라(나)가 수신처인 경우 그대로 처리
		if (pInterServerCmd->To == CoreServerProcessInfo_v->ServerId)
			ListenerCommonClient::OnReceived(sender, cmd);
		else
			DebugAssertMsg(false, "띠용? 인터서버 클라이언트인데 마지막 도착지점이 아닌데용?");
	} else if (IsHostCommand(cmd)) {
			// 호스트 커맨드인 경우
			ListenerCommonClient::OnReceived(sender, cmd);
	} else {
		DebugAssertMsg(false, "인터서버 커맨드가 아닌 이상한 커맨드를 수신했습니다. (%d)", cmd->Cmd);
	}

}
