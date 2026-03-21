/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#include "jnet/Host/Session.h"

#include "PacketReader.h"


NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
PacketParser::PacketParser(Session* _pSession)
: session_(_pSession)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketParser::Received(_u32l _receivedBytes)
{
	PacketBuffer* pRecvBuffer = session_->GetRecvBuffer().GetPtr();
	pRecvBuffer->MoveWritePos(_receivedBytes);

	PacketReader reader(pRecvBuffer->GetReadableBuffer());

	for (;;)
	{
		PacketElementView view;
		_u32 readMemSize = 0;
		const int r = reader.Next(view, readMemSize);

		if (r == PacketReader::rrSuccess)
		{
			if (view.type_ == static_cast<_u8>(PacketType::Raw))
			{
				session_->NotifyRaw(view.raw_.buf, static_cast<int>(view.raw_.len));
			}
			else if (view.type_ == static_cast<_u8>(PacketType::Command))
			{
				session_->NotifyCommand(view.cmd_);
			}
			else if (view.type_ == static_cast<_u8>(PacketType::Message))
			{
				session_->NotifyMessage(view.msg_);
			}

			pRecvBuffer->MoveReadPos(static_cast<int>(readMemSize));
		}
		else if (r == PacketReader::rrReadEnd)
		{
			pRecvBuffer->PopReads();
			break;
		}
		else
		{
			switch (r)
			{
			case PacketReader::rrInvalidPacket_MagicNumberMismatch:
				_NetLogWarn_("패킷 매직넘버가 이상합니다. 패킷이 잘못된 것 같습니다.");
				pRecvBuffer->ResetPosition();
				break;
			case PacketReader::rrInvalidPacket_UnknownPacketType:
				_NetLogWarn_("알 수 없는 패킷 타입입니다.");
				pRecvBuffer->ResetPosition();
				break;
			case PacketReader::rrInvalidCmd:
				_NetLogWarn_("커맨드 크기가 이상합니다.");
				pRecvBuffer->ResetPosition();
				break;
			case PacketReader::rrInvalidMsg:
				_NetLogWarn_("메시지 크기가 이상합니다.");
				pRecvBuffer->ResetPosition();
				break;
				// 아직 데이터가 덜 모인 경우: readPos를 옮기지 않고 다음 수신을 기다린다.
			case PacketReader::rrReadHeaderFailed_InsufficientHeaderSize:
			case PacketReader::rrReadHeaderFailed_InsufficientPaylodeLen:
				break;
			default:
				_NetLogWarn_("PacketParser 알 수 없는 오류 (%d)", r);
				pRecvBuffer->ResetPosition();
				break;
			}
			break;
		}

		
	}
}
NS_END
