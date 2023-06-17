/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:36:07 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

// 패킷을 받을 때는 가상 함수 테이블이 없는 구조체로 받자.
struct ICommand;
struct IRecvPacket
{
	IRecvPacket() = delete;
	~IRecvPacket() = delete;

	Int16U	GetPacketLength() const { return m_iPacketLen; }
	Int16U	GetCommandCount() const { return m_iCommandCount; }

	void ForEach(const JCore::Action<ICommand*>& consumer) {
		int iCmdIndex = 0;
		char* pCmdData = reinterpret_cast<char*>(this) + PacketHeaderSize_v;

		while (iCmdIndex < m_iCommandCount) {
			ICommand* pCurCmd = reinterpret_cast<ICommand*>(pCmdData);
			consumer(pCurCmd);
			pCmdData += pCurCmd->CmdLen;
			iCmdIndex += 1;
		}
	}
protected:
	Int16U m_iCommandCount;
	Int16U m_iPacketLen;
};


NS_JNET_END