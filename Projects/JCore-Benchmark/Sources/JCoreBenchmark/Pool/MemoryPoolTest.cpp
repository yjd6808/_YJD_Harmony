/*
 * 작성자: 윤정도
 * 생성일: 12/2/2023 3:16:42 PM
 * =====================
 *
 */

#include <JCoreBenchmark/CoreBenchmark.h>
#include <JCore/Pool/IndexedMemoryPool.h>
#include <JCore/Pool/BinarySearchMemoryPool.h>

#if IndexMemoryPoolTest == ON


struct TestStruct
{
	char _[999];
};

constexpr int DataCount = 1024 * 1024;
constexpr int DataSize = sizeof(TestStruct);
vector<TestStruct*> Box(DataCount, nullptr);



static void BM_IndexedPoolDynamic(State& state) {
	IndexedMemoryPool pool
	(
		HashMap<int, int>{ { 1024, DataCount } }	// 1GB
	);

	const size_t max = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < max; ++i) {
			int _;
			TestStruct* s = new (pool.DynamicPop(DataSize, _)) TestStruct{};
			Box[i] = s;
		}

		for (size_t i = 0; i < max; ++i) {
			pool.DynamicPush(Box[i], DataSize);
		}
	}
}

static void BM_BinaryPoolDynamic(State& state) {
	BinarySearchMemoryPool pool
	(
		HashMap<int, int>{ { 1024, DataCount } }	// 1GB
	);

	const size_t max = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < max; ++i) {
			int _;
			TestStruct* s = new (pool.DynamicPop(DataSize, _)) TestStruct{};
			Box[i] = s;
		}

		for (size_t i = 0; i < max; ++i) {
			pool.DynamicPush(Box[i], DataSize);
		}
	}
}


static void BM_IndexedPoolStatic(State& state) {
	IndexedMemoryPool pool
	(
		HashMap<int, int>{ { 1024, DataCount } }	// 1GB
	);

	const size_t max = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < max; ++i) {
			TestStruct* s = new (pool.StaticPop<DataSize>()) TestStruct{};
			Box[i] = s;
		}

		for (size_t i = 0; i < max; ++i) {
			pool.StaticPush<DataSize>(Box[i]);
		}
	}
}

static void BM_BinaryPoolStatic(State& state) {
	BinarySearchMemoryPool pool
	(
		HashMap<int, int>{ { 1024, DataCount } }	// 1GB
	);

	const size_t max = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < max; ++i) {
			TestStruct* s = new (pool.StaticPop<DataSize>()) TestStruct{};
			Box[i] = s;
		}

		for (size_t i = 0; i < max; ++i) {
			pool.StaticPush<DataSize>(Box[i]);
		}
	}
}

static void BM_NoPool(State& state) {
	const size_t max = state.range();
	for (auto _ : state) {
		for (size_t i = 0; i < max; ++i) {
			Box[i] = new TestStruct{};
		}

		for (size_t i = 0; i < max; ++i) {
			delete Box[i];
		}
	}
}


BENCHMARK(BM_BinaryPoolDynamic)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(DataCount);
BENCHMARK(BM_IndexedPoolDynamic)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(DataCount);
BENCHMARK(BM_BinaryPoolStatic)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(DataCount);
BENCHMARK(BM_IndexedPoolStatic)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(DataCount);
BENCHMARK(BM_NoPool)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(DataCount);


#endif

/*
999바이트 객체 기준으로 1024 * 1024회씩 999바이트 객체 생성/소멸 테스트
-------------------------------------------------------------------------------------
Benchmark                                           Time             CPU   Iterations
-------------------------------------------------------------------------------------
BM_BinaryPoolDynamic/1048576/iterations:5         191 ms          184 ms            5
BM_IndexedPoolDynamic/1048576/iterations:5        169 ms          169 ms            5
BM_BinaryPoolStatic/1048576/iterations:5          170 ms          169 ms            5
BM_IndexedPoolStatic/1048576/iterations:5         176 ms          175 ms            5
BM_NoPool/1048576/iterations:5                    855 ms          856 ms            5

*/