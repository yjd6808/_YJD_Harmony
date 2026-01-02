/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:24:28 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Packet/Command.h>
#include <jnet/Protocol.h>

struct ListenerHelperBase
{
	static void LogCommand(jnet::TransportProtocol _protocol, jnet::Transmission _transmission, jnet::ICommand* _pCommand);
	static void LogPacketHex(jnet::RecvedCommandPacket* _pRecvPacket);
	static void LogPacketHex(jnet::IPacket* _pSentPacket);
};
