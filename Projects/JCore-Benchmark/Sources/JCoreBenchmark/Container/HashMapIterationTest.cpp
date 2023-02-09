/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 12:06:58 PM
 * =====================
 *
 */

#include <JCoreBenchmark/CoreBenchmark.h>
#include <JCore/Container/Vector.h>
#include <JCore/Container/HashMap.h>

#if HashMapIterationTest == ON

#define UseStd OFF

	#if UseStd == ON
		std::vector<int> v;
		std::unordered_map<int, int> h;
	#else
		JCore::Vector<int> v;
		JCore::HashMap<int, int> h;
	#endif

#endif


#if UseStd == ON

static void BM_HashMapIteration(State& state) {
	int64_t dataSize = state.range();
	h.clear();
	for (int i = 0; i < dataSize; ++i) {
		h.insert(make_pair(i, i + 1));
	}

	volatile int a = 0;
	for (auto _ : state) {
		for (auto& k : h) {
			a = k.second;
		}
	}
}

static void BM_VectorIteration(State& state) {
	int64_t dataSize = state.range();
	v.clear();
	for (int i = 0; i < dataSize; ++i) {
		v.push_back(i);
	}

	volatile int a = 0;
	for (auto _ : state) {
		for (int i = 0; i < v.size(); ++i) {
			a = v[i];
		}
	}
}


#else

static void BM_HashMapIteration(State& state) {
	int64_t dataSize = state.range();
	h.Clear();
	for (int i = 0; i < dataSize; ++i) {
		h.Insert(i, i + 1);
	}

	volatile int a = 0;
	for (auto _ : state) {
		auto it = h.Begin();
		while (it->HasNext()) {
			a = it->Next().Value;
		}
	}
}

static void BM_VectorIteration(State& state) {
	int64_t dataSize = state.range();
	v.Clear();
	for (int i = 0; i < dataSize; ++i) {
		v.PushBack(i);
	}

	volatile int a = 0;
	for (auto _ : state) {
		for (int i = 0; i < v.Size(); ++i) {
			a = v[i];
		}
	}
}





#endif

static void BM_VectorAddDelete(State& state) {
	int64_t dataSize = state.range();
	v.Clear();
	for (int i = 0; i < dataSize; ++i) {
		v.PushBack(i);
	}

	int Legend = 300;
	for (auto _ : state) {
		for (int i = 0; i < 1000; ++i) {
			v.PushBack(++Legend);
			v.RemoveAt(100);
		}
	}
}

static void BM_HashMapAddDelete(State& state) {
	int64_t dataSize = state.range();
	h.Clear();
	for (int i = 0; i < dataSize; ++i) {
		h.Insert(i, i + 1);
	}

	int Legend = 300;
	for (auto _ : state) {
		for (int i = 0; i < 1000; ++i) {
			h.Insert(++Legend, Legend);
			h.Remove(Legend);
		}
	}
}

BENCHMARK(BM_HashMapIteration)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(1 << 24);
BENCHMARK(BM_VectorIteration)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(1 << 24);
BENCHMARK(BM_VectorAddDelete)->Unit(TimeUnit::kMillisecond)->Arg(200);
BENCHMARK(BM_HashMapAddDelete)->Unit(TimeUnit::kMillisecond)->Arg(200);


/*
// ------------------------------------------------------------------------------------
// 표준 컨테이너 성능 비교
// ------------------------------------------------------------------------------------
STD HashMap, Vector 테스트
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapIteration/16777216/iterations:5        680 ms          681 ms            5
BM_VectorIteration/16777216/iterations:5        9.43 ms         9.38 ms            5







// ------------------------------------------------------------------------------------
// 내가 만든 컨테이너 성능 비교
// ------------------------------------------------------------------------------------
JCore HashMap, Vector 테스트 [수정전]
throw 던지는 함수 호출땜에 성능 저하 있음

------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapIteration/16777216/iterations:5       1248 ms         1247 ms            5
BM_VectorIteration/16777216/iterations:5         133 ms          131 ms            5


디버깅 시에만 throw 하도록 함
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapIteration/16777216/iterations:5       1252 ms         1253 ms            5
BM_VectorIteration/16777216/iterations:5        6.50 ms         6.25 ms            5

*/