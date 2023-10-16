/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:08:19 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/EventListener/ServerEventListener.h>

class L_GAME : public JNetwork::ServerEventListener
{
	 void OnStarted();
	 void OnStartFailed(Int32U errorCode);
	 void OnStopped();
	 void OnConnected(JNetwork::Session* session);
	 void OnConnectFailed(JNetwork::Session* session, Int32U errorCode);
	 void OnDisconnected(JNetwork::Session* session);
	 void OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes);
	 void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd);
	 void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket);
};
