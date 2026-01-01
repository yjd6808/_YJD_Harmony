#pragma once

#include <jnet/Host/Session.h>

struct C_CHAT
{
	static void OnConnected(jnet::Session* _pSession);
	static void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode);
	static void OnDisconnected(jnet::Session* _pSession);
};
