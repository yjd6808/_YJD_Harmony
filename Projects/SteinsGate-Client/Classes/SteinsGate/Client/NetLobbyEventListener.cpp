/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 08:52:07 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetLobbyEventListener.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void NetLobbyEventListener::OnConnected(Session* session) {
}
void NetLobbyEventListener::OnDisconnected(Session* session) {
}
void NetLobbyEventListener::OnConnectFailed(Session* session, Int32U errorCode) {
}
void NetLobbyEventListener::OnSent(Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}
void NetLobbyEventListener::OnReceived(Session* session, JNetwork::ICommand* cmd) {
}