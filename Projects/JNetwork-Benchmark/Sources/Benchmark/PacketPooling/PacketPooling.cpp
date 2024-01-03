/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM (크리스마스 2일전 토요일날 작성함)
 * =====================
 * 소켓 옵션의 송/수신 버퍼링을 켜고 끌때의 성능 차이를 비교한다.
 *
 * BM_PacketPooling / 10 / 100000 / 1 / 0 / iterations:5      17698 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 100000
 * 풀링 : O
 * 풀링 효율을 네트워킹동안 측정
 * BM_PacketPooling / 10 / 100000 / 0 / 0 / iterations : 5      17740 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 100000
 * 풀링 : X
 * 풀링 효율을 네트워킹동안 측정
 * BM_PacketPooling / 10 / 100000 / 1 / 1 / iterations : 5       8855 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 100000
 * 풀링 : O
 * 풀링 효율을 송신 오버랩 중에서만 측정
 * BM_PacketPooling / 10 / 100000 / 0 / 1 / iterations : 5       8869 ms        0.000 ms            5
 * 테스트 클라이언트 수 : 10
 * 테스트 송신 횟수 : 100000
 * 풀링 : X
 * 풀링 효율을 송신 오버랩 중에서만 측정
 *
 * [결론]
 * 풀링 하든지 안하든지 실제 트래픽에서는 비중이 거의 없다.
 */


#include <Benchmark/Core.h>

#include "PacketPooling_ClientSide.h"
#include "PacketPooling_ServerSide.h"
#include "PacketPooling_Common.h"

USING_NS_PACKET_POOLING;

#if PacketPoolingTest == ON

void BM_PacketPooling(State& state) {
	TestClientCount = state.range(0);
	TestSendCount = state.range(1);
	UsePooling = state.range(2);
	Mode = (TestMode)state.range(3);

	char szLabel[1024];
	sprintf_s(szLabel, 1024, "\n테스트 클라이언트 수 : %d\n테스트 송신 횟수 : %d\n%s\n%s", TestClientCount, TestSendCount, UsePooling ? "풀링 : O" : " 풀링 : X", Mode == TestMode::OnSending ? "풀링 효율을 송신 오버랩 중에서만 측정" : "풀링 효율을 네트워킹동안 측정");
	state.SetLabel(szLabel);


	for (auto _ : state) {
		state.PauseTiming();
		ServerSide::Initialize();
		ClientSide::Initialize();
		state.ResumeTiming();

		ClientSide::StartTest();
		TestFinished.Wait();


		state.PauseTiming();
		// 서버에서 송신 트래픽 모두 수신할때까지 기다린다.
		if (Mode == TestMode::OnSending)
			TestFinished.Wait();

		ServerSide::Finalize();
		ClientSide::Finalize();
		state.ResumeTiming();
	}
	
}

BENCHMARK(BM_PacketPooling)->Unit(benchmark::TimeUnit::kMillisecond)
	->Iterations(5)											// 테스트 횟수
	->Args({ 10, 100'000, 1, int(TestMode::OnNetworking) })	// 테스트 클라이언트 수, 테스트 송신 횟수, 패킷 풀링 여부
	->Args({ 10, 100'000, 0, int(TestMode::OnNetworking) })
	->Args({ 10, 100'000, 1, int(TestMode::OnSending)	})
	->Args({ 10, 100'000, 0, int(TestMode::OnSending)	});
#endif
