/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerClientCommon.h"

USING_NS_JC;
USING_NS_JNET;

ListenerClientCommon::ListenerClientCommon(CommandParser* parser)
	: Parser(parser)
{}

void ListenerClientCommon::OnReceived(Session* session, ICommand* recvCmd) {
	ListenerClientBase::OnReceived(session, recvCmd);

	if (Parser && !Parser->RunCommand(session, recvCmd)) {
		const Cmd_t id = recvCmd->GetCommand();
		const char* szName = Core::CommandNameDictionary.Get(id);

		_LogWarn_("%c %s %sB %s(%d) Parse Faliled",
			TransmissionName(Transmission::Recv),
			TransportProtocolName(session->Protocol()),
			StringUtil::FillLeft(recvCmd->CmdLen, ' ', 4).Source(),
			szName,
			id
		);
	}
}

void ListenerClientCommon::OnReceived(Session* session, RecvedCommandPacket* recvPacket) {
	ListenerClientBase::OnReceived(session, recvPacket);
}
