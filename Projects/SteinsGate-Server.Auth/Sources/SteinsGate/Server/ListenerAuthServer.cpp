/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerAuthServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerAuthServer::ListenerAuthServer(AuthServer* server, CommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pAuthTcp(server)
{}

void ListenerAuthServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerAuthServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerAuthServer::OnDisconnected(Session* disconnetedSession) {
	ListenerServerCommon::OnDisconnected(disconnetedSession);
}

void ListenerAuthServer::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}

void ListenerAuthServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerServerCommon::OnReceived(receiver, cmd);
}

void ListenerAuthServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}


