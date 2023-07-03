/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "ListenerInterServerHostTcp.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

#include <SteinsGate/Server/R_CENTER.h>
#include <SteinsGate/Server/S_CENTER.h>

USING_NS_JC;
USING_NS_JNET;

ListenerInterServerHostTcp::ListenerInterServerHostTcp() {
	Parser.AddCommand<CmdItsMe>(R_CENTER::RecvItsMe);
	Parser.AddCommand<CmdHostMessage>(R_CENTER::RecvCenterMessage);
	Parser.AddCommand<CmdNotifyBootState>(R_CENTER::RecvNotifyBootState);
}

void ListenerInterServerHostTcp::OnStarted() {
	ListenerCommonServer::OnStarted();
}

void ListenerInterServerHostTcp::OnConnected(Session* connectedSession) {
	ListenerCommonServer::OnConnected(connectedSession);
}

void ListenerInterServerHostTcp::OnDisconnected(Session* disconnetedSession) {
	ListenerCommonServer::OnDisconnected(disconnetedSession);
	CenterSession* pSession = (CenterSession*)disconnetedSession;

	if (pSession->IsValid()) {
		CoreServer_v->RemoveSession(pSession);
	}
}

void ListenerInterServerHostTcp::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerCommonServer::OnSent(sender, sentPacket, sentBytes);
}


void ListenerInterServerHostTcp::OnReceived(Session* sender, IRecvPacket* recvPacket) {

	ms_tlsSenderMap.Clear();

	recvPacket->ForEach([this, sender](ICommand* cmd) {
		if (!IsInterServerCommand(cmd)) {
			_LogWarn_("인터서버 커맨드가 아닌 커맨드를 수신했습니다. (%d)", cmd->Cmd);
			return;
		}

		if (IsRelayCommand(cmd)) {
			// 릴레이 커맨드인 경우
			RelayCommandBase* pInterServerCmd;

			if (cmd->Type == InterServerCmdType::RelayStatic)
				pInterServerCmd = static_cast<RelayStaticCommand*>(cmd);
			else
				pInterServerCmd = static_cast<RelayDynamicCommand*>(cmd);

			R_CENTER::LastFromId = pInterServerCmd->From;

			// 클라(나)가 수신처인 경우 그대로 처리
			if (pInterServerCmd->To == CoreServerProcessInfo_v->ServerId)
				ListenerCommonServer::OnReceived(sender, cmd);
			else
				RelayCommand(cmd, pInterServerCmd);
		} else if (IsHostCommand(cmd)) {
			// 호스트 커맨드인 경우
			ListenerCommonServer::OnReceived(sender, cmd);
		} else {
			DebugAssertMsg(false, "유효하지 않은 타입의 커맨드를 수신했습니다. (%d)", cmd->Cmd);
		}

	});

	

	// 보통 1개만 있겟지..?
	ms_tlsSenderMap.ForEachValue([](Session* session) { session->FlushSendBuffer(); });
	
}

void ListenerInterServerHostTcp::OnStopped() {
	ListenerCommonServer::OnStopped();
}

// TODO: 하나의 패킷에 여러개의 커맨드가 담긴 경우가 있을 수 있으므로.
void ListenerInterServerHostTcp::RelayCommand(ICommand* cmd, RelayCommandBase* relayCmd) {
	Session* pSender = CoreServer_v->GetCenterSession(relayCmd->To);
	pSender->SendAlloc(cmd);
	ms_tlsSenderMap.Insert(pSender, pSender);
}
