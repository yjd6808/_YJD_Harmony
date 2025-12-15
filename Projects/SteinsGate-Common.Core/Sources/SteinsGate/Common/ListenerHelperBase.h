/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:24:28 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>
#include <JNetwork/Protocol.h>

struct ListenerHelperBase
{
	static void LogCommand(JNetwork::TransportProtocol _protocol, JNetwork::Transmission _transmission,
	                       JNetwork::ICommand* _pCommand);
	static void LogPacketHex(JNetwork::RecvedCommandPacket* _pRecvPacket);
	static void LogPacketHex(JNetwork::IPacket* _pSentPacket);
};
