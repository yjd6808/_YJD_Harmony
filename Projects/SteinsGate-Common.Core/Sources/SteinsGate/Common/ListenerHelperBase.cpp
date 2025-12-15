/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:26:26 PM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "ListenerHelperBase.h"

#include <SteinsGate/Common/PacketViewer.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerHelperBase::LogCommand(TransportProtocol _protocol, Transmission _transmission, ICommand* _pCommand)
{
	const Cmd_t id = _pCommand->GetId();

	if (_transmission == Transmission::Recv && !Core::RuntimeConfigBase->showRecvCommand_)
	{
		return;
	}

	if (_transmission == Transmission::Send && !Core::RuntimeConfigBase->showSendCommand_)
	{
		return;
	}

	if (Core::RuntimeConfigBase->IsFilteredCommand(_transmission, id))
	{
		return;
	}

	const char* pName = Core::CommandNameDictionary.Get(id);

	_LogDebug_("%c %s %sB %s(%d)",
	           TransmissionName(_transmission),
	           TransportProtocolName(_protocol),
	           StringUtil::FillLeft(_pCommand->GetLength(), ' ', 4).Source(),
	           pName,
	           id
	);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerHelperBase::LogPacketHex(RecvedCommandPacket* _pRecvPacket)
{
	if (Core::RuntimeConfigBase->showRecvPacketHex_)
	{
		PacketViewer::View(_pRecvPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerHelperBase::LogPacketHex(IPacket* _pSentPacket)
{
	if (Core::RuntimeConfigBase->showSendPacketHex_)
	{
		PacketViewer::View(_pSentPacket);
	}
}
