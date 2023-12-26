/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerServerCommon.h"

USING_NS_JC;
USING_NS_JNET;

ListenerServerCommon::ListenerServerCommon(CommandParser* parser)
	: Parser(parser)
{}

void ListenerServerCommon::OnReceived(Session* session, ICommand* recvCmd) {
	ListenerServerBase::OnReceived(session, recvCmd);

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

void ListenerServerCommon::OnReceived(Session* session, RecvedCommandPacket* packet) {
	ListenerServerBase::OnReceived(session, packet);
}
