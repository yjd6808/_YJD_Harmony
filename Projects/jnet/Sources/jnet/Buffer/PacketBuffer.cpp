/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 9:56:28 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Buffer/PacketBuffer.h>
#include <jnet/Packet/Packet.h>

USING_NS_JNET;

#define MSG_PREFIX_HEADER_SIZE (0)

//////////////////////////////////////////////////////////////////////////////////////////
PacketBuffer::PacketBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize)
: PooledStreamBuffer(_allocator, _bufferSize)
{
	// 최초 패킷 헤더 영역은 디폴트로 초기화 해놓도록 한다.
	jc::Memory::CopyUnsafe(buffer_, &DefaultPacketHeader, sizeof(DefaultPacketHeader));
}

//////////////////////////////////////////////////////////////////////////////////////////
PacketBuffer::PacketBuffer(const PacketBuffer& _other)
: PooledStreamBuffer(_other)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PacketBuffer::IsValid() const
{
	PacketBuffer dbgBuffer(*this); // 기존꺼 무결성 보장

	int writePos = GetWritePos();

	dbgBuffer.ResetPosition();
	dbgBuffer.MoveWritePos(writePos);

	int commandCount = dbgBuffer.GetElemCount();
	int packetLen = dbgBuffer.GetPayloadLength();

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
ElemCnt_t PacketBuffer::GetElemCount()
{
	return GetPacketHeader().elemCount_;
}

//////////////////////////////////////////////////////////////////////////////////////////
PaloadLen_t PacketBuffer::GetPayloadLength()
{
	return GetPacketHeader().payloadLen_;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::SharedPtr<PacketBuffer> PacketBuffer::Create(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize)
{
	return jc::MakeShared<PacketBuffer>(_allocator, _bufferSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::CMessage PacketBuffer::EmplaceMsg()
{
	if (PreProcessElement(PacketType::Message) == false)
		return jc::CMessage::Null;

	int remaining = GetRemainBufferSize();
	if (remaining < jc::CMessage::MESSAGE_HEADER_SIZE + 5) // 5 byte = int 1개 이상 쓸 수 있는 최소한의 공간
	{
		_NetLogWarn_("패킷 버퍼에 메시지를 쓸 공간이 부족합니다. 남은 공간: %d", remaining);
		return jc::CMessage::Null; // null message 반환
	}

	currMsgStartOffset_ = writePos_;
	currMsgPending_ = true;	// 메시지 시작됨

	jc::CMessage msg;
	jc::CMessageView* pView = dbg_new jc::CMessageView(
		(_u8*)buffer_ + writePos_, 
		MSG_PREFIX_HEADER_SIZE, 
		(_u32)remaining, 
		GetAllocator());
	pView->InitHeader();

	msg.SetContext(pView);
	return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketBuffer::CompleteMsg()
{
	PacketHeader& header = GetPacketHeader();
	if (header.packetType_ != PacketType::Message)
	{
		jc_assert_msg(false, "메시지 패킷이 아닙니다.");
		return;
	}

	if (currMsgPending_ == false)
	{
		jc_assert_msg(false, "이미 메시지가 완성되어 있습니다.");
		return; // 중복 호출 금지.
	}

	auto pMsgHeaeder = (jc::CMessageHeader*)(buffer_ + currMsgStartOffset_);
	++header.elemCount_;

	_u16 msgTotalSize = MSG_PREFIX_HEADER_SIZE + jc::CMessage::MESSAGE_HEADER_SIZE + pMsgHeaeder->writeMemOffset_;
	header.payloadLen_ += msgTotalSize;
	writePos_ += msgTotalSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketBuffer::EmplaceCmd(ICommand* _pCmd)
{
	if (PreProcessElement(PacketType::Command) == false)
		return;

	const int cmdSize = _pCmd->GetLength();
	char* pCmdMem = buffer_ + writePos_;

	if (!MoveWritePos(cmdSize))
	{
		jc_assert_msg(false, "버퍼에 커맨드를 쓸 공간이 부족합니다.");
	}

	jc::Memory::CopyUnsafe(pCmdMem, _pCmd, cmdSize);
	PacketHeader& header = GetPacketHeader();
	++header.elemCount_;
	header.payloadLen_ += cmdSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
// : 패킷 버퍼에 element (msg 또는 cmd)를 쓰기전에 체크/수행해야할 전처리
//////////////////////////////////////////////////////////////////////////////////////////
bool PacketBuffer::PreProcessElement(PacketType_t _type)
{
	PacketHeader* pHeader = GetPacketHeaderPtr();
	if (currMsgPending_ && pHeader->packetType_ == PacketType::Message)
	{
		// 메시지 패킷의 경우 remain size를 CMessage의 pBuf_로 전체 할당해주기 때문에
		// 다음 element를 쓰기전에 CMessage에 실제로 Write한 메모리 사이즈만큼 패킷 버퍼의 writePos_를 옮겨주는 전처리 작업이 필요하다.
		CompleteMsg();
	}

	if (pHeader->packetType_ == _type)
	{
		return true; // 이미 초기화 되어있다. 그냥 그대로 사용하면 된다.
	}

	int remaining = GetRemainBufferSize();
	if (remaining < PACKET_HEADER_SIZE)
	{
		_NetLogWarn_("버퍼에 패킷 헤더를 쓸 공간이 부족합니다. 남은 공간: %d", remaining);
		return false;
	}

	currPacketHeaderOffset_ = writePos_;
	currMsgPending_ = false;	// 타입이 바뀌면 진행 중 메시지는 없다고 간주

	pHeader = GetPacketHeaderPtr();
	pHeader->packetType_ = _type;
	pHeader->magicNumber_ = PACKET_MAGIC_NUMBER;
	pHeader->payloadLen_ = 0;
	pHeader->elemCount_ = 0;

	writePos_ += PACKET_HEADER_SIZE;
	return true;
}