/*
 * 작성자: 윤정도
 * 생성일: 12/27/2023 3:25:50 AM
 * =====================
 * Nagle 알고리즘을 켜고 끌때 송신 버퍼링 유무와 어떤 관계가 있는지 확인
 * 
 * [테스트 결과]
 * Heavy Traffic 확인 결과(Overlapped)
 * 송신 버퍼링 끄기 + Nagle O: 평균 수행시간: 0.918
 * 송신 버퍼링 끄기 + Nagle X: 평균 수행시간: 0.922
 * 송신 버퍼링 켜기 + Nagle O: 평균 수행시간: 1.004
 * 송신 버퍼링 켜기 + Nagle X: 평균 수행시간: 1.040
 *
 * Middle Traffic 확인 결과(Blocking + Send)
 * 송신 버퍼링 끄기 + Nagle O: 평균 수행시간: 4.637
 * 송신 버퍼링 끄기 + Nagle X: 평균 수행시간: 4.635
 * 송신 버퍼링 켜기 + Nagle O: 평균 수행시간: 0.005
 * 송신 버퍼링 켜기 + Nagle X: 평균 수행시간: 0.005
 *
 * Middle Traffic 확인 결과(Overlapped)
 * 송신 버퍼링 끄기 + Nagle O: 평균 수행시간: 0.007
 * 송신 버퍼링 끄기 + Nagle X: 평균 수행시간: 0.005
 * 송신 버퍼링 켜기 + Nagle O: 평균 수행시간: 0.005
 * 송신 버퍼링 켜기 + Nagle X: 평균 수행시간: 0.005
 *
 *
 * Light Traffic 와이어샤크 확인 결과
 * 클라이언트 1, 송신횟수 10번, 패킷크기 4바이트, Overlapped송신
 *
 *
 * [결과]
 * 송신 버퍼링을 끄고 켤때와 Nagle의 조합시 Overlapped 송신시에서는 특별한 변화를 관측하지 못했다.
 * Blocking + Send 송신 결과를 통해 송신 버퍼가 확실히 존재한다는 사실을 인지할 수 있었다.
 *  -> 빠르게 송/수신을 주고받기때문에 그런것으로 보인다. 메시지를 천천히 주고받을 때 일어나는 경우에 대해서 관찰을 해봐야겠다.
 *
 * 이 두가지 사실을 조합하면 Overlapped 송신시는 WSABUF 그 자체의 데이터를 따로 버퍼링하지 않고 전달한다는 사실을 알 수 있다.
 * 뿐만아니라 단순 Send 함수 호출시에는 따로 커널의 소켓 버퍼를 사용한다는 사실을 알 수 있다.
 * 이 때문에 송신버퍼링을 끈 상태로 Send 함수를 호출시 내부 소켓버퍼가 없기때문에 Send 함수의 인자로 전달한 버퍼의 데이터가 모두 보내진 이후에
 * 이어서 Send 호출이 가능했다.
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
#if ASYNC_SENDING
	NonblokingSocket = true;
#else
	NonblokingSocket = false;
#endif
	DontEcho_v = true;

	Vector<double> times;

	for (int j = 0; j < 4; ++j) {
		bool nagle = false;
		if (j == 0 || j == 1) {
			DisableSendBuffering = true;
		} else if (j == 2 || j == 3) {
			DisableSendBuffering = false;
		}

		if (j == 0 || j == 2) {
			nagle = true;
		}

		Console::WriteLine("%s / %s", DisableSendBuffering ? "송신 버퍼링 : X" : "송신 버퍼링 : O", nagle ? "Nagle : O" : "Nagle : X");

		for (int i = 0; i < TEST_COUNT; ++i) {
			ServerSide::Initialize();
			ClientSide::Initialize(nagle);
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