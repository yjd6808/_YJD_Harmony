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
PacketParser* PacketParser::Create(int _type, Session* _pSession)
{
	switch (_type)
	{
	case Raw:      return dbg_new PacketRawParser(_pSession);
	case Command:  return dbg_new PacketCommandParser(_pSession);
	default:
		jc_assert(false);
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
PacketRawParser::PacketRawParser(Session* _pSession)
	: PacketParser(_pSession)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketRawParser::Received(Int32UL _receivedBytes)
{
	CommandBuffer* pRecvBuffer = session_->GetRecvBuffer().GetPtr();
	pRecvBuffer->MoveWritePos(_receivedBytes);
	session_->NotifyRaw(pRecvBuffer->GetReadableSource(), _receivedBytes);
	pRecvBuffer->MoveReadPos(_receivedBytes);
	pRecvBuffer->PopReads();
}

//////////////////////////////////////////////////////////////////////////////////////////
PacketCommandParser::PacketCommandParser(Session* _pSession)
	: PacketParser(_pSession)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void PacketCommandParser::Received(Int32UL _receivedBytes)
{
	CommandBuffer* pRecvBuffer = session_->GetRecvBuffer().GetPtr();
	pRecvBuffer->MoveWritePos(_receivedBytes);

	for (;;)
	{
		const int readableBufferSize = pRecvBuffer->GetReadableBufferSize();

		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기다린다.
		if (readableBufferSize < PACKET_HEADER_SIZE)
			return;

		// 패킷 헤더 길이 + 패킷 길이 만큼 수신하지 않았으면 다시 모일때까지 기다린다.
		RecvedCommandPacket* pPacket = pRecvBuffer->Peek<RecvedCommandPacket*>();
		const int packetLength = pPacket->GetPacketLength();

		if (readableBufferSize < (PACKET_HEADER_SIZE + packetLength))
		{
			return;
		}

		pRecvBuffer->MoveReadPos(PACKET_HEADER_SIZE);
		session_->NotifyPacket(pPacket);

		for (int commandIndex = 0; commandIndex < pPacket->GetCommandCount(); ++commandIndex)
		{
			ICommand* pCommand = pRecvBuffer->Peek<ICommand*>();
			CmdLen_t commandLength = pCommand->GetLength();
			session_->NotifyCommand(pCommand);

			if (pRecvBuffer->MoveReadPos(commandLength) == false)
			{
				_NetLogWarn_("커맨드 크기가 이상합니다.");
				pRecvBuffer->ResetPosition();
				return;
			}
		}

		pRecvBuffer->PopReads();
	}
}

NS_JNET_END
