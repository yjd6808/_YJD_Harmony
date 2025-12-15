/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:19:21 AM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/Session.h>

struct C_GAME
{
	static void OnConnected(JNetwork::Session* _pSession);
	static void OnConnectFailed(JNetwork::Session* _pSession, Int32U _errorCode);
	static void OnDisconnected(JNetwork::Session* _pSession);
};
