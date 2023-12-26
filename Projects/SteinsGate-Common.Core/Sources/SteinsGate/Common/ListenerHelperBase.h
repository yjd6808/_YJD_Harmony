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
	static void LogCommand(JNetwork::TransportProtocol protocol, JNetwork::Transmission transmission, JNetwork::ICommand* cmd);
	static void LogPacketHex(JNetwork::RecvedCommandPacket* recvPacket);
	static void LogPacketHex(JNetwork::IPacket* sentPacket);
};
