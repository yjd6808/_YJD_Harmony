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
	struct IPacket;
}

struct PacketViewer
{
	static void View(JNetwork::IPacket* packet);
	static void View(char* data, int len);
	static void View(JNetwork::RecvedCommandPacket* packet);
	static void View(JNetwork::ICommand* cmd);
private:
	static void View(JNetwork::Transmission transmission, char* data, int len, int cmdCount);
	static void View(JNetwork::Transmission transmission, char* data, int len);

	static void Hex(char* data, int len, JCORE_OUT JCore::String& hex);
};


