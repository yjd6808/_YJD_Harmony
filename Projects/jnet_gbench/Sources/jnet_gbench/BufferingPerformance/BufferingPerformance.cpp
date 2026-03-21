/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM (크리스마스 2일전 토요일날 작성함)
 * =====================
 * 소켓 옵션의 송/수신 버퍼링을 켜고 끌때의 성능 차이를 비교한다.
 *
 * BM_BufferingPerformance/10/10000/0/0/iterations:5       2105 ms        0.000 ms            5
 * 송신 버퍼링 : O
 * 수신 버퍼링 : O
 * BM_BufferingPerformance/10/10000/0/1/iterations:5       2031 ms        0.000 ms            5
 * 송신 버퍼링 : O
 * 수신 버퍼링 : X
 * BM_BufferingPerformance/10/10000/1/0/iterations:5       1776 ms        0.000 ms            5
 * 송신 버퍼링 : X
 * 수신 버퍼링 : O
 * BM_BufferingPerformance/10/10000/1/1/iterations:5       1783 ms         3.12 ms            5
 * 송신 버퍼링 : X
 * 수신 버퍼링 : X
 *
 *
 * [결론]
 * 오버랩 송신이더라도 송신 버퍼 크기를 0으로 설정하면 성능이 향상된다.
 * 수신 버퍼크기는 영향을 주지 않는다.
 */



#include "BufferingPerformance_ClientSide.h"
#include "BufferingPerformance_ServerSide.h"
#include "BufferingPerformance_Common.h"

USING_NS_BUFFERING_PERFORMANCE;

#if BufferingPerformanceTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
void BM_BufferingPerformance(State& _state)
{
	TestClientCount = _state.range(0);
	TestSendCount = _state.range(1);
	DisableSendBuffering = _state.range(2);
	DisableRecvBuffering = _state.range(3);
	char label[256];
	sprintf_s(label, 256, "\n%s\n%s", DisableSendBuffering ? "송신 버퍼링 : X" : "송신 버퍼링 : O", DisableRecvBuffering ? "수신 버퍼링 : X" : "수신 버퍼링 : O");
	_state.SetLabel(label);

	
	for (auto iteration : _state)
	{
		_state.PauseTiming();
		ServerSide::Initialize();
		ClientSide::Initialize();
		_state.ResumeTiming();

		ClientSide::StartTest();
		TestFinished.Wait();

		_state.PauseTiming();
		ServerSide::Finalize();
		ClientSide::Finalize();
		_state.ResumeTiming();
	}
	
}

BENCHMARK(BM_BufferingPerformance)->Unit(benchmark::TimeUnit::kMillisecond)
	->Iterations(5)
	->Args({10, 10'000, 0, 0})
	->Args({10, 10'000, 0, 1})
	->Args({10, 10'000, 1, 0})
	->Args({10, 10'000, 1, 1});

#endif
