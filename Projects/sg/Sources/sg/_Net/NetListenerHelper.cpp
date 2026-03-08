/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:26:26 PM
 * =====================
 *
 */


#include "Core.h"
#include "_Net/NetListenerHelper.h"

#include <jnet/Packet/PacketViewer.h>

#include <sg/_Core/AppConfig.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
void NetListenerHelper::LogRaw(jnet::TransportProtocol _protocol, jnet::Transmission _transmission, WSABUF _buf)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetListenerHelper::LogMessage(jnet::TransportProtocol _protocol, jnet::Transmission _transmission, const jc::CMessage& _msg)
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void NetListenerHelper::LogCommand(TransportProtocol _protocol, Transmission _transmission, ICommand* _pCommand)
{
	const Cmd_t id = _pCommand->GetId();

	if (_transmission == Transmission::Recv && !g_cAppConfig.showRecvCommand_)
	{
		return;
	}

	if (_transmission == Transmission::Send && !g_cAppConfig.showSendCommand_)
	{
		return;
	}

	if (g_cAppConfig.IsFilteredCommand(_transmission, id))
	{
		return;
	}

	const char* pName = sg::CommandNameDictionary.Get(id);

	_LogDebug_("%c %s %sB %s(%d)",
		TransmissionName(_transmission),
		TransportProtocolName(_protocol),
		StringUtil::FillLeft(_pCommand->GetLength(), ' ', 4).Source(),
		pName,
		id);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetListenerHelper::LogPacketHex(RecvedPacket* _pRecvPacket)
{
	if (g_cAppConfig.showRecvPacketHex_)
	{
		PacketViewer::View(_pRecvPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetListenerHelper::LogPacketHex(IPacket* _pSentPacket)
{
	if (g_cAppConfig.showSendPacketHex_)
	{
		PacketViewer::View(_pSentPacket);
	}
}
