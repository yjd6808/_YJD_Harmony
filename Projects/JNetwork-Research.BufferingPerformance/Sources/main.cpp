/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM (크리스마스 2일전 토요일날 작성함)
 * =====================
 * 소켓 옵션의 송/수신 버퍼링을 켜고 끌때의 성능 차이를 비교하는 용도의 프로젝트이다.
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Config.h>
#include <JNetwork/Packet/PacketParser.h>

#include <JCore/Threading/Thread.h>

#include "ClientSide.h"
#include "ServerSide.h"
#include "Common.h"


USING_NS_JC;
USING_NS_JNET;

int main() {
	Winsock::Initialize(2, 2);
	InitializeJCore();
	DefaultParserType = PacketParser::Command;

	for (int j = 0; j < 4; ++j) {
		if (j == 0) {
			DisableSendBuffering = false;
			DisableRecvBuffering = false;
		} else if (j == 1) {
			DisableSendBuffering = false;
			DisableRecvBuffering = true;
		} else if (j == 2) {
			DisableSendBuffering = true;
			DisableRecvBuffering = false;
		} else if (j == 3) {
			DisableSendBuffering = true;
			DisableRecvBuffering = true;
		}

		Console::WriteLine("%s", DisableSendBuffering ? "송신 버퍼링 : X" : "송신 버퍼링 : O");
		Console::WriteLine("%s", DisableRecvBuffering ? "수신 버퍼링 : X" : "수신 버퍼링 : O");
		for (int i = 0; i < 5; ++i) {
			ServerSide::Initialize();
			ClientSide::Initialize();
			while (!TestFinished_v) { Thread::Sleep(100); }
			ServerSide::Finalize();
			ClientSide::Finalize();
			TestFinished_v = false;
		}
		Console::WriteLine("================================================");
	}

	FinalizeJCore();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
}