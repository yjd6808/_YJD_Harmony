/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "ListenerCenterServer.h"

#include <SteinsGate/Common/CenterCmd.h>

#include <SteinsGate/Server/R_CENTER.h>
#include <SteinsGate/Server/S_CENTER.h>

USING_NS_JC;
USING_NS_JNET;

ListenerCenterServer::ListenerCenterServer() {
	Parser.AddCommand(CmdItsMe_CS,				R_CENTER::RecvItsMe);
	Parser.AddCommand(CmdCenterMessage_CS,		R_CENTER::RecvCenterMessage);
	Parser.AddCommand(CmdNotifyBootState_CS,	R_CENTER::RecvNotifyBootState);
}

void ListenerCenterServer::OnStarted() {
	_LogInfo_("서버가 시작되었습니다.");
}

void ListenerCenterServer::OnConnected(Session* connectedSession) {
}

void ListenerCenterServer::OnDisconnected(Session* disconnetedSession) {
	CenterSession* pSession = (CenterSession*)disconnetedSession;
	CoreServer_v->SetCenterSession(pSession->GetClientType(), nullptr);
}

void ListenerCenterServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
}

void ListenerCenterServer::OnReceived(Session* receiver, ICommand* cmd) {
	if (!Parser.RunCommand(receiver, cmd)) {
		_LogWarn_("커맨드: %d 수행 실패 (Center)", cmd->GetCommand());
	}
}

void ListenerCenterServer::OnStopped() {
	_LogInfo_("서버가 중지되었습니다.");
}
