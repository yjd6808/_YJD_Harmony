/*
 * 작성자 : 윤정도
 * 자주 사용하는 패킷 송신 함수
 */

#pragma once

#include <TF/Game/Player.h>

struct SendFn
{
	static void SendHardDisconnectSyn(Player* player);
	static void SendLoadCharacterInfoAck(Player* player);
};

