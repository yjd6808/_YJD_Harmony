/*
 * 작성자: 윤정도
 * 생성일: 3/3/2026 9:17:36 PM
 * =====================
 */


#include "jnet/Packet/PacketReader.h"

USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
PacketReader::PacketReader(WSABUF _buf)
: packetBuf_(_buf)
, msgView_(nullptr, 0, 0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
PacketReader::PacketReader(char* _pBuf, _u32 _len)
: msgView_(nullptr, 0, 0)
{
	packetBuf_.buf = _pBuf;
	packetBuf_.len = _len;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketReader::Reset()
{
	isValidPacket_ = false;
	currHeaderMemOffset_ = 0;
	readOffset_ = 0;
	readMemOffset_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
int PacketReader::Next(OUT PacketElementView& _view)
{
	_u32 dummySize = 0;
	return Next(_view, dummySize);
}

//////////////////////////////////////////////////////////////////////////////////////////
int PacketReader::Next(OUT PacketElementView& _view, OUT _u32& _readMemSize)
{
	if (packetBuf_.buf == nullptr)
		return rrInvalidPacket_BufferIsNull;

	_readMemSize = 0;

	for (;;)
	{
		_u32 remaining = packetBuf_.len - readMemOffset_;
		PacketHeader* pHeader = nullptr;
		// 헤더를 아직 읽지 않은 경우 헤더부터 읽는다.
		if (!isValidPacket_)
		{
			if (remaining == 0)
				return rrReadEnd;
			if (remaining < PACKET_HEADER_SIZE)
				return rrReadHeaderFailed_InsufficientHeaderSize;

			currHeaderMemOffset_ = readMemOffset_;
			pHeader = (PacketHeader*)(packetBuf_.buf + currHeaderMemOffset_);
			if (pHeader->magicNumber_ != PACKET_MAGIC_NUMBER)
				return rrInvalidPacket_MagicNumberMismatch;
			if ((PACKET_HEADER_SIZE + pHeader->payloadLen_) > remaining)
				return rrReadHeaderFailed_InsufficientPaylodeLen; // 패킷이 완전히 도착하지 않은 상태

			readMemOffset_ += PACKET_HEADER_SIZE;
			_readMemSize += PACKET_HEADER_SIZE;
			remaining -= PACKET_HEADER_SIZE;
			isValidPacket_ = true;
			readOffset_ = 0;
		}
		else
		{
			pHeader = (PacketHeader*)(packetBuf_.buf + currHeaderMemOffset_);
		}

		// Raw 패킷은 payload 전체를 한 번에 반환
		if (pHeader->packetType_ == PacketType::Raw)
		{
			_view.raw_.buf = packetBuf_.buf + readMemOffset_;
			_view.raw_.len = pHeader->payloadLen_;
			_view.type_ = static_cast<_u8>(PacketType::Raw);
			readMemOffset_ += pHeader->payloadLen_;
			_readMemSize += pHeader->payloadLen_;

			isValidPacket_ = false;
			return rrSuccess;
		}

		// Command 패킷: 현재 위치의 ICommand 하나를 반환
		if (pHeader->packetType_ == PacketType::Command)
		{
			if (readOffset_ >= pHeader->elemCount_)
			{
				// 현재 패킷의 모든 커맨드를 소진함. 다음 헤더를 시도한다.
				isValidPacket_ = false;
				continue;
			}

			ICommand* pCmd = reinterpret_cast<ICommand*>(packetBuf_.buf + readMemOffset_);
			CmdLen_t cmdLen = pCmd->GetLength();
			if (cmdLen > remaining || cmdLen == 0) // packetLen체크를 다했는데 이럴 수 없다.
				return rrInvalidCmd;

			_view.cmd_ = pCmd;
			_view.type_ = static_cast<_u8>(PacketType::Command);

			++readOffset_;
			readMemOffset_ += cmdLen;
			_readMemSize += cmdLen;
			return rrSuccess;
		}

		// Message 패킷: 현재 위치의 단일 메시지 하나를 반환
		if (pHeader->packetType_ == PacketType::Message)
		{
			if (readOffset_ >= pHeader->elemCount_)
			{
				// 현재 패킷의 모든 메시지를 소진함. 다음 헤더를 시도한다.
				isValidPacket_ = false;
				continue;
			}

			if (remaining < jc::CMessage::MESSAGE_HEADER_SIZE)
			{
				jc_assert(false);
				return rrInvalidMsg;
			}

			char* pElemSource = packetBuf_.buf + readMemOffset_;
			jc::CMessageHeader* pMsgHeader = reinterpret_cast<jc::CMessageHeader*>(pElemSource);
			const _u32 msgLen = jc::CMessage::MESSAGE_HEADER_SIZE + pMsgHeader->writeMemOffset_;
			if (msgLen > remaining)
				return rrInvalidMsg;

			msgView_ = jc::CMessageView(reinterpret_cast<_u8*>(pElemSource), 0, msgLen, nullptr);
			msgView_.SetStackAllocatedContext(true);

			_view.msg_ = jc::CMessage();
			_view.msg_.SetContext(&msgView_);
			_view.type_ = static_cast<_u8>(PacketType::Message);

			++readOffset_;
			readMemOffset_ += msgLen;
			_readMemSize += msgLen;
			return rrSuccess;
		}

		return rrInvalidPacket_UnknownPacketType;
	}
}
