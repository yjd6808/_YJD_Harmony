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



#include "PacketPooling_ClientSide.h"
#include "PacketPooling_ServerSide.h"
#include "PacketPooling_Common.h"
#include "jc/Utils/ProgressNotifier.h"

USING_NS_PACKET_POOLING;

class PacketPoolingProgressListener : public jc::IProgressListener
{
public:
	void OnProgressed(int _step, int _maxStep) override
	{
		const float percent = _maxStep > 0
			? (float(_step) / float(_maxStep)) * 100.0f
			: 0.0f;


		Console::WriteLine("[PacketPooling] Progress: %d / %d (%.1f%%)", _step, _maxStep, percent);
	}

	void OnFinished(int _step, int _maxStep) override
	{
		Console::WriteLine("[PacketPooling] Finished: %d / %d (100%%)", _step, _maxStep);
	}
};

#if PacketPoolingTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
void BM_PacketPooling(State& _state)
{
    TestClientCount = _state.range(0);
    TestSendCount = _state.range(1);
    UsePooling = _state.range(2);
    Mode = (TestMode)_state.range(3);

    char label[1024];
    sprintf_s(label, 1024, "\n테스트 클라이언트 수 : %d\n테스트 송신 횟수 : %d\n%s\n%s", TestClientCount, TestSendCount, UsePooling ? "풀링 : O" : " 풀링 : X", Mode == TestMode::OnSending ? "풀링 효율을 송신 오버랩 중에서만 측정" : "풀링 효율을 네트워킹동안 측정");
    _state.SetLabel(label);
	
	 const int maxStep = static_cast<int>(_state.max_iterations);
	 jc::PercentProgressNotifier progressNotifier(maxStep, 1.0f);	// 5% 단위로 알림
	 auto* pListener = dbg_new PacketPoolingProgressListener();
	 progressNotifier.SetListener(pListener, true);
	 int i = 0;

    for (auto value : _state)
    {
        _state.PauseTiming();
        ServerSide::Initialize();
        ClientSide::Initialize();
        _state.ResumeTiming();

        ClientSide::StartTest();

		{
			LockGuard lg(TestLock);
			TestCondVar.Wait(lg, []() {
				return TestStep >= 1;
			});
		}

        _state.PauseTiming();
        // 서버에서 송신 트래픽 모두 수신할때까지 기다린다.
		if (Mode == TestMode::OnSending)
		{
			{
				LockGuard lg(TestLock);
				TestCondVar.Wait(lg, []() {
					return TestStep >= 2;
				});
			}
		}

        ServerSide::Finalize();
        ClientSide::Finalize();
		progressNotifier.Progress(++i);
        _state.ResumeTiming();
		TestStep = 0;
    }


}

BENCHMARK(BM_PacketPooling)->Unit(benchmark::TimeUnit::kMillisecond)
    ->Iterations(1)                                        // 테스트 횟수
    ->Args({ 64, 1'000, 1, int(TestMode::OnNetworking) })   // 테스트 클라이언트 수, 테스트 송신 횟수, 패킷 풀링 여부
    ->Args({ 64, 1'000, 0, int(TestMode::OnNetworking) })
    ->Args({ 64, 1'000, 1, int(TestMode::OnSending) })
    ->Args({ 64, 1'000, 0, int(TestMode::OnSending) });
#endif
