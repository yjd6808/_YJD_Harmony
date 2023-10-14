/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 11:00:11 AM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/Command.h>
#include <JNetwork/Packet/RecvPacket.h>

NS_JNET_BEGIN

void IRecvPacket::ForEach(const JCore::Action<ICommand*>& consumer) {
	int iCmdIndex = 0;
	char* pCmdData = reinterpret_cast<char*>(this) + PacketHeaderSize_v;

	while (iCmdIndex < m_iCommandCount) {
		ICommand* pCurCmd = reinterpret_cast<ICommand*>(pCmdData);
		consumer(pCurCmd);
		pCmdData += pCurCmd->CmdLen;
		iCmdIndex += 1;
	}
}

IRecvPacket* IRecvPacket::Clone() const {
	IRecvPacket* pCopy = (IRecvPacket*)dbg_new char[PacketHeaderSize_v + m_iPacketLen];
	const char* pCmdData = (char*)this + PacketHeaderSize_v;

	JCore::Memory::CopyUnsafe(pCopy, pCmdData, m_iPacketLen);	// 데이터영역 복사
	pCopy->m_iPacketLen = m_iPacketLen;
	pCopy->m_iCommandCount = m_iCommandCount;
	return pCopy;
}

NS_JNET_END
