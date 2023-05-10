/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:19:21 AM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/Session.h>

struct C_LOBBY
{
	static void OnConnected(JNetwork::Session* session);
	static void OnConnectFailed(JNetwork::Session* session, Int32U errorCode);
	static void OnDisconnected(JNetwork::Session* session);
};