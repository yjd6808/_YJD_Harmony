/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:26:26 PM
 * =====================
 *
 */


#include "Core.h"
#include "ListenerHelperBase.h"

#include <jnet/Packet/PacketViewer.h>

#include <sg/Config.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerHelperBase::LogCommand(TransportProtocol _protocol, Transmission _transmission, ICommand* _pCommand)
{
	const Cmd_t id = _pCommand->GetId();

	if (_transmission == Transmission::Recv && !g_cConfigRuntime.showRecvCommand_)
	{
		return;
	}

	if (_transmission == Transmission::Send && !g_cConfigRuntime.showSendCommand_)
	{
		return;
	}

	if (g_cConfigRuntime.IsFilteredCommand(_transmission, id))
	{
		return;
	}

	const char* pName = sg::CommandNameDictionary.Get(id);

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
	if (g_cConfigRuntime.showRecvPacketHex_)
	{
		PacketViewer::View(_pRecvPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerHelperBase::LogPacketHex(IPacket* _pSentPacket)
{
	if (g_cConfigRuntime.showSendPacketHex_)
	{
		PacketViewer::View(_pSentPacket);
	}
}
