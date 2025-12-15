/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 10:14:21 PM
 * =====================
 *
 */


#pragma once

namespace JCore
{
class String;
}

namespace JNetwork
{
class IPacket;
class RecvedCommandPacket;
class ICommand;
enum class Transmission;
}

struct PacketViewer
{
	static void View(JNetwork::IPacket* _pPacket);
	static void View(char* _pData, int _len);
	static void View(JNetwork::RecvedCommandPacket* _pPacket);
	static void View(JNetwork::ICommand* _pCmd);

private:
	static void View(JNetwork::Transmission _transmission, char* _pData, int _len, int _cmdCount);
	static void View(JNetwork::Transmission _transmission, char* _pData, int _len);

	static void Hex(char* _pData, int _len, JCORE_OUT JCore::String& _hex);
};
