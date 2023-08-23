/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "ListenerCenterServer.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/ListenerHelperCommon.h>

#include <SteinsGate/Server/R_CENTER.h>
#include <SteinsGate/Server/S_CENTER.h>



USING_NS_JC;
USING_NS_JNET;

ListenerCenterServer::ListenerCenterServer(CenterServer* server, SGCommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pCenterTcp(server)
{}

void ListenerCenterServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerCenterServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerCenterServer::OnDisconnected(Session* disconnetedSession) {
	ListenerServerCommon::OnDisconnected(disconnetedSession);
	CenterSession* pSession = (CenterSession*)disconnetedSession;

	if (pSession->IsValid()) {
		Core::Server->RemoveSession(pSession);
	}
}

void ListenerCenterServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}


void ListenerCenterServer::OnReceived(Session* session, IRecvPacket* recvPacket) {

	ms_tlsSenderMap.Clear();

	recvPacket->ForEach([this, session](ICommand* cmd) {
		if (!ListenerHelperCommon::IsInterServerCommand(cmd)) {
			_LogWarn_("인터서버 커맨드가 아닌 커맨드를 수신했습니다. (%d)", cmd->Cmd);
			return;
		}

		if (ListenerHelperCommon::IsRelayCommand(cmd)) {
			// 릴레이 커맨드인 경우
			RelayCommandBase* pInterServerCmd;

			if (cmd->Type == InterServerCmdType::RelayStatic)
				pInterServerCmd = static_cast<RelayStaticCommand*>(cmd);
			else
				pInterServerCmd = static_cast<RelayDynamicCommand*>(cmd);

			R_CENTER::LastFromId = pInterServerCmd->From;

			// 클라(나)가 수신처인 경우 그대로 처리
			if (pInterServerCmd->To == Core::ServerProcessInfo->ServerId)
				ListenerServerCommon::OnReceived(session, cmd);
			else
				RelayCommand(cmd, pInterServerCmd);
		} else if (ListenerHelperCommon::IsHostCommand(cmd)) {
			// 호스트 커맨드인 경우
			ListenerServerCommon::OnReceived(session, cmd);
		} else {
			DebugAssertMsg(false, "유효하지 않은 타입의 커맨드를 수신했습니다. (%d)", cmd->Cmd);
		}

	});

	

	// 보통 1개만 있겟지..?
	ms_tlsSenderMap.ForEach([](Session* session) { session->FlushSendBuffer(); });
	
}

void ListenerCenterServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}

// TODO: 하나의 패킷에 여러개의 커맨드가 담긴 경우가 있을 수 있으므로.
void ListenerCenterServer::RelayCommand(ICommand* cmd, RelayCommandBase* relayCmd) {
	Session* pSender = Core::Server->GetCenterSession(relayCmd->To);
	pSender->SendAlloc(cmd);
	ms_tlsSenderMap.Insert(pSender);
}
