/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 10:14:21 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Packet/Packet.h>
#include <jnet/Protocol.h>

NS_JNET_BEGIN

struct PacketViewer
{
	static void View(IPacket* _pPacket);
	static void View(char* _pData, int _len);
	static void View(RecvedCommandPacket* _pPacket);
	static void View(ICommand* _pCmd);

private:
	static void View(Transmission _transmission, char* _pData, int _len, int _cmdCount);
	static void View(Transmission _transmission, char* _pData, int _len);

	static void Hex(char* _pData, int _len, OUT jc::String& _hex);
};

NS_JNET_END