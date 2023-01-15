/*
 * 작성자: 윤정도
 * 표준 해쉬맵과 내가 구현한 해쉬맵 성능 비교
 * =====================================================================
 * [ 1차 테스트결과 ]
 *   삽입: 3.2배
 *	 검색: 11배
 *	 스택 메모리: 15배이상
 *	 정도 내가 안좋은 성능을 보인다.
 *
 *	 이미 어느정도 예상을 했다.
 *	 오히려 삼입 성능은 어느정도 선방했다고 생각한다.
 *	 원인도 대략적으로 알고 있었고 만들면서도 느낌 ㅋㅋ
 *
 *	 1. 버킷을 연결리스트 기반으로 기존에 구현한 연결리스트 상속받아 구현,
 *	    이게 좀 무겁다. 쉽게 구현할려고 머리, 꼬리 다달음 메모리를 많이먹는다.
 *	 2. 가상함수화된 컨테이너
 *	 3. 삽입시 매번 노드 동적할당하여 생성
 *	 4. 검색시 매번 반복자 동적할당하여 생성
 *	 5. 반복자도 가상함수화됨
 *   6. 해쉬함수의 성능
 *	 7. 버킷이 모두 양방향 연결리스트이고 
 *
 *	 처음 구현해본 해쉬맵이라 그런지 더욱더 성능이 안좋은것 같다.
 *	 대략 0.001초에 2508개의 데이터를 검색할 수 있다.
 *
 * =====================================================================
 * [ 2차 테스트 결과 ]
 *   삽입: 3배
 *	 검색: 9배
 *   스택 메모리: 4배
 * 1번 문제를 해결했다. 메모리는 크게 개선되었지만 성능은 크게 변함이 없다.
 * 검색시 매번 반복자를 동적할당하여 생성하는 문제를 해결하면 성능이 크게 개선될것이다.
 *
 * =====================================================================
 * [ 3차 테스트 결과 ]
 *   삽입: 0.5배
 *	 검색: 1배
 *   스택 메모리: 4배
 * 반복자를 동적할당하는 문제를 해결하니까 성능이 매우 상승하였다.
 * 
 */

#include <JCoreBenchmark/CoreBenchmark.h>
#include <JCore/Container/HashMap.h>

#include <unordered_map>


#if HashMapTest == ON

#define StdHashMap		OFF

#if StdHashMap == ON
unordered_map<int, int> hashMap{ 10'000'000 };
#else
HashMap<int, int> hashMap{ 10'000'000 };
#endif

static void BM_HashMapSearch(State& state) {
	int64_t dataSize = state.range();
#if StdHashMap == ON
	hashMap.clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.insert(make_pair(i, i));
	}
#else
	hashMap.Clear();
	for (int i = 0; i < dataSize; ++i) {
		hashMap.Insert(i, i);
	}
#endif

	volatile int k = 0;
	for (auto _ : state) {
		for (int i = 0; i < dataSize; ++i) {
			k = hashMap[i];
		}
	}
}



static void BM_HashMapInsert(State& state) {
	int64_t dataSize = state.range();
	
#if StdHashMap == ON
	hashMap.clear();
	for (auto _ : state) {
		for (int i = 0; i < dataSize; ++i) {
			hashMap.insert(make_pair(i, i));
		}
#else
	hashMap.Clear();
	for (auto _ : state) {
		for (int i = 0; i < dataSize; ++i) {
			hashMap.Insert(i, i);
		}
#endif
	}
}

BENCHMARK(BM_HashMapSearch)->Repetitions(5)->ReportAggregatesOnly()->Unit(TimeUnit::kMillisecond)->Arg(1 << 22);
BENCHMARK(BM_HashMapInsert)->Repetitions(5)->ReportAggregatesOnly()->Unit(TimeUnit::kMillisecond)->Arg(1 << 22);




#endif


/*
 *
------------------------------------------------------------------------------------

							STD HASH MAP

------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5_mean          169 ms          170 ms            5
BM_HashMapSearch/4194304/repeats:5_median        165 ms          164 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev       7.44 ms         7.61 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           4.39 %          4.49 %             5
BM_HashMapInsert/4194304/repeats:5_mean          630 ms          628 ms            5
BM_HashMapInsert/4194304/repeats:5_median        617 ms          609 ms            5
BM_HashMapInsert/4194304/repeats:5_stddev       21.8 ms         25.7 ms            5
BM_HashMapInsert/4194304/repeats:5_cv           3.46 %          4.09 %             5

------------------------------------------------------------------------------------

				    JCORE HASH MAP [1차 테스트]
	       			더미노드 연결리스트, 더미노드 해쉬맵

------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5_mean         1771 ms         1759 ms            5
BM_HashMapSearch/4194304/repeats:5_median       1796 ms         1797 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev        138 ms          128 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           7.79 %          7.26 %             5
BM_HashMapInsert/4194304/repeats:5_mean         1981 ms         1981 ms            5
BM_HashMapInsert/4194304/repeats:5_median       1976 ms         1969 ms            5
BM_HashMapInsert/4194304/repeats:5_stddev       50.2 ms         51.1 ms            5
BM_HashMapInsert/4194304/repeats:5_cv           2.54 %          2.58 %             5




------------------------------------------------------------------------------------

					JCORE HASH MAP [2차 테스트 - 2022/12/24일] 
					연결리스트, 해쉬맵 모두 더미노드 제거

------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5_mean         1541 ms         1534 ms            5
BM_HashMapSearch/4194304/repeats:5_median       1570 ms         1547 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev       86.2 ms         82.2 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           5.59 %          5.36 %             5
BM_HashMapInsert/4194304/repeats:5_mean         1845 ms         1844 ms            5
BM_HashMapInsert/4194304/repeats:5_median       1840 ms         1844 ms            5
BM_HashMapInsert/4194304/repeats:5_stddev       63.3 ms         67.2 ms            5
BM_HashMapInsert/4194304/repeats:5_cv           3.43 %          3.65 %             5



------------------------------------------------------------------------------------

					JCORE HASH MAP [3차 테스트 - 2022/12/25일]
					연결리스트대신 공허의 다이나믹 배열로 변경
					반복자를 안써서 속도가 비약적으로 상승한듯?

------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_HashMapSearch/4194304/repeats:5_mean          196 ms          195 ms            5
BM_HashMapSearch/4194304/repeats:5_median        191 ms          188 ms            5
BM_HashMapSearch/4194304/repeats:5_stddev       10.5 ms         12.5 ms            5
BM_HashMapSearch/4194304/repeats:5_cv           5.37 %          6.44 %             5
BM_HashMapInsert/4194304/repeats:5_mean          344 ms          341 ms            5
BM_HashMapInsert/4194304/repeats:5_median        343 ms          344 ms            5
BM_HashMapInsert/4194304/repeats:5_stddev       6.82 ms         8.91 ms            5
BM_HashMapInsert/4194304/repeats:5_cv           1.98 %          2.62 %             5

*/
