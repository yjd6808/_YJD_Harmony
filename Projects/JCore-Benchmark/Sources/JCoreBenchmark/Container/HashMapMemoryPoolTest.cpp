/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 5:29:57 AM
 * =====================
 * 반복자 순회 및 반복자 생성 퍼포먼스 테스트
 *
 * 1. 반복자 순회
 * 표준 해쉬맵과 성능 거의 동일
 *
 * 2. 잦은 반복자 생성
 *
 */



#include <JCoreBenchmark/CoreBenchmark.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Pool/BinarySearchMemoryPool.h>


#if HashMapMemoryPoolTest == ON


static BinarySearchMemoryPool Pool{};

auto lmb = []() {
	Pool.Initialize({
		{ 16, 5000 }
	});
	return true;
}();

class MyAlloc
{
public:
	// 명시적으로 사이즈 요청해서 반환하는 2가지 기능을 구현할 것
	template <typename T>
	static auto AllocateStatic() {	// Static
		return (RemovePointer_t<T>*)Pool.StaticPop<sizeof(T)>();
	}

	template <typename T = void*>	// 명시하지 않을 경우 void* 반환
	static auto AllocateDynamic(int requestSize, int& realAllocatedSize) {	// Dynamic
		return (RemovePointer_t<T>*)Pool.DynamicPop(requestSize, realAllocatedSize);
	}

	template <typename T, typename... Args>
	static auto AllocateInitStatic(Args&&... args) {	// Static
		auto pRet = (RemovePointer_t<T>*)Pool.StaticPop<sizeof(T)>();
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T = void*, typename... Args>	// 명시하지 않을 경우 void* 반환
	static auto AllocateInitDynamic(int requestSize, int& realAllocatedSize, Args&&... args) {	// Dynamic
		auto pRet = (RemovePointer_t<T>*)Pool.DynamicPop(requestSize, realAllocatedSize);
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T>
	static void DeallocateStatic(void* del) {
		Pool.StaticPush<sizeof(T)>(del);

		
	}

	static void DeallocateDynamic(void* del, int size) {
		Pool.DynamicPush(del, size);
	}
};


// #define StdHashMap

#ifdef StdHashMap
	std::unordered_map<int, int> hashMap;
#else
	#define UseAllocator ON

	#if UseAllocator == OFF
		HashMap<int, int> hashMap{ 2'000'000 };
	#else
		HashMap<int, int, MyAlloc> hashMap{ 100'000 };
	#endif
#endif



static void BM_HashMapIteratorVisit(State& state) {
	int64_t dataSize = state.range();
	std::vector<int> v;
#ifdef StdHashMap
	hashMap.clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.insert(make_pair(i, i + 1));
	}
#else
	hashMap.Clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.Insert(i, i + 1);
	}
#endif


	int k = 0;
	for (auto _ : state) {
#ifdef StdHashMap
		for (auto it = hashMap.begin(); it != hashMap.end(); ++it) {
			k = hashMap[it->first];
			state.PauseTiming();
			v.push_back(k);
			state.ResumeTiming();
		}
#else
		auto it = hashMap.Begin();
		while (it->HasNext()) {
			k = hashMap[it->Next().Key];
			state.PauseTiming();
			v.push_back(k);
			state.ResumeTiming();
		}
#endif
	}
#ifdef StdHashMap
	DebugAssertMsg(v.size() == hashMap.size(), "데이터가 이상합니다.");
#else
	DebugAssertMsg(v.size() == hashMap.Size(), "데이터가 이상합니다.");
#endif
}

static void BM_HashMapIteratorCreation(State& state) {
	int64_t dataSize = state.range();
	int64_t iterationCount = state.range(1);
	std::vector<int> v;
#ifdef StdHashMap
	hashMap.clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.insert(make_pair(i, i + 1));
	}
#else
	hashMap.Clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.Insert(i, i + 1);
	}
#endif


	int k = 0;
	for (auto _ : state) {
#ifdef StdHashMap
		for (int i = 0; i < iterationCount; ++i) {
			for (auto it = hashMap.begin(); it != hashMap.end(); ++it) {
				k = hashMap[it->first];
				state.PauseTiming();
				v.push_back(k);
				state.ResumeTiming();
			}
		}
#else
		for (int i = 0; i < iterationCount; ++i) {
			auto it = hashMap.Begin();
			while (it->HasNext()) {
				k = hashMap[it->Next().Key];
				state.PauseTiming();
				v.push_back(k);
				state.ResumeTiming();
			}
		}
#endif
	}
#ifdef StdHashMap
	DebugAssertMsg(v.size() == dataSize * iterationCount, "데이터가 이상합니다.");
#else
	DebugAssertMsg(v.size() == dataSize * iterationCount, "데이터가 이상합니다.");
#endif
}

// 데이터 64개를 반복자 65536번 생성해서 순회
BENCHMARK(BM_HashMapIteratorCreation)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Args({ 1 << 2, 1 << 20 });

// 데이터 4,194,304개를 반복자 1개를 생성해서 순회
// BENCHMARK(BM_HashMapIteratorVisit)->Iterations(5)->Unit(TimeUnit::kMillisecond)->Arg(1 << 24);	// 데이터 400


/*

------------------------------------------------------------------------------------
                                 STD HASH MAP
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5              5233 ms         4984 ms            1
BM_HashMapSearch/4194304/repeats:5              5087 ms         4500 ms            1
BM_HashMapSearch/4194304/repeats:5              4917 ms         5047 ms            1
BM_HashMapSearch/4194304/repeats:5              4955 ms         4938 ms            1
BM_HashMapSearch/4194304/repeats:5              4971 ms         5094 ms            1
BM_HashMapSearch/4194304/repeats:5_mean         5033 ms         4913 ms            5
BM_HashMapSearch/4194304/repeats:5_median       4971 ms         4984 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev        129 ms          238 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           2.56 %          4.85 %             5


------------------------------------------------------------------------------------
							  JCORE HASM MAP 
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5              5273 ms         5391 ms            1
BM_HashMapSearch/4194304/repeats:5              5119 ms         5531 ms            1
BM_HashMapSearch/4194304/repeats:5              4981 ms         4953 ms            1
BM_HashMapSearch/4194304/repeats:5              4994 ms         4859 ms            1
BM_HashMapSearch/4194304/repeats:5              4996 ms         4891 ms            1
BM_HashMapSearch/4194304/repeats:5_mean         5072 ms         5125 ms            5
BM_HashMapSearch/4194304/repeats:5_median       4996 ms         4953 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev        125 ms          312 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           2.47 %          6.10 %             5





// STD HASH MAP
// 데이터 4, 생성 1048576회
BM_HashMapIteratorCreation/4/1048576/iterations:5       4577 ms         4572 ms            5



// JCORE HASMAP (할당자 없음)
// 데이터 4, 생성 1048576회
BM_HashMapIteratorCreation/4/1048576/iterations:5       4822 ms         4791 ms            5


// JCORE HASMAP (할당자 있음)
// 메모리 블록 찾는데 시간이 더 걸리는 것 같은데;
// 데이터 4, 생성 1048576회
BM_HashMapIteratorCreation/4/1048576/iterations:5       4935 ms         5009 ms            5

*/

#endif