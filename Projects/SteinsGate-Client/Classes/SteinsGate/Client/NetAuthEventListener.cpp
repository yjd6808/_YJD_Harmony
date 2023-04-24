/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetAuthEventListener.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

SGNetAuthEventListener::SGNetAuthEventListener(SGCommandParser* parser)
	: SGNetClientEventListener(parser)
{}

void SGNetAuthEventListener::OnStarted() {
	_LogPlain_("서버가 시작되었습니다.");
}

void SGNetAuthEventListener::OnConnected(JNetwork::Session* connectedSession) {
}

void SGNetAuthEventListener::OnDisconnected(JNetwork::Session* disconnetedSession) {
}

void SGNetAuthEventListener::OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void SGNetAuthEventListener::OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) {
}

void SGNetAuthEventListener::OnStopped() {
	_LogPlain_("서버가 중지되었습니다.");
}
