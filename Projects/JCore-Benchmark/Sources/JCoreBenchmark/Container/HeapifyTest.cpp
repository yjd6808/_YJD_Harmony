/*
 * 작성자: 윤정도
 * 생성일: 11/17/2023 5:35:22 PM
 * =====================
 * MakeHeap의 SiftUp방식과 SiftDown방식 성능비교
 */



#include <JCoreBenchmark/CoreBenchmark.h>
#include <JCore/Container/Arrays.h>

#if HeapifyTest == ON

std::random_device rd;
std::mt19937 g(rd());
std::vector<int> v;

static void BM_SetUp(const State& state) {
	int64_t arg = state.range();
	v.clear();
	for (int i = 0; i < arg; ++i) {
		v.push_back(i);
	}
	
}


static void BM_MakeHeapSiftDown(State& state) {
	for (auto _ : state) {
		state.PauseTiming();
		ranges::shuffle(v, g);
		state.ResumeTiming();
		Arrays::MakeHeapSiftDown(v.data(), v.size(), NaturalOrder{});
	}
}

static void BM_MakeHeapSiftUp(State& state) {
	for (auto _ : state) {
		state.PauseTiming();
		ranges::shuffle(v, g);
		state.ResumeTiming();
		Arrays::MakeHeapSiftUp(v.data(), v.size(), NaturalOrder{});
	}
}

#if DebugMode
BENCHMARK(BM_MakeHeapSiftDown)->Iterations(10)->Setup(BM_SetUp)->Unit(TimeUnit::kSecond)->Arg(1 << 24);
BENCHMARK(BM_MakeHeapSiftUp)->Iterations(10)->Setup(BM_SetUp)->Unit(TimeUnit::kSecond)->Arg(1 << 24);
#else
BENCHMARK(BM_MakeHeapSiftDown)->Repetitions(5)->Iterations(10)->Setup(BM_SetUp)->Unit(TimeUnit::kSecond)->Arg(1 << 24);
BENCHMARK(BM_MakeHeapSiftUp)->Repetitions(5)->Iterations(10)->Setup(BM_SetUp)->Unit(TimeUnit::kSecond)->Arg(1 << 24);
#endif



#endif