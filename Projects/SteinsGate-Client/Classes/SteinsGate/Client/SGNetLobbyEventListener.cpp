/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 08:52:07 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGNetLobbyEventListener.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

SGNetLobbyEventListener::SGNetLobbyEventListener(SGCommandParser* parser)
	: SGNetClientEventListener(parser)
{}

void SGNetLobbyEventListener::OnStarted() {
	_LogPlain_("서버가 시작되었습니다.");
}

void SGNetLobbyEventListener::OnConnected(JNetwork::Session* connectedSession) {
}

void SGNetLobbyEventListener::OnDisconnected(JNetwork::Session* disconnetedSession) {
}

void SGNetLobbyEventListener::OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void SGNetLobbyEventListener::OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) {
}

void SGNetLobbyEventListener::OnStopped() {
	_LogPlain_("서버가 중지되었습니다.");
}
