/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 1:34:01 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/SessionEventListener.h>

NS_JNET_BEGIN

class ClientEventListener : public SessionEventListener
{
public:
	virtual void OnConnectFailed(Session* session, Int32U errorCode) {}	// Tcp클라이언트 전용
};



NS_JNET_END