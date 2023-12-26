/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ListenerChatServer.h"

USING_NS_JC;
USING_NS_JNET;

ListenerChatServer::ListenerChatServer(ChatServer* server, CommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pChatTcp(server)
{}

void ListenerChatServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerChatServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerChatServer::OnDisconnected(Session* disconnetedSession) {
	ListenerServerCommon::OnDisconnected(disconnetedSession);
}

void ListenerChatServer::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}

void ListenerChatServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerServerCommon::OnReceived(receiver, cmd);
}

void ListenerChatServer::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
	ListenerServerCommon::OnReceived(session, recvPacket);
}

void ListenerChatServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}


