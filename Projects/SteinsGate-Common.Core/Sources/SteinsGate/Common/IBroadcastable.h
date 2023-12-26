/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:32:13 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Declspec.h>
#include <JNetwork/Packet/Packet.h>

struct JCORE_NOVTABLE IBroadcastable
{
	virtual ~IBroadcastable() = default;
	virtual void BroadcastPacket(JNetwork::IPacket* packet) = 0;
};
