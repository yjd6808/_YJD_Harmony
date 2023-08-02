/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 10:15:25 PM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "PacketViewer.h"

#include <JCore/Digit.h>
#include <JCore/Primitives/String.h>

USING_NS_JC;

void PacketViewer::View(char* data, int len, int cmdCount) {
	String szHex{ 1024 };

	if (len > szHex.Capacity() - 1) {
		_LogWarn_("패킷 뷰 실패(출력 불가능한 크기)");
		return;
	}
	Hex(data, len, szHex);
	Console::WriteLine("[패킷 뷰]\n패킷 크기: %d\n커맨드 수: %d\n헥스\n%s", len, cmdCount, szHex.Source());
}

void PacketViewer::View(JNetwork::ISendPacket* packet) {
	WSABUF wsaBuf = packet->GetWSABuf();
	View(wsaBuf.buf, wsaBuf.len, packet->GetCommandCount());
}

void PacketViewer::View(JNetwork::IRecvPacket* packet) {
	View(reinterpret_cast<char*>(packet), packet->GetPacketLength() + JNetwork::PacketHeaderSize_v, packet->GetCommandCount());
}

void PacketViewer::View(JNetwork::ICommand* cmd) {
	String szHex{ 1024 };

	if (cmd->CmdLen > szHex.Capacity() - 1) {
		_LogWarn_("커맨드 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(reinterpret_cast<char*>(cmd), cmd->CmdLen, szHex);
	Console::WriteLine("[커맨드 뷰]\n커맨드 타입:%d\n커맨드: %d\n크기: %d\n헥스\n%s", cmd->Type, cmd->Cmd, cmd->CmdLen, szHex.Source());
}

void PacketViewer::Hex(char* data, int len, JCORE_OUT String& hex) {
	hex.SetLength(0);
	char szByte[4]{ '0', '0', ' ', NULL };
	for (int i = 0; i < len; ++i) {
		szByte[0] = Digit::HexChar[Byte(data[i]) / 0x10];
		szByte[1] = Digit::HexChar[Byte(data[i]) % 0x10];
		hex += szByte;

		if (i != 0 && i % 10 == 9) {
			hex += '\n';
		}
	}
}
