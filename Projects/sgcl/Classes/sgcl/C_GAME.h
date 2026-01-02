/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:19:21 AM
 * =====================
 *
 */


#pragma once


#include <jnet/Host/Session.h>

struct C_GAME
{
	static void OnConnected(jnet::Session* _pSession);
	static void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode);
	static void OnDisconnected(jnet::Session* _pSession);
};
