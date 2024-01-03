/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM (크리스마스 2일전 토요일날 작성함)
 * =====================
 * 소켓 옵션의 송/수신 버퍼링을 켜고 끌때의 성능 차이를 비교한다.
 *
 * [Heavy 트래픽]
 * BM_BufferingWithNagle/10/10000/1/1/1/iterations:5        952 ms         6.25 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 10000
 * 송신 버퍼링 : X
 * Nagle : O
 * 비동기 : O
 * BM_BufferingWithNagle/10/10000/1/0/1/iterations:5       1265 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 10000
 * 송신 버퍼링 : X
 * Nagle : X
 * 비동기 : O
 * BM_BufferingWithNagle/10/10000/0/1/1/iterations:5       1147 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 10000
 * 송신 버퍼링 : O
 * Nagle : O
 * 비동기 : O
 * BM_BufferingWithNagle/10/10000/0/0/1/iterations:5       1105 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 10000
 * 송신 버퍼링 : O
 * Nagle : X
 * 비동기 : O
 *
 *
 *
 *
 *
 * [Middle 트래픽]
 * BM_BufferingWithNagle/1/100/1/1/1/iterations:5       4.92 ms         3.12 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : X
 * Nagle : O
 * 비동기 : O
 * BM_BufferingWithNagle/1/100/1/0/1/iterations:5       9.39 ms         3.12 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : X
 * Nagle : X
 * 비동기 : O
 * BM_BufferingWithNagle/1/100/0/1/1/iterations:5       9.71 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : O
 * Nagle : O
 * 비동기 : O
 * BM_BufferingWithNagle/1/100/0/0/1/iterations:5       7.96 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : O
 * Nagle : X
 * 비동기 : O
 * BM_BufferingWithNagle/1/100/1/1/0/iterations:5       4693 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : X
 * Nagle : O
 * 비동기 : X
 * BM_BufferingWithNagle/1/100/1/0/0/iterations:5       4670 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : X
 * Nagle : X
 * 비동기 : X
 * BM_BufferingWithNagle/1/100/0/1/0/iterations:5       11.3 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : O
 * Nagle : O
 * 비동기 : X
 * BM_BufferingWithNagle/1/100/0/0/0/iterations:5       11.1 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 1
 * 테스트 송신 횟수 : 100
 * 송신 버퍼링 : O
 * Nagle : X
 * 비동기 : X
 *
 *
 *
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


#include <Benchmark/Core.h>

#include "BufferingWithNagle_ClientSide.h"
#include "BufferingWithNagle_ServerSide.h"
#include "BufferingWithNagle_Common.h"

USING_NS_BUFFERING_WITH_NAGLE;

#if BufferingWithNagleTest == ON

#define HEAVY		0
#define MIDDLE		1
#define LIGHT		2

// 트래픽 선택
#define TRAFFIC		MIDDLE

void BM_BufferingWithNagle(State& state) {
	TestClientCount = state.range(0);
	TestSendCount = state.range(1);
	DisableSendBuffering = state.range(2);
	const bool bNagle = state.range(3);
	AsyncSending = state.range(4);

	char szLabel[1024];
	sprintf_s(szLabel, 1024, "\n테스트 클라이언트 수 : %d\n테스트 송신 횟수 : %d\n%s\n%s\n%s", TestClientCount, TestSendCount, DisableSendBuffering ? "송신 버퍼링 : X" : "송신 버퍼링 : O", bNagle ? "Nagle : O" : "Nagle : X", AsyncSending ? "비동기 : O" : "비동기 : X");
	state.SetLabel(szLabel);


	for (auto _ : state) {
		state.PauseTiming();
		ServerSide::Initialize();
		ClientSide::Initialize(bNagle);
		state.ResumeTiming();

		ClientSide::StartTest();
		TestFinished.Wait();

		state.PauseTiming();
		ServerSide::Finalize();
		ClientSide::Finalize();
		state.ResumeTiming();
	}
	
}

BENCHMARK(BM_BufferingWithNagle)->Unit(benchmark::TimeUnit::kMillisecond)
	#if TRAFFIC == HEAVY
		->Iterations(5)					// 테스트 횟수
		->Args({ 10, 10'000, 1, 1, 1 })	// 테스트 클라이언트 수, 테스트 송신 횟수, 송신 버퍼링 여부, Nagle 여부, Async 송신 여부
		->Args({ 10, 10'000, 1, 0, 1 })
		->Args({ 10, 10'000, 0, 1, 1 })
		->Args({ 10, 10'000, 0, 0, 1 });
	#elif TRAFFIC == MIDDLE
		->Iterations(5)
		->Args({ 1, 100, 1, 1, 1 })
		->Args({ 1, 100, 1, 0, 1 })
		->Args({ 1, 100, 0, 1, 1 })
		->Args({ 1, 100, 0, 0, 1 })
		->Args({ 1, 100, 1, 1, 0 })
		->Args({ 1, 100, 1, 0, 0 })
		->Args({ 1, 100, 0, 1, 0 })
		->Args({ 1, 100, 0, 0, 0 });
	#elif TRAFFIC == LIGHT
	#endif

#endif
