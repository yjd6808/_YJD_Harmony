/*
 * 작성자: 윤정도
 */

#include <JCoreBenchmark/CoreBenchmark.h>

/*
 * Reserve와 Commit 성능 테스트
 * 테스트 결론: 예약함과 동시에 커밋을 수행하는게 예약한 후 커밋 하는 것보다 성능이 좋다.
 *            32'768페이지를 예약함과 동시에 커밋하는게 32'768페이지를 예약하고 나중에 1페이지를 커밋하는것보다 더 성능이 좋다.
 *			  1페이지당 4'096바이트임을 감안하면 
 */


#if VirtualAllocTest == ON

#define CalculateFree	OFF						// 해제하는 시간 포함할지
#define NoLoop			OFF						// 연속 할당 테스트
constexpr int PageSize_v = 4'096;

#if NoLoop == ON
constexpr int MaxPageCount_v = 32'768;
constexpr int MaxLoop_v = 1;
#else
constexpr int MaxPageCount_v = 1'024;
constexpr int MaxLoop_v = 30;
#endif


// MEM RESERVE와 동시에 MEM COMMIT을 수행한 경우와
// MEM RESERVE만 수행한 경우의 성능 비교
static void BM_Case1(State& state) {
	int iPageCount = (int)state.range(0);
	int iOption = (int)state.range(1);
	
	for (auto _ : state) {
		void* ptr = VirtualAlloc(nullptr, PageSize_v * iPageCount, iOption, PAGE_READWRITE);
		state.PauseTiming();
		
#if CalculateFree == ON
		assert(ptr != nullptr);
		state.ResumeTiming();
#endif
		VirtualFree(ptr, 0, MEM_RELEASE);
#if CalculateFree == OFF
		assert(p != nullptr);
		state.ResumeTiming();
#endif
	}
}

// MEM_RESERVE를 크게 해놓은 후
// MEM_COMMIT을 1개의 페이지부터 시작해서 32'768까지 한번에 커밋할 때
// 한번에 커밋하는 수치가 차이나면 성능 차이도 발생하는지
static void BM_Case2(State& state) {
	int iCommitPageCount = (int)state.range(0);

	for (auto _ : state) {
		state.PauseTiming();
		void* pBase = VirtualAlloc(nullptr, PageSize_v * MaxPageCount_v, MEM_RESERVE, PAGE_READWRITE);		// 크게 예약
		state.ResumeTiming();
		void* p = VirtualAlloc(pBase, PageSize_v * iCommitPageCount, MEM_COMMIT, PAGE_READWRITE);			// 일부 사용

		state.PauseTiming();
#if CalculateFree == ON
		assert(p != nullptr);
		state.ResumeTiming();
#endif
		VirtualFree(pBase, 0, MEM_RELEASE);
#if CalculateFree == OFF
		assert(p != nullptr);
		state.ResumeTiming();
#endif
	}
}


BENCHMARK(BM_Case1)->ArgsProduct({
	{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, MaxPageCount_v },
	{ MEM_RESERVE | MEM_COMMIT, MEM_RESERVE }
});

BENCHMARK(BM_Case2)->RangeMultiplier(2)->Range(1, MaxPageCount_v);

/*
 *
 
---------------------------------------------------------------

                    Virtual Free 포함 벤치마크

Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------

[RESERVE | COMMIT]
BM_Case1/1/12288           5666 ns         5625 ns       100000
BM_Case1/2/12288           4955 ns         5000 ns       100000
BM_Case1/4/12288           4876 ns         4604 ns       112000
BM_Case1/8/12288           5270 ns         5625 ns       100000
BM_Case1/16/12288          5049 ns         5000 ns       100000
BM_Case1/32/12288          5060 ns         5022 ns       112000
BM_Case1/64/12288          6038 ns         5781 ns       100000
BM_Case1/128/12288         5974 ns         6417 ns       112000
BM_Case1/256/12288         5823 ns         6417 ns       112000
BM_Case1/512/12288         5276 ns         5156 ns       100000
BM_Case1/1024/12288        5719 ns         5301 ns       112000
BM_Case1/2048/12288        5443 ns         5859 ns       112000
BM_Case1/4096/12288        5634 ns         5720 ns       112000
BM_Case1/32768/12288       7739 ns         8092 ns       112000

[RESERVE]
BM_Case1/1/8192            4351 ns         4743 ns       112000
BM_Case1/2/8192            4500 ns         3906 ns       160000
BM_Case1/4/8192            4510 ns         4604 ns       112000
BM_Case1/8/8192            4707 ns         5176 ns       160000
BM_Case1/16/8192           4844 ns         4973 ns       144516
BM_Case1/32/8192           4951 ns         4395 ns       149333
BM_Case1/64/8192           5782 ns         5720 ns       112000
BM_Case1/128/8192          5505 ns         5441 ns       112000
BM_Case1/256/8192          5424 ns         5625 ns       100000
BM_Case1/512/8192          5012 ns         4865 ns       144516
BM_Case1/1024/8192         4983 ns         5301 ns       112000
BM_Case1/2048/8192         4811 ns         5022 ns       112000
BM_Case1/4096/8192         5133 ns         4414 ns       148670
BM_Case1/32768/8192        5877 ns         6278 ns        89600



BM_Case2/1                10160 ns        10812 ns        89600
BM_Case2/2                 9998 ns        10603 ns        56000
BM_Case2/4                10006 ns        10289 ns        89600
BM_Case2/8                10647 ns        10254 ns        64000
BM_Case2/16               11595 ns        11998 ns        56000
BM_Case2/32               10992 ns        11719 ns        64000
BM_Case2/64               12546 ns        13497 ns        49778
BM_Case2/128              16025 ns        16323 ns        37333
BM_Case2/256              21788 ns        21972 ns        29867
BM_Case2/512              36036 ns        36621 ns        17920
BM_Case2/1024             65277 ns        65569 ns        11200
BM_Case2/2048            116230 ns       117188 ns         5600
BM_Case2/4096            208841 ns       209961 ns         3200
BM_Case2/8192            451537 ns       452080 ns         1659
BM_Case2/16384           878122 ns       878514 ns          747
BM_Case2/32768          1668889 ns      1633713 ns          373




---------------------------------------------------------------
                 Virtual Free 미포함 벤치마크
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
[RESERVE | COMMIT]
BM_Case1/1/12288           3099 ns         3578 ns       235789
BM_Case1/2/12288           3034 ns         2584 ns       235789
BM_Case1/4/12288           3018 ns         3034 ns       288368
BM_Case1/8/12288           2916 ns         3453 ns       407273
BM_Case1/16/12288          3050 ns         3024 ns       263529
BM_Case1/32/12288          2845 ns         2951 ns       248889
BM_Case1/64/12288          2978 ns         3467 ns       320000
BM_Case1/128/12288         2981 ns         2849 ns       235789
BM_Case1/256/12288         3012 ns         3181 ns       280000
BM_Case1/512/12288         2884 ns         2539 ns       344615
BM_Case1/1024/12288        3003 ns         3380 ns       263529
BM_Case1/2048/12288        2803 ns         2905 ns       263529
BM_Case1/4096/12288        3039 ns         3158 ns       192985
BM_Case1/32768/12288       3594 ns         3296 ns       213333


[RESERVE]
BM_Case1/1/8192            2497 ns         2668 ns       263529
BM_Case1/2/8192            2509 ns         2448 ns       248889
BM_Case1/4/8192            2547 ns         2651 ns       235789
BM_Case1/8/8192            2523 ns         2267 ns       344615
BM_Case1/16/8192           2503 ns         2609 ns       203636
BM_Case1/32/8192           2443 ns         2490 ns       263529
BM_Case1/64/8192           2493 ns         2846 ns       373333
BM_Case1/128/8192          2487 ns         2720 ns       298667
BM_Case1/256/8192          2611 ns         2790 ns       280000
BM_Case1/512/8192          2526 ns         2354 ns       298667
BM_Case1/1024/8192         2478 ns         2888 ns       248889
BM_Case1/2048/8192         2425 ns         2386 ns       235789
BM_Case1/4096/8192         2611 ns         2720 ns       224000
BM_Case1/32768/8192        2475 ns         2407 ns       298667


BM_Case2/1                 3741 ns         3599 ns       186667
BM_Case2/2                 3809 ns         3181 ns       186667
BM_Case2/4                 3689 ns         3516 ns       160000
BM_Case2/8                 3823 ns         2888 ns       194783
BM_Case2/16                3798 ns         3955 ns       248889
BM_Case2/32                3821 ns         3906 ns       224000
BM_Case2/64                3994 ns         3581 ns       200704
BM_Case2/128               4084 ns         3438 ns       218157
BM_Case2/256               5287 ns         6627 ns        89600
BM_Case2/512               6167 ns         7175 ns       156800
BM_Case2/1024              8330 ns         6975 ns       112000
BM_Case2/2048             12537 ns        14230 ns        56000
BM_Case2/4096             21226 ns        20508 ns        32000
BM_Case2/8192             38651 ns        37354 ns        21333
BM_Case2/16384            72851 ns        85449 ns         6400
BM_Case2/32768           144384 ns       165311 ns         7467


 */

#endif
