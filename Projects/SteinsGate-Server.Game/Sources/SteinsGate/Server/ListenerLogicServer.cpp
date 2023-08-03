/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ListenerLogicServer.h"

#include <SteinsGate/Common/GameCmd.h>
#include <SteinsGate/Common/QueryDefine.h>
#include <SteinsGate/Common/PacketViewer.h>

#include <SteinsGate/Server/R_LOGIC.h>

USING_NS_JC;
USING_NS_JNET;

ListenerLogicServer::ListenerLogicServer() {
}

void ListenerLogicServer::OnStarted() {
	ListenerCommonServer::OnStarted();
}

void ListenerLogicServer::OnConnected(Session* connectedSession) {
	ListenerCommonServer::OnConnected(connectedSession);
}

void ListenerLogicServer::OnDisconnected(Session* disconnetedSession) {
	ListenerCommonServer::OnDisconnected(disconnetedSession);
}

void ListenerLogicServer::OnSent(Session* sender, ISendPacket* sentPacket, Int32UL sentBytes) {
	PacketViewer::View(sentPacket);
	sentPacket->ForEach([](ICommand* cmd) { PacketViewer::View(cmd); });
	ListenerCommonServer::OnSent(sender, sentPacket, sentBytes);
}

void ListenerLogicServer::OnReceived(Session* receiver, ICommand* cmd) {
	ListenerCommonServer::OnReceived(receiver, cmd);
}

void ListenerLogicServer::OnStopped() {
	ListenerCommonServer::OnStopped();
}


