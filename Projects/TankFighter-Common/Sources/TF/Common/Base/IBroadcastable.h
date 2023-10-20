/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:32:13 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Declspec.h>
#include <JNetwork/Packet/SendPacket.h>

struct JCORE_NOVTABLE IBroadcastable
{
	virtual ~IBroadcastable() = default;
	virtual void BroadcastPacket(JNetwork::ISendPacket* packet, int state) = 0;
};