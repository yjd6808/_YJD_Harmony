/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ListenerAreaServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerAreaServer::ListenerAreaServer(AreaServer* server, CommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pAreaTcp(server)
{}

void ListenerAreaServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerAreaServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerAreaServer::OnDisconnected(Session* disconnetedSession, Int32U errorCode) {
	ListenerServerCommon::OnDisconnected(disconnetedSession, errorCode);
}

void ListenerAreaServer::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}

void ListenerAreaServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerServerCommon::OnReceived(receiver, cmd);
}

void ListenerAreaServer::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
	ListenerServerCommon::OnReceived(session, recvPacket);
}

void ListenerAreaServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}


