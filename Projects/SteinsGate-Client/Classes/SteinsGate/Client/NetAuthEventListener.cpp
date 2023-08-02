/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetAuthEventListener.h"

#include <SteinsGate/Common/PacketViewer.h>

#include <SteinsGate/Client/S_AUTH.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void NetAuthEventListener::OnConnected(Session* session) {
	SyncConnectionResult(ClientConnectServerType::Auth, session, true, 0);
}
void NetAuthEventListener::OnDisconnected(Session* session) {
	SyncDisconnectionResult(ClientConnectServerType::Auth, session);
}

void NetAuthEventListener::OnConnectFailed(Session* session, Int32U errorCode) {
	SyncConnectionResult(ClientConnectServerType::Auth, session, false, errorCode);
}

void NetAuthEventListener::OnSent(Session* session, ISendPacket* sentPacket, Int32UL sentBytes) {
}

void NetAuthEventListener::OnReceived(Session* session, ICommand* cmd) {
	SyncReceivedCommand(ClientConnectServerType::Auth, session, cmd);
}

void NetAuthEventListener::OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* packet) {
	PacketViewer::View(packet);
	packet->ForEach([](ICommand* cmd) { PacketViewer::View(cmd); });
}
