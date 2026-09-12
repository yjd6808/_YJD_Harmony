/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * ObjectPoolTLSIndexed<T> 단독 벤치 (Dummy64)
 *
 * - Single: 동일 스레드 Pop/Push 핑퐁. 핫 패스(락·원자 연산 없음) 순수 측정.
 * - Cross: A 생성 → B 반납(inbox CAS) → A 회수. foreign 경로 전체 시간 (생성+반납+회수 포함).
 * - Drain: B가 N개를 inbox에 쌓고 A가 통째로 회수. Exchange 1회 + 체인 연결.
 */

#include "jc/Pool/ObjectPoolTLSIndexed.h"

#if ObjectPoolTLSIndexedBench == ON

struct Dummy64 : TlsPoolNode
{
	char pad_[JC_CACHE_ALIGN_SIZE - sizeof(TlsPoolNode)];
};
using TDummyPool = ObjectPoolTLSIndexed<Dummy64>;

static void BM_TLSIndexedSingle(State& state) {
	const size_t count = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < count; ++i) {
			Dummy64* pObj = TDummyPool::Pop();
			DoNotOptimize(pObj);
			TDummyPool::Push(pObj);
			ClobberMemory();
		}
	}
}

static void BM_TLSIndexedCross(State& state) {
	const size_t count = state.range();
	vector<Dummy64*> box(count, nullptr);
	for (auto _ : state) {
		for (size_t i = 0; i < count; ++i) {
			box[i] = TDummyPool::Pop();
		}
		thread worker([&box, count] {
			for (size_t i = 0; i < count; ++i) {
				TDummyPool::Push(box[i]);
			}
		});
		worker.join();
		for (size_t i = 0; i < count; ++i) {
			box[i] = TDummyPool::Pop();
		}
		for (size_t i = 0; i < count; ++i) {
			TDummyPool::Push(box[i]);
		}
	}
}

static void BM_TLSIndexedDrain(State& state) {
	const size_t count = state.range();
	vector<Dummy64*> box(count, nullptr);
	for (auto _ : state) {
		for (size_t i = 0; i < count; ++i) {
			box[i] = TDummyPool::Pop();
		}
		thread worker([&box, count] {
			for (size_t i = 0; i < count; ++i) {
				TDummyPool::Push(box[i]);
			}
		});
		worker.join();
		for (size_t i = 0; i < count; ++i) {	// inbox Exchange 1회 + 체인 연결 후 순차 Pop
			TDummyPool::Push(TDummyPool::Pop());
		}
	}
}

BENCHMARK(BM_TLSIndexedSingle)->Iterations(10)->Unit(TimeUnit::kMicrosecond)->Arg(1000000);
BENCHMARK(BM_TLSIndexedCross)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(10000);
BENCHMARK(BM_TLSIndexedDrain)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(10000);

#endif

/*
[측정 결과] Release/x64, 8논리 (2026-09-06)
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_TLSIndexedSingle/1000000/iterations:10       5880 us         6250 us           10
BM_TLSIndexedCross/10000/iterations:5           1.03 ms        0.000 ms            5
BM_TLSIndexedDrain/10000/iterations:5          0.727 ms        0.000 ms            5

- Single: 5880us/1e6회 Pop+Push 쌍 = 5.9ns/쌍, 약 2.9ns/회. §10.1 기대(~2ns) 부합.
- Cross: 1.03ms/1e4개 = 생성+반납+회수+정리 4만 연산에 스레드 생성/join 포함, 약 26ns/회. 기대(20~40ns) 부합.
- Drain: 0.73ms/1e4개, 약 24ns/회. 기대 부합.
- Single은 DoNotOptimize+ClobberMemory 없이는 컴파일러가 핑퐁 루프를 소거한다 (0.6ns/쌍으로 관측). 필수.
*/
