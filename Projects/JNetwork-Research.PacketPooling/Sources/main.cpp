/*
 * 작성자: 윤정도
 * 생성일: 12/27/2023 3:25:50 AM
 * =====================
 * Nagle 알고리즘을 켜고 끌때 송신 버퍼링 유무와 어떤 관계가 있는지 확인
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
	// InitializeNetLogger();

	DefaultParserType = PacketParser::Command;
	DefaultRecvBufferSize = 280'000;
	DefaultSendBufferSize = 280'000;

	Vector<double> times;

	for (int j = 0; j < 2; ++j) {
		if (j == 0) {
			UsePooling_v = true;
		} else if (j == 1) {
			UsePooling_v = false;
		}

		Console::WriteLine("%s", UsePooling_v ? "풀링 : O" : " 풀링 : X");

		for (int i = 0; i < TEST_COUNT; ++i) {
			ServerSide::Initialize();
			ClientSide::Initialize();
			while (!TestFinished_v) { Thread::Sleep(1000); }
			ServerSide::Finalize();
			ClientSide::Finalize();
			TestFinished_v = false;
			times.PushBack(LastTime_v.GetTotalSeconds());
		}

		double s = 0.0;
		for (int i = 0; i < times.Size(); ++i) {
			s += times[i];
		}
		s /= times.Size();
		times.Clear();

		Console::WriteLine("평균 수행시간: %.3lf", s);
		Console::WriteLine("================================================");
	}

	// FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
}