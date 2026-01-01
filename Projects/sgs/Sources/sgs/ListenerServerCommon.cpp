/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCoreHeader.h"
#include "ListenerServerCommon.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerServerCommon::ListenerServerCommon(CommandParser* _pParser)
: pParser_(_pParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerCommon::OnReceived(Session* _pSession, ICommand* _pRecvCmd)
{
	ListenerServerBase::OnReceived(_pSession, _pRecvCmd);

	if (pParser_ && !pParser_->RunCommand(_pSession, _pRecvCmd))
	{
		const Cmd_t id = _pRecvCmd->GetId();
		const char* pName = sg::CommandNameDictionary.Get(id);

		_LogWarn_("%c %s %sB %s(%d) Parse Faliled",
		          TransmissionName(Transmission::Recv),
		          TransportProtocolName(_pSession->Protocol()),
		          StringUtil::FillLeft(_pRecvCmd->GetLength(), ' ', 4).Source(),
		          pName,
		          id
		);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerServerCommon::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ListenerServerBase::OnReceived(_pSession, _pRecvPacket);
}
