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

	if (transmission == Transmission::Recv && !Core::RuntimeConfigBase->ShowRecvCommand) {
		return;
	}

	if (transmission == Transmission::Send && !Core::RuntimeConfigBase->ShowSendCommand) {
		return;
	}

	if (Core::RuntimeConfigBase->IsFilteredCommand(transmission, id)) {
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

void ListenerHelperBase::LogPacketHex(RecvedCommandPacket* recvPacket) {
	if (Core::RuntimeConfigBase->ShowRecvPacketHex)
		PacketViewer::View(recvPacket);
}

void ListenerHelperBase::LogPacketHex(IPacket* sentPacket) {
	if (Core::RuntimeConfigBase->ShowSendPacketHex)
		PacketViewer::View(sentPacket);
}
