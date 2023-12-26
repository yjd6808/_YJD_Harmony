/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "ListenerLobbyServer.h"

#include <SteinsGate/Common/QueryDefine.h>
#include <SteinsGate/Common/PacketViewer.h>

#include <SteinsGate/Server/R_LOBBY.h>

USING_NS_JC;
USING_NS_JNET;

ListenerLobbyServer::ListenerLobbyServer(LobbyServer* server, CommandParser* parser)
	: ListenerServerCommon(parser)
	, m_pLobbyTcp(server)
{}

void ListenerLobbyServer::OnStarted() {
	ListenerServerCommon::OnStarted();
}

void ListenerLobbyServer::OnConnected(Session* connectedSession) {
	ListenerServerCommon::OnConnected(connectedSession);
}

void ListenerLobbyServer::OnDisconnected(Session* disconnetedSession) {
	ListenerServerCommon::OnDisconnected(disconnetedSession);
}

void ListenerLobbyServer::OnSent(Session* sender, IPacket* sentPacket, Int32UL sentBytes) {
	ListenerServerCommon::OnSent(sender, sentPacket, sentBytes);
}

void ListenerLobbyServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerServerCommon::OnReceived(receiver, cmd);
}

void ListenerLobbyServer::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
	ListenerServerCommon::OnReceived(session, recvPacket);
}


void ListenerLobbyServer::OnStopped() {
	ListenerServerCommon::OnStopped();
}


