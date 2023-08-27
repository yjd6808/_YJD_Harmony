/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 10:44:36 PM
 * =====================
 */


#pragma once

struct ISessionEventHandler
{
	virtual ~ISessionEventHandler() = default;
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
};