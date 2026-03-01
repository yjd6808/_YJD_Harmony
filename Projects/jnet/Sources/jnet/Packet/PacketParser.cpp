/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Host/Session.h>


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
	for (;;)
	{
		const int readableBufferSize = pRecvBuffer->GetReadableBufferSize();

		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기다린다.
		if (readableBufferSize < PACKET_HEADER_SIZE)
			return;

		PacketHeader* pHeader = pRecvBuffer->Peek<PacketHeader*>();
		if (pHeader->magicNumber_ != PACKET_MAGIC_NUMBER)
		{
			_NetLogWarn_("패킷 매직넘버가 이상합니다. 패킷이 잘못된 것 같습니다.");
			pRecvBuffer->ResetPosition();
			return;
		}

		_u16 packetLength = PACKET_HEADER_SIZE + pHeader->payloadLen_;
		if (readableBufferSize < packetLength)
			return; // 모일때까지 기달

		RecvedCmdPacket* pPacket = pRecvBuffer->Peek<RecvedCmdPacket*>();
		session_->NotifyPacket(pPacket);
		if (pHeader->packetType_ == PacketType::Raw)
		{
			session_->NotifyRaw(pPacket->payload_, pHeader->payloadLen_);
			pRecvBuffer->MoveReadPos(packetLength);
		}
		else if (pHeader->packetType_ == PacketType::Command)
		{
			pRecvBuffer->MoveReadPos(PACKET_HEADER_SIZE);
			for (int i = 0; i < pHeader->elemCount_; ++i)
			{
				ICommand* pCmd = pRecvBuffer->Peek<ICommand*>();
				CmdLen_t cmdLen = pCmd->GetLength();
				session_->NotifyCommand(pCmd);

				if (pRecvBuffer->MoveReadPos(cmdLen) == false)
				{
					_NetLogWarn_("커맨드 크기가 이상합니다.");
					pRecvBuffer->ResetPosition();
					return;
				}
			}
		}
		else
		{
			jc_assert(false);
		}

		pRecvBuffer->PopReads();
	}
}

NS_END
