/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "ListenerLobbyServer.h"

#include <SteinsGate/Common/LobbyCmd.h>
#include <SteinsGate/Common/QueryDefine.h>
#include <SteinsGate/Common/PacketViewer.h>

#include <SteinsGate/Server/R_LOBBY.h>

USING_NS_JC;
USING_NS_JNET;

ListenerLobbyServer::ListenerLobbyServer() {
}

void ListenerLobbyServer::OnStarted() {
	ListenerCommonServer::OnStarted();
}

void ListenerLobbyServer::OnConnected(Session* connectedSession) {
	ListenerCommonServer::OnConnected(connectedSession);
}

void ListenerLobbyServer::OnDisconnected(Session* disconnetedSession) {
	ListenerCommonServer::OnDisconnected(disconnetedSession);
}

void ListenerLobbyServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	PacketViewer::View(sentPacket);
	sentPacket->ForEach([](ICommand* cmd) { PacketViewer::View(cmd); });
	ListenerCommonServer::OnSent(sender, sentPacket, sentBytes);
}

void ListenerLobbyServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerCommonServer::OnReceived(receiver, cmd);
}

void ListenerLobbyServer::OnStopped() {
	ListenerCommonServer::OnStopped();
}


