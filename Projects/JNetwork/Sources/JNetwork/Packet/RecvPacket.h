/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:36:07 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/RecvPacket.h>

 // 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
struct IRecvPacket
{
	IRecvPacket() = delete;
	~IRecvPacket() = delete;

	Int16U	GetPacketLength() const { return m_iPacketLen; }
	Int16U	GetCommandCount() const { return m_iCommandCount; }
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;
};
