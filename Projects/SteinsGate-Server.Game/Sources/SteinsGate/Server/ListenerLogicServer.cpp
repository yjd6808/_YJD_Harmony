/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ListenerLogicServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerLogicServer::ListenerLogicServer(LogicServer* server, CommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pLogicTcp(server)
{}

void ListenerLogicServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerLogicServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerLogicServer::OnDisconnected(Session* disconnetedSession) {
	ListenerServerCommon::OnDisconnected(disconnetedSession);
}

void ListenerLogicServer::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}

void ListenerLogicServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerServerCommon::OnReceived(receiver, cmd);
}

void ListenerLogicServer::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
	ListenerServerCommon::OnReceived(session, recvPacket);
}

void ListenerLogicServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}


