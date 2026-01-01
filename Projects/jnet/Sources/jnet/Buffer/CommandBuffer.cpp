/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 9:56:28 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Buffer/CommandBuffer.h>
#include <jnet/Packet/Packet.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
CommandBuffer::CommandBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize)
: BufferAbstract()
, requestBufferSize_(0)
, allocator_(_allocator)
{
	jc_assert_msg(allocator_ != nullptr, "할당자가 없습니다.");
	int realAlloc;
	buffer_ = (char*)allocator_->DynamicPop(_bufferSize, realAlloc);
	bufferSize_ = realAlloc;
	requestBufferSize_ = _bufferSize;

	Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBuffer::CommandBuffer(const CommandBuffer& _other)
: BufferAbstract()
, requestBufferSize_(0)
, allocator_(_other.allocator_)
{
	int realAlloc;
	buffer_ = (char*)allocator_->DynamicPop(_other.requestBufferSize_, realAlloc);
	bufferSize_ = realAlloc;
	requestBufferSize_ = _other.requestBufferSize_;
	jc_assert_msg(realAlloc == _other.GetBufferCapacity(), "머지..? 메모리풀 얼른 고쳐");

	jc::Memory::Copy(buffer_, bufferSize_, _other.buffer_, _other.bufferSize_);

	readPos_ = _other.readPos_;
	writePos_ = _other.writePos_;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBuffer::~CommandBuffer()
{
	allocator_->DynamicPush(buffer_, requestBufferSize_);
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandBufferPtr CommandBuffer::Create(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize)
{
	return jc::MakeShared<CommandBuffer>(_allocator, _bufferSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandBuffer::Initialize()
{
	jc::Arrays::Fill(buffer_, PACKET_HEADER_SIZE, (char)0);

	readPos_ += PACKET_HEADER_SIZE;
	writePos_ += PACKET_HEADER_SIZE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandBuffer::AddCommandCount()
{
	CmdCnt_t& commandCount = *(CmdCnt_t*)buffer_;
	++commandCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandBuffer::AddPacketLength(int _size)
{
	PktLen_t& pktLen = *(PktLen_t*)(buffer_ + sizeof(CmdCnt_t));
	pktLen += _size;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CommandBuffer::IsValid() const
{
	CommandBuffer dbgBuffer(*this); // 기존꺼 무결성 보장

	int writePos = GetWritePos();

	dbgBuffer.ResetPosition();
	dbgBuffer.MoveWritePos(writePos);

	int commandCount = dbgBuffer.GetCommandCount();
	int packetLen = dbgBuffer.GetPacketLength();

	dbgBuffer.MoveReadPos(PACKET_HEADER_SIZE);

	for (int i = 0; i < commandCount; i++)
	{
		ICommand* pCmd = dbgBuffer.Peek<ICommand*>();
		int commandLen = pCmd->GetLength();

		if (!dbgBuffer.MoveReadPos(commandLen))
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
CmdCnt_t CommandBuffer::GetCommandCount()
{
	CmdCnt_t& commandCount = *(CmdCnt_t*)buffer_;
	return commandCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
PktLen_t CommandBuffer::GetPacketLength()
{
	PktLen_t& pktLen = *(PktLen_t*)(buffer_ + sizeof(CmdCnt_t));
	return pktLen;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandBuffer::Alloc(ICommand* _pCmd)
{
	const int cmdSize = _pCmd->GetLength();

	if (!MoveWritePos(cmdSize))
	{
		jc_assert_msg(false, "버퍼에 커맨드를 쓸 공간이 부족합니다.");
	}

	char* pMem = Peek<char*>();
	jc::Memory::CopyUnsafe(pMem, _pCmd, cmdSize);
	MoveReadPos(cmdSize);

	AddCommandCount();
	AddPacketLength(cmdSize);
}

NS_JNET_END
