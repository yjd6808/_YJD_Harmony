/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 10:48:34 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Packet/Packet.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
void RecvedCmdPacket::ForEach(const jc::Action<ICommand*>& _consumer)
{
	int i = 0;
	char* pRead = payload_;

	if (header_.packetType_ != PacketType::Command)
	{
		jc_assert(false);
		return;
	}

	while (i < header_.cmdCount_)
	{
		ICommand* pCmd = reinterpret_cast<ICommand*>(pRead);
		_consumer(pCmd);

		pRead += pCmd->GetLength();
		++i;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
RecvedCmdPacket* RecvedCmdPacket::Clone() const
{
	RecvedCmdPacket* pCopy = reinterpret_cast<RecvedCmdPacket*>(dbg_new _u8[PACKET_HEADER_SIZE + header_.payloadLen_]);
	const _u8* pRead = reinterpret_cast<const _u8*>(this) + PACKET_HEADER_SIZE;
	jc::Memory::CopyUnsafe(pCopy, pRead, header_.payloadLen_); // 데이터영역 복사
	return pCopy;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RecvedCmdPacket::Delete()
{
	delete[] reinterpret_cast<_u8*>(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
CmdBufferPacket::CmdBufferPacket(const CommandBufferPtr& _buffer)
: CmdPacket(_buffer->GetCommandCount(), _buffer->GetPayloadLength())
, pBuf_(_buffer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
CmdBufferPacket::CmdBufferPacket(const jc::MemoryPoolAbstractPtr& _allocator, const CommandBufferPtr& _buffer)
: CmdPacket(_allocator, _buffer->GetCommandCount(), _buffer->GetPayloadLength())
, pBuf_(_buffer)
{
}

NS_END
