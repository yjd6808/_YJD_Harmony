/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 10:48:34 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/Packet.h>

NS_JNET_BEGIN

void RecvedCommandPacket::ForEach(const JCore::Action<ICommand*>& consumer) {
	int iCmdIndex = 0;
	char* pCmdData = reinterpret_cast<char*>(this) + PacketHeaderSize_v;

	while (iCmdIndex < m_iCommandCount) {
		ICommand* pCurCmd = reinterpret_cast<ICommand*>(pCmdData);
		consumer(pCurCmd);
		pCmdData += pCurCmd->CmdLen;
		iCmdIndex += 1;
	}
}

RecvedCommandPacket* RecvedCommandPacket::Clone() const {
	RecvedCommandPacket* pCopy = (RecvedCommandPacket*)dbg_new char[PacketHeaderSize_v + m_iPacketLen];
	const char* pCmdData = (char*)this + PacketHeaderSize_v;

	JCore::Memory::CopyUnsafe(pCopy, pCmdData, m_iPacketLen);	// 데이터영역 복사
	pCopy->m_iPacketLen = m_iPacketLen;
	pCopy->m_iCommandCount = m_iCommandCount;
	return pCopy;
}


CommandBufferPacket::CommandBufferPacket(const CommandBufferPtr& buffer)
	: CommandPacket(buffer->GetCommandCount(), buffer->GetPacketLength())
	, m_Buffer(buffer)
{}

CommandBufferPacket::CommandBufferPacket(const JCore::MemoryPoolAbstractPtr& allocator, const CommandBufferPtr& buffer)
	: CommandPacket(allocator, buffer->GetCommandCount(), buffer->GetPacketLength())
	, m_Buffer(buffer)
{}


NS_JNET_END