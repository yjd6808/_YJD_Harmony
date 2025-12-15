#pragma once

#include <JNetwork/Host/Session.h>

struct C_CHAT
{
	static void OnConnected(JNetwork::Session* _pSession);
	static void OnConnectFailed(JNetwork::Session* _pSession, Int32U _errorCode);
	static void OnDisconnected(JNetwork::Session* _pSession);
};
