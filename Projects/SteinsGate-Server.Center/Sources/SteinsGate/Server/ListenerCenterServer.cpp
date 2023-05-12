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
	Parser.AddCommand<CmdItsMe>(R_CENTER::RecvItsMe);
	Parser.AddCommand<CmdCenterMessage>(R_CENTER::RecvCenterMessage);
	Parser.AddCommand<CmdNotifyBootState>(R_CENTER::RecvNotifyBootState);
}

void ListenerCenterServer::OnStarted() {
	ListenerCommonServer::OnStarted();
}

void ListenerCenterServer::OnConnected(Session* connectedSession) {
	ListenerCommonServer::OnConnected(connectedSession);
}

void ListenerCenterServer::OnDisconnected(Session* disconnetedSession) {
	ListenerCommonServer::OnDisconnected(disconnetedSession);
	CenterSession* pSession = (CenterSession*)disconnetedSession;
	CoreServer_v->SetCenterSession(pSession->GetClientType(), nullptr);
}

void ListenerCenterServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerCommonServer::OnSent(sender, sentPacket, sentBytes);
}

void ListenerCenterServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerCommonServer::OnReceived(receiver, cmd);
}

void ListenerCenterServer::OnStopped() {
	ListenerCommonServer::OnStopped();
}
