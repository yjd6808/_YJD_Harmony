/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Host/Session.h>


NS_JNET_BEGIN

PacketParser::PacketParser(Session* session)
	: m_pSession(session)
{}

PacketParser* PacketParser::Create(int type, Session* session) {
	switch (type) {
	case Raw:	  return dbg_new PacketRawParser(session);
	case Command: return dbg_new PacketCommandParser(session);
	default: DebugAssert(false);
	}
	return nullptr;
}

PacketRawParser::PacketRawParser(Session* session) : PacketParser(session) {}

void PacketRawParser::Received(Int32UL receivedBytes) {
	CommandBuffer* pRecvBuffer = m_pSession->GetRecvBuffer().GetPtr();
	pRecvBuffer->MoveWritePos(receivedBytes);
	m_pSession->NotifyRaw(pRecvBuffer->GetReadableSource(), receivedBytes);
	pRecvBuffer->MoveReadPos(receivedBytes);
	pRecvBuffer->PopReads();
}

PacketCommandParser::PacketCommandParser(Session* session) : PacketParser(session) {}

void PacketCommandParser::Received(Int32UL receivedBytes) {
	CommandBuffer* pRecvBuffer = m_pSession->GetRecvBuffer().GetPtr();
	pRecvBuffer->MoveWritePos(receivedBytes);

	for (;;) {
		const int iReadableBufferSize = pRecvBuffer->GetReadableBufferSize();
		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기다린다.
		if (iReadableBufferSize < PacketHeaderSize_v)
			return;

		// 패킷 헤더 길이 + 패킷 길이 만큼 수신하지 않았으면 다시 모일때까지 기다린다.
		RecvedCommandPacket* packet = pRecvBuffer->Peek<RecvedCommandPacket*>();
		const int iPacketLength = packet->GetPacketLength();
		if (iReadableBufferSize < (PacketHeaderSize_v + iPacketLength)) {
			return;
		}

		pRecvBuffer->MoveReadPos(PacketHeaderSize_v);
		m_pSession->NotifyPacket(packet);

		for (int i = 0; i < packet->GetCommandCount(); i++) {
			ICommand* pCmd = pRecvBuffer->Peek<ICommand*>();
			CmdLen_t uiCmdLen = pCmd->GetCommandLen();
			m_pSession->NotifyCommand(pCmd);

			if (pRecvBuffer->MoveReadPos(uiCmdLen) == false) {
				_NetLogWarn_("커맨드 크기가 이상합니다.");
				pRecvBuffer->ResetPosition();
				return;
			}
		}

		pRecvBuffer->PopReads();
	}
}

NS_JNET_END
