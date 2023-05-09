/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerAuthServer.h"

#include <SteinsGate/Common/AuthCmd.h>

#include <SteinsGate/Server/R_AUTH.h>

USING_NS_JC;
USING_NS_JNET;

ListenerAuthServer::ListenerAuthServer() {
	Parser.AddCommand(CmdLogin_CS,			R_AUTH::RecvLogin);
}

void ListenerAuthServer::OnStarted() {
	_LogInfo_("서버가 시작되었습니다.");
}

void ListenerAuthServer::OnConnected(Session* connectedSession) {
}

void ListenerAuthServer::OnDisconnected(Session* disconnetedSession) {
}

void ListenerAuthServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
}

void ListenerAuthServer::OnReceived(Session* receiver, ICommand* cmd) {
	if (!Parser.RunCommand(receiver, cmd)) {
		_LogWarn_("커맨드: %d 수행 실패 (Auth)", cmd->GetCommand());
	}
}

void ListenerAuthServer::OnStopped() {
	_LogInfo_("서버가 중지되었습니다.");
}


