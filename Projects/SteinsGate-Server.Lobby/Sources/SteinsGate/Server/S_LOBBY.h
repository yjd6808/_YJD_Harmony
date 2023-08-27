/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:55:50 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Packet/SendHelper.h>
#include <SteinsGate/Common/Struct.h>

struct S_LOBBY : JNetwork::SendHelper<S_LOBBY>
{
	static void SEND_LOC_JoinLobbyAck(bool success);
};

