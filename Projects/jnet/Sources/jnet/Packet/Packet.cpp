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
void RecvedCommandPacket::ForEach(const jc::Action<ICommand*>& _consumer)
{
	int commandIndex = 0;
	char* pCommandData = reinterpret_cast<char*>(this) + PACKET_HEADER_SIZE;

	while (commandIndex < commandCount_)
	{
		ICommand* pCurrentCommand = reinterpret_cast<ICommand*>(pCommandData);
		_consumer(pCurrentCommand);

		pCommandData += pCurrentCommand->GetLength();
		++commandIndex;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
RecvedCommandPacket* RecvedCommandPacket::Clone() const
{
	RecvedCommandPacket* pCopy = reinterpret_cast<RecvedCommandPacket*>(dbg_new char[PACKET_HEADER_SIZE + packetLength_]);
	const char* pCommandData = reinterpret_cast<const char*>(this) + PACKET_HEADER_SIZE;

	jc::Memory::CopyUnsafe(pCopy, pCommandData, packetLength_); // 데이터영역 복사
	pCopy->packetLength_ = packetLength_;
	pCopy->commandCount_ = commandCount_;
	return pCopy;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBufferPacket::CommandBufferPacket(const CommandBufferPtr& _buffer)
: CommandPacket(_buffer->GetCommandCount(), _buffer->GetPacketLength())
, buffer_(_buffer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBufferPacket::CommandBufferPacket(const jc::MemoryPoolAbstractPtr& _allocator,
                                         const CommandBufferPtr& _buffer)
: CommandPacket(_allocator, _buffer->GetCommandCount(), _buffer->GetPacketLength())
, buffer_(_buffer)
{
}

NS_JNET_END
