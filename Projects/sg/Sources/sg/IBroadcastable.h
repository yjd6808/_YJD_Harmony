/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:32:13 AM
 * =====================
 *
 */


#pragma once


#include <jc/Declspec.h>
#include <jnet/Packet/Packet.h>

struct JCORE_NOVTABLE IBroadcastable
{
	virtual ~IBroadcastable() = default;
	virtual void BroadcastPacket(jnet::IPacket* _pPacket) = 0;
};
