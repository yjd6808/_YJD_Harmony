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

//////////////////////////////////////////////////////////////////////////////////////////
ListenerClientCommon::ListenerClientCommon(CommandParser* _pParser)
: pParser_(_pParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerClientCommon::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	ListenerClientBase::OnReceived(_pSession, _pRecvCmd);

	if (pParser_ && !pParser_->RunCommand(_pSession, _pRecvCmd))
	{
		const Cmd_t id = _pRecvCmd->GetId();
		const char* pCommandName = Core::CommandNameDictionary.Get(id);

		_LogWarn_("%c %s %sB %s(%d) Parse Faliled",
		          TransmissionName(Transmission::Recv),
		          TransportProtocolName(_pSession->Protocol()),
		          StringUtil::FillLeft(_pRecvCmd->GetLength(), ' ', 4).Source(),
		          pCommandName,
		          id
		);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerClientCommon::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerClientBase::OnReceived(_pSession, _pRecvPacket);
}
