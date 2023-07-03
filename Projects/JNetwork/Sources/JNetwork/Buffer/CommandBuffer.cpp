/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 9:56:28 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Buffer/CommandBuffer.h>
#include <JNetwork/Packet/SendPacket.h>

NS_JNET_BEGIN

CommandBuffer::CommandBuffer(const JCore::MemoryPoolAbstractPtr& allocator, int bufferSize)
	: BufferAbstract()
	, m_Allocator(allocator)
	, m_iRequestBufferSize(0)
{
	int iRealAlloc;
	m_pBuffer = (char*)m_Allocator->DynamicPop(bufferSize, iRealAlloc);
	m_iBufferSize = iRealAlloc;
	m_iRequestBufferSize = bufferSize;

	Initialize();
}

CommandBuffer::CommandBuffer(const CommandBuffer& other)
	: BufferAbstract()
	, m_Allocator(other.m_Allocator)
	, m_iRequestBufferSize(0)
{
	int iRealAlloc;
	m_pBuffer = (char*)m_Allocator->DynamicPop(other.m_iRequestBufferSize, iRealAlloc);
	m_iBufferSize = iRealAlloc;
	m_iRequestBufferSize = other.m_iRequestBufferSize;
	DebugAssertMsg(iRealAlloc == other.GetBufferCapacity(), "머지..? 메모리풀 얼른 고쳐");

	JCore::Memory::Copy(m_pBuffer, m_iBufferSize, other.m_pBuffer, other.m_iBufferSize);

	m_iReadPos = other.m_iReadPos;
	m_iWritePos = other.m_iWritePos;
}

CommandBuffer::~CommandBuffer() {
	m_Allocator->DynamicPush(m_pBuffer, m_iRequestBufferSize);
}

CommandBufferPtr CommandBuffer::Create(const JCore::MemoryPoolAbstractPtr& allocator, int bufferSize) {
	return JCore::MakeShared<CommandBuffer>(allocator, bufferSize);
}

void CommandBuffer::Initialize() {
	JCore::Arrays::Fill(m_pBuffer, PacketHeaderSize_v, (char)0);

	m_iReadPos += PacketHeaderSize_v;
	m_iWritePos += PacketHeaderSize_v;
}

void CommandBuffer::AddCommandCount() {
	CmdCnt_t& uiCommandCount = *(CmdCnt_t*)m_pBuffer;
	++uiCommandCount;
}

void CommandBuffer::AddPacketLength(int size) {
	PktLen_t& uiPktLen = *(PktLen_t*)(m_pBuffer + sizeof(CmdCnt_t));
	uiPktLen += size;
}

bool CommandBuffer::IsValid() const {
	CommandBuffer dbgBuffer(*this);	// 기존꺼 무결성 보장

	int iWritePos = this->GetWritePos();

	dbgBuffer.ResetPosition();
	dbgBuffer.MoveWritePos(iWritePos);

	int iCommandCount = dbgBuffer.GetCommandCount();
	int iPacketLen = dbgBuffer.GetPacketLength();

	dbgBuffer.MoveReadPos(PacketHeaderSize_v);

	for (int i = 0; i < iCommandCount; i++) {
		ICommand* pCmd = dbgBuffer.Peek<ICommand*>();
		int iCommandLen = pCmd->GetCommandLen();

		if (dbgBuffer.MoveReadPos(iCommandLen) == false) {
			return false;
		}
	}

	return true;
}

CmdCnt_t CommandBuffer::GetCommandCount() {
	CmdCnt_t& uiCommandCount = *(CmdCnt_t*)m_pBuffer;
	return uiCommandCount;
}

PktLen_t CommandBuffer::GetPacketLength() {
	PktLen_t& uiPktLen = *(PktLen_t*)(m_pBuffer + sizeof(CmdCnt_t));
	return uiPktLen;
}


NS_JNET_END
