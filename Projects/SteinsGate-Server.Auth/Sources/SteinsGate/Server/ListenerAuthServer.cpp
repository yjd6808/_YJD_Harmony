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

#include <SteinsGate/Common/QueryDefine.h>

USING_NS_JC;
USING_NS_JNET;

ListenerAuthServer::ListenerAuthServer() {
	Parser.AddCommand<CmdLogin>(R_AUTH::RecvLogin);
}

void ListenerAuthServer::OnStarted() {
	ListenerCommonServer::OnStarted();

}

void ListenerAuthServer::OnConnected(Session* connectedSession) {
	ListenerCommonServer::OnConnected(connectedSession);
}

void ListenerAuthServer::OnDisconnected(Session* disconnetedSession) {
	ListenerCommonServer::OnDisconnected(disconnetedSession);
}

void ListenerAuthServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerCommonServer::OnSent(sender, sentPacket, sentBytes);
}

void ListenerAuthServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerCommonServer::OnReceived(receiver, cmd);
}

void ListenerAuthServer::OnStopped() {
	ListenerCommonServer::OnStopped();
}


