/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:09:18 PM
 * =====================
 *
 */


#pragma once

class C_GAME
{
public:
	static void ConnectedSynchronized();
	static void ConnectFailedSynchronized(Int32U errorCode);
	static void DisconnectedSynchronized();
};
