/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class NetClientEventListener : public JNetwork::ClientEventListener
{
protected:
	static void SyncConnectionResult(ClientConnectServerType_t listenerType, JNetwork::Session* session, bool success, Int32U errorCode);
	static void SyncDisconnectionResult(ClientConnectServerType_t listenerType, JNetwork::Session* session);
	static void SyncReceivedCommand(ClientConnectServerType_t listenerType, SGSession* session, JNetwork::ICommand* cmd);
};


