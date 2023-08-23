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

void ListenerHelperBase::LogCommand(TransportProtocol protocol, Transmission transmission, ICommand* cmd) {
	const Cmd_t id = cmd->GetCommand();

	if (transmission == Transmission::Recv && (!Core::RuntimeConfigBase->ShowRecvCommand || Core::RuntimeConfigBase->RecvCommandFilter.Exist(id))) {
		return;
	}

	if (transmission == Transmission::Send && (!Core::RuntimeConfigBase->ShowSendCommand || Core::RuntimeConfigBase->SendCommandFilter.Exist(id))) {
		return;
	}

	const char* szName = Core::CommandNameDictionary.Get(id);

	_LogDebug_("%c %s %sB %s(%d)", 
		TransmissionName(transmission),
		TransportProtocolName(protocol), 
		StringUtil::FillLeft(cmd->CmdLen, ' ', 4).Source(), 
		szName, 
		id
	);

}

void ListenerHelperBase::LogPacketHex(IRecvPacket* recvPacket) {
	if (Core::RuntimeConfigBase->ShowRecvPacketHex)
		PacketViewer::View(recvPacket);
}

void ListenerHelperBase::LogPacketHex(ISendPacket* sentPacket) {
	if (Core::RuntimeConfigBase->ShowSendPacketHex)
		PacketViewer::View(sentPacket);
}
