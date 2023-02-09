/*
 * 작성자: 윤정도
 */
#include <JCoreBenchmark/CoreBenchmark.h>

/*
 * VirtualAlloc, new, operator new의 성능을 테스트해보도록 한다.
 * new는 operator new를 수행하고 생성자를 호출해주는 기능을 수행한다.
 * 따라서 생성자가 없는 타입들에 대해서는 operator new나 new나 성능차이가 없을 것이다.
 *
 * new나 malloc 모두 결국 HeapAlloc을 호출한다.
 * 그리고 결국 HeapAlloc으로 할당받는 메모리는 HeapCreate을 통해서 Reserved 메모리영역이다.
 * HeapCreate은 VirtualAlloc으로 메모리를 Reserve 해놓고 내부적으로 자료구조를 두고 있는데
 * 이 자료구조는 VirtualAlloc으로 Reserved 큰 메모리 덩어리를 잘게 나눠서 관리한다.
 *
 * delete, new는 최종적으로 HeapAlloc과 HeapFree를 호출하는데 이 함수들은
 * 실제로 새로운 메모리 영역을 할당/해제하는게 아니다. (할당은 당연히 HeapCreate로 Reserved된 사이즈보다 작다는 가정하에)
 * 이전에 Reserved되었던 영역에서 나눠주게 됨.
 * 정리하면 우리가 쓰는 new는 원래 VirtualAlloc을 통해서 할당된 큰 메모리 덩어리들로
 * 이걸 잘게 나눠서 풀로 구성해놓고 유저가 요청한대로 편하게 나눠주도록 하는 그런걸로 보인다.
 * 
 * 테스트를 해보면 0x10000 크기를 초과하는 할당요청을 자주 수행하는 경우 new, operator new보다
 * VirtualAlloc의 성능이 더 좋아지는걸 확인할 수 있다.
 *
 * 테스트 결론: 큰 메모리 덩어리를 자주 할당받고 해제해야할 경우 VirtualAlloc이 유리하다.
 * 
 * ==================================================================================
 * VirtualAlloc: 1byte만 할당받고 싶다고하더라도 최소 페이지(4096) 단위로 메모리를 할당해줌
 *				 주소가 0x10000 배수로만 할당되기 때문에 페이지 1개만 
 *               반환할때 할당받은 페이지내의 아무 주소나 반환하더라도 성공적으로 반환이 이뤄짐
 *
 * MEM_RESERVE: 다른 프로세스에서 예약된 영역의 메모리를 사용하지 못하게 만든다.
 * MEM_COMMIT: 물리 메모리와 커밋 요청한 프로세스의 가상 메모리를 매핑해줌
 *
 */


#if AllocationTest == ON

static void BM_VirtualAllocInefficient(State& state) {
	int iAllocSize = (int)state.range(0);
	int iAllocCount = (int)state.range(1);
	void** pAlloc = (void**)new int*[iAllocCount];
	for (auto _ : state) {
		for (int i = 0; i < iAllocCount; ++i) {
			pAlloc[i] = VirtualAlloc(nullptr, iAllocSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		}
		for (int i = 0; i < iAllocCount; ++i) {
			VirtualFree(pAlloc[i], 0, MEM_RELEASE);
		}
	}
}

static void BM_NewAllocInefficient(State& state) {
	int iAllocSize = (int)state.range(0);
	int iAllocCount = (int)state.range(1);
	void** pAlloc = (void**)new int* [iAllocCount];
	for (auto _ : state) {
		for (int i = 0; i < iAllocCount; ++i) {
			pAlloc[i] = new char[iAllocSize];
		}
		for (int i = 0; i < iAllocCount; ++i) {
			delete[] pAlloc[i];
		}
	}
}

static void BM_OperatorNewInefficent(State& state) {
	int iAllocSize = (int)state.range(0);
	int iAllocCount = (int)state.range(1);
	void** pAlloc = (void**)new int* [iAllocCount];
	for (auto _ : state) {
		for (int i = 0; i < iAllocCount; ++i) {
			pAlloc[i] = operator new(iAllocSize);
		}
		for (int i = 0; i < iAllocCount; ++i) {
			delete[] pAlloc[i];
		}
	}
}

BENCHMARK(BM_VirtualAllocInefficient)->ArgsProduct({
	{ 1, 64, 256, 1'024, 4'096, 16'384, 65'536, 262'144, 1'048'576, 4'194'304 },	// 할당받을 메모리 크기
	{ 100, 200, 400 }															// 연속으로 몇번할당 받을지 (그만큼 다시 반환)
});

BENCHMARK(BM_NewAllocInefficient)->ArgsProduct({
	{ 1, 64, 256, 1'024, 4'096, 16'384, 65'536, 262'144, 1'048'576, 4'194'304 },
	{ 100, 200, 400 }
});

BENCHMARK(BM_OperatorNewInefficent)->ArgsProduct({
	{ 1, 64, 256, 1'024, 4'096, 16'384, 65'536, 262'144, 1'048'576, 4'194'304 },
	{ 100, 200, 400 }
});




/* 릴리즈 모드
 *
 * 65536/200를 찾아서 비교해보면 AllocationGranularity 크기를 기점으로 VirtualAlloc이
 * operator new와 new를 앞서기 시작한다.
 *
---------------------------------------------------------------------------------
Benchmark                                       Time             CPU   Iterations
---------------------------------------------------------------------------------
BM_VirtualAllocInefficient/1/1               4421 ns         4049 ns       165926
BM_VirtualAllocInefficient/64/1              6378 ns         4813 ns       149333
BM_VirtualAllocInefficient/256/1             3750 ns         3737 ns       167253
BM_VirtualAllocInefficient/1024/1            3724 ns         3610 ns       194783
BM_VirtualAllocInefficient/4096/1            3855 ns         3760 ns       203636
BM_VirtualAllocInefficient/16384/1           3748 ns         3516 ns       186667
BM_VirtualAllocInefficient/65536/1           4296 ns         4111 ns       182458
BM_VirtualAllocInefficient/262144/1          4580 ns         4349 ns       154483
BM_VirtualAllocInefficient/1048576/1         7663 ns         7533 ns       112000
BM_VirtualAllocInefficient/4194304/1         4919 ns         4604 ns       149333
BM_VirtualAllocInefficient/1/5              19355 ns        18589 ns        34462
BM_VirtualAllocInefficient/64/5             19375 ns        18834 ns        37333
BM_VirtualAllocInefficient/256/5            19673 ns        18136 ns        34462
BM_VirtualAllocInefficient/1024/5           19223 ns        17997 ns        37333
BM_VirtualAllocInefficient/4096/5           19260 ns        18415 ns        37333
BM_VirtualAllocInefficient/16384/5          18547 ns        17997 ns        37333
BM_VirtualAllocInefficient/65536/5          21268 ns        20508 ns        32000
BM_VirtualAllocInefficient/262144/5         22974 ns        21449 ns        29867
BM_VirtualAllocInefficient/1048576/5        26959 ns        25111 ns        29867
BM_VirtualAllocInefficient/4194304/5        22031 ns        20996 ns        32000
BM_VirtualAllocInefficient/1/20             74905 ns        73239 ns         7467
BM_VirtualAllocInefficient/64/20            76365 ns        77424 ns         7467
BM_VirtualAllocInefficient/256/20           78001 ns        74986 ns         8960
BM_VirtualAllocInefficient/1024/20          75721 ns        69054 ns         7467
BM_VirtualAllocInefficient/4096/20          73426 ns        71498 ns         8960
BM_VirtualAllocInefficient/16384/20         82323 ns        78125 ns        11200
BM_VirtualAllocInefficient/65536/20         78971 ns        78474 ns         8960
BM_VirtualAllocInefficient/262144/20        88476 ns        85794 ns         7467
BM_VirtualAllocInefficient/1048576/20       92509 ns        87193 ns         8960
BM_VirtualAllocInefficient/4194304/20       86346 ns        83705 ns         8960
BM_VirtualAllocInefficient/1/40            156964 ns       156941 ns         4978
BM_VirtualAllocInefficient/64/40           155139 ns       149972 ns         4480
BM_VirtualAllocInefficient/256/40          160945 ns       153460 ns         4480
BM_VirtualAllocInefficient/1024/40         150702 ns       145089 ns         5600
BM_VirtualAllocInefficient/4096/40         154312 ns       153802 ns         4978
BM_VirtualAllocInefficient/16384/40        153221 ns       147879 ns         5600
BM_VirtualAllocInefficient/65536/40        166860 ns       160435 ns         4480
BM_VirtualAllocInefficient/262144/40       189250 ns       181362 ns         4480
BM_VirtualAllocInefficient/1048576/40      185221 ns       175797 ns         3733
BM_VirtualAllocInefficient/4194304/40      173842 ns       164958 ns         4073
BM_VirtualAllocInefficient/1/200           828498 ns       784738 ns          896
BM_VirtualAllocInefficient/64/200          840376 ns       819615 ns          896
BM_VirtualAllocInefficient/256/200         832373 ns       810811 ns          925
BM_VirtualAllocInefficient/1024/200        819872 ns       784738 ns          896
BM_VirtualAllocInefficient/4096/200        882814 ns       854492 ns          896
BM_VirtualAllocInefficient/16384/200       841628 ns       802176 ns          896
BM_VirtualAllocInefficient/65536/200       948986 ns       889369 ns          896
BM_VirtualAllocInefficient/262144/200      852423 ns       837054 ns          896
BM_VirtualAllocInefficient/1048576/200     880264 ns       857597 ns          747
BM_VirtualAllocInefficient/4194304/200     854682 ns       836680 ns          747
BM_NewAllocInefficient/1/1                   71.5 ns         65.6 ns     11200000
BM_NewAllocInefficient/64/1                  68.6 ns         65.6 ns     11200000
BM_NewAllocInefficient/256/1                 64.5 ns         62.8 ns     11200000
BM_NewAllocInefficient/1024/1                64.9 ns         61.4 ns     11200000
BM_NewAllocInefficient/4096/1                63.6 ns         60.0 ns     11200000
BM_NewAllocInefficient/16384/1               66.8 ns         62.8 ns      8960000
BM_NewAllocInefficient/65536/1                180 ns          173 ns      4072727
BM_NewAllocInefficient/262144/1               209 ns          185 ns      4480000
BM_NewAllocInefficient/1048576/1            11683 ns        10603 ns        56000
BM_NewAllocInefficient/4194304/1            13318 ns        12870 ns        49778
BM_NewAllocInefficient/1/5                    313 ns          293 ns      2133333
BM_NewAllocInefficient/64/5                   322 ns          307 ns      2240000
BM_NewAllocInefficient/256/5                  339 ns          331 ns      2357895
BM_NewAllocInefficient/1024/5                 373 ns          353 ns      2036364
BM_NewAllocInefficient/4096/5                 355 ns          344 ns      2133333
BM_NewAllocInefficient/16384/5                349 ns          352 ns      2133333
BM_NewAllocInefficient/65536/5               1205 ns         1099 ns       497778
BM_NewAllocInefficient/262144/5             26450 ns        24902 ns        26353
BM_NewAllocInefficient/1048576/5            72428 ns        69754 ns        11200
BM_NewAllocInefficient/4194304/5            77413 ns        73242 ns         8960
BM_NewAllocInefficient/1/20                  1284 ns         1172 ns       560000
BM_NewAllocInefficient/64/20                 1341 ns         1283 ns       560000
BM_NewAllocInefficient/256/20                1278 ns         1186 ns       448000
BM_NewAllocInefficient/1024/20               1248 ns         1256 ns       497778
BM_NewAllocInefficient/4096/20               1298 ns         1245 ns       640000
BM_NewAllocInefficient/16384/20              2443 ns         2431 ns       263529
BM_NewAllocInefficient/65536/20             82032 ns        73940 ns        11200
BM_NewAllocInefficient/262144/20            44106 ns        41470 ns        15448
BM_NewAllocInefficient/1048576/20          284404 ns       263660 ns         2489
BM_NewAllocInefficient/4194304/20          291630 ns       282493 ns         2489
BM_NewAllocInefficient/1/40                  2583 ns         2490 ns       263529
BM_NewAllocInefficient/64/40                 3777 ns         3610 ns       194783
BM_NewAllocInefficient/256/40                3363 ns         3299 ns       203636
BM_NewAllocInefficient/1024/40               3367 ns         3223 ns       203636
BM_NewAllocInefficient/4096/40               3512 ns         3488 ns       224000
BM_NewAllocInefficient/16384/40             10765 ns        10498 ns        64000
BM_NewAllocInefficient/65536/40              9895 ns         9417 ns        74667
BM_NewAllocInefficient/262144/40           254195 ns       235395 ns         2987
BM_NewAllocInefficient/1048576/40          536455 ns       500000 ns         1000
BM_NewAllocInefficient/4194304/40          615109 ns       593750 ns         1000
BM_NewAllocInefficient/1/200                15897 ns        15346 ns        40727
BM_NewAllocInefficient/64/200               16565 ns        16044 ns        44800
BM_NewAllocInefficient/256/200              18184 ns        18032 ns        40727
BM_NewAllocInefficient/1024/200             19626 ns        19183 ns        40727
BM_NewAllocInefficient/4096/200             19689 ns        18415 ns        37333
BM_NewAllocInefficient/16384/200            75608 ns        68359 ns        11200
BM_NewAllocInefficient/65536/200           607360 ns       593750 ns         1000
BM_NewAllocInefficient/262144/200         1756834 ns      1650799 ns          407
BM_NewAllocInefficient/1048576/200        2926746 ns      2790179 ns          280
BM_NewAllocInefficient/4194304/200        2875791 ns      2823795 ns          249
BM_OperatorNewInefficent/1/1                 68.4 ns         66.3 ns      8960000
BM_OperatorNewInefficent/64/1                65.0 ns         64.2 ns     11200000
BM_OperatorNewInefficent/256/1               66.1 ns         64.2 ns     11200000
BM_OperatorNewInefficent/1024/1              64.6 ns         61.4 ns     11200000
BM_OperatorNewInefficent/4096/1              66.3 ns         62.8 ns     11200000
BM_OperatorNewInefficent/16384/1             68.4 ns         67.0 ns     11200000
BM_OperatorNewInefficent/65536/1              203 ns          199 ns      3446154
BM_OperatorNewInefficent/262144/1             249 ns          240 ns      2800000
BM_OperatorNewInefficent/1048576/1          11419 ns        10882 ns        56000
BM_OperatorNewInefficent/4194304/1          15648 ns        15381 ns        49778
BM_OperatorNewInefficent/1/5                  335 ns          318 ns      2357895
BM_OperatorNewInefficent/64/5                 336 ns          318 ns      2357895
BM_OperatorNewInefficent/256/5                369 ns          345 ns      2036364
BM_OperatorNewInefficent/1024/5               338 ns          315 ns      2133333
BM_OperatorNewInefficent/4096/5               385 ns          371 ns      2357895
BM_OperatorNewInefficent/16384/5              352 ns          330 ns      2036364
BM_OperatorNewInefficent/65536/5             1002 ns         1004 ns       746667
BM_OperatorNewInefficent/262144/5            1354 ns         1339 ns       560000
BM_OperatorNewInefficent/1048576/5          72353 ns        66964 ns        11200
BM_OperatorNewInefficent/4194304/5          72869 ns        71150 ns        11200
BM_OperatorNewInefficent/1/20                1322 ns         1287 ns       497778
BM_OperatorNewInefficent/64/20               1323 ns         1228 ns       560000
BM_OperatorNewInefficent/256/20              1296 ns         1270 ns       640000
BM_OperatorNewInefficent/1024/20             1315 ns         1283 ns       560000
BM_OperatorNewInefficent/4096/20             1363 ns         1343 ns       640000
BM_OperatorNewInefficent/16384/20            2556 ns         2459 ns       298667
BM_OperatorNewInefficent/65536/20            4814 ns         4551 ns       154483
BM_OperatorNewInefficent/262144/20           9364 ns         9242 ns        89600
BM_OperatorNewInefficent/1048576/20        279328 ns       266841 ns         2635
BM_OperatorNewInefficent/4194304/20        293709 ns       284934 ns         2358
BM_OperatorNewInefficent/1/40                3652 ns         3488 ns       179200
BM_OperatorNewInefficent/64/40               3390 ns         3076 ns       213333
BM_OperatorNewInefficent/256/40              3714 ns         3557 ns       224000
BM_OperatorNewInefficent/1024/40             3429 ns         3296 ns       213333
BM_OperatorNewInefficent/4096/40             3437 ns         3299 ns       203636
BM_OperatorNewInefficent/16384/40            5153 ns         5000 ns       100000
BM_OperatorNewInefficent/65536/40           11362 ns        10882 ns        56000
BM_OperatorNewInefficent/262144/40         121288 ns       112305 ns         6400
BM_OperatorNewInefficent/1048576/40        536954 ns       515625 ns         1000
BM_OperatorNewInefficent/4194304/40        557667 ns       515625 ns         1000
BM_OperatorNewInefficent/1/200              16085 ns        14962 ns        40727
BM_OperatorNewInefficent/64/200             16544 ns        16392 ns        44800
BM_OperatorNewInefficent/256/200            20008 ns        18834 ns        37333
BM_OperatorNewInefficent/1024/200           18161 ns        17160 ns        37333
BM_OperatorNewInefficent/4096/200           20450 ns        19950 ns        40727
BM_OperatorNewInefficent/16384/200          59325 ns        54688 ns        10000
BM_OperatorNewInefficent/65536/200          94313 ns        90681 ns         8960
BM_OperatorNewInefficent/262144/200       1897606 ns      1801273 ns          373
BM_OperatorNewInefficent/1048576/200      2970279 ns      2845982 ns          280
BM_OperatorNewInefficent/4194304/200      2729870 ns      2640845 ns          213


100 200 400
---------------------------------------------------------------------------------
Benchmark                                       Time             CPU   Iterations
---------------------------------------------------------------------------------
BM_VirtualAllocInefficient/1/100           453241 ns       429010 ns         1384
BM_VirtualAllocInefficient/64/100          419323 ns       423825 ns         1659
BM_VirtualAllocInefficient/256/100         424695 ns       427246 ns         1792
BM_VirtualAllocInefficient/1024/100        411121 ns       417150 ns         1723
BM_VirtualAllocInefficient/4096/100        412415 ns       408082 ns         1723
BM_VirtualAllocInefficient/16384/100       388761 ns       392369 ns         1792
BM_VirtualAllocInefficient/65536/100       408147 ns       410156 ns         1600
BM_VirtualAllocInefficient/262144/100      421676 ns       426219 ns         1723
BM_VirtualAllocInefficient/1048576/100     439538 ns       421713 ns         1445
BM_VirtualAllocInefficient/4194304/100     436510 ns       433243 ns         1659
BM_VirtualAllocInefficient/1/200           781785 ns       784738 ns          896
BM_VirtualAllocInefficient/64/200          791964 ns       802176 ns          896
BM_VirtualAllocInefficient/256/200         807122 ns       794846 ns          747
BM_VirtualAllocInefficient/1024/200        797778 ns       802176 ns          896
BM_VirtualAllocInefficient/4096/200        774950 ns       767299 ns          896
BM_VirtualAllocInefficient/16384/200       812825 ns       802176 ns          896
BM_VirtualAllocInefficient/65536/200       878082 ns       889369 ns          896
BM_VirtualAllocInefficient/262144/200      796126 ns       802176 ns          896
BM_VirtualAllocInefficient/1048576/200     801918 ns       794846 ns          747
BM_VirtualAllocInefficient/4194304/200     851165 ns       836680 ns          747
BM_VirtualAllocInefficient/1/400          1577576 ns      1569475 ns          448
BM_VirtualAllocInefficient/64/400         1586100 ns      1574017 ns          407
BM_VirtualAllocInefficient/256/400        1564479 ns      1569475 ns          448
BM_VirtualAllocInefficient/1024/400       1689988 ns      1674107 ns          448
BM_VirtualAllocInefficient/4096/400       1573366 ns      1569475 ns          448
BM_VirtualAllocInefficient/16384/400      1606884 ns      1639230 ns          448
BM_VirtualAllocInefficient/65536/400      1559890 ns      1569475 ns          448
BM_VirtualAllocInefficient/262144/400     1597898 ns      1574017 ns          407
BM_VirtualAllocInefficient/1048576/400    1585861 ns      1574017 ns          407
BM_VirtualAllocInefficient/4194304/400    1645245 ns      1650799 ns          407
BM_NewAllocInefficient/1/100                 8163 ns         8161 ns        74667
BM_NewAllocInefficient/64/100                7830 ns         7847 ns        89600
BM_NewAllocInefficient/256/100               8518 ns         8580 ns        74667
BM_NewAllocInefficient/1024/100              9166 ns         9208 ns        74667
BM_NewAllocInefficient/4096/100              9138 ns         9087 ns        75656
BM_NewAllocInefficient/16384/100            97479 ns        98349 ns         7467
BM_NewAllocInefficient/65536/100           212884 ns       214844 ns         3200
BM_NewAllocInefficient/262144/100          513655 ns       516183 ns         1120
BM_NewAllocInefficient/1048576/100        1319677 ns      1283482 ns          560
BM_NewAllocInefficient/4194304/100        1288240 ns      1286396 ns          498
BM_NewAllocInefficient/1/200                15404 ns        15346 ns        44800
BM_NewAllocInefficient/64/200               14996 ns        15067 ns        49778
BM_NewAllocInefficient/256/200              18424 ns        18415 ns        40727
BM_NewAllocInefficient/1024/200             18821 ns        18834 ns        37333
BM_NewAllocInefficient/4096/200             18239 ns        18415 ns        40727
BM_NewAllocInefficient/16384/200            62272 ns        62779 ns        11200
BM_NewAllocInefficient/65536/200           116570 ns       114397 ns         5600
BM_NewAllocInefficient/262144/200         1516791 ns      1537400 ns          498
BM_NewAllocInefficient/1048576/200        2547148 ns      2566964 ns          280
BM_NewAllocInefficient/4194304/200        2651718 ns      2698293 ns          249
BM_NewAllocInefficient/1/400                30044 ns        29820 ns        23579
BM_NewAllocInefficient/64/400               29503 ns        29297 ns        22400
BM_NewAllocInefficient/256/400              26129 ns        25844 ns        23579
BM_NewAllocInefficient/1024/400             31984 ns        31459 ns        20364
BM_NewAllocInefficient/4096/400             44852 ns        45200 ns        18667
BM_NewAllocInefficient/16384/400           135640 ns       138108 ns         4978
BM_NewAllocInefficient/65536/400          1199889 ns      1220703 ns          448
BM_NewAllocInefficient/262144/400         3490339 ns      3509358 ns          187
BM_NewAllocInefficient/1048576/400        5108984 ns      5161830 ns          112
BM_NewAllocInefficient/4194304/400        5493760 ns      5468750 ns          100
BM_OperatorNewInefficent/1/100               7864 ns         7847 ns        89600
BM_OperatorNewInefficent/64/100              7875 ns         7847 ns        89600
BM_OperatorNewInefficent/256/100             8649 ns         8580 ns        74667
BM_OperatorNewInefficent/1024/100            8806 ns         8894 ns        89600
BM_OperatorNewInefficent/4096/100            8817 ns         8894 ns        89600
BM_OperatorNewInefficent/16384/100          25068 ns        24902 ns        26353
BM_OperatorNewInefficent/65536/100          57430 ns        55804 ns         8960
BM_OperatorNewInefficent/262144/100        421894 ns       426219 ns         1723
BM_OperatorNewInefficent/1048576/100      1355338 ns      1349147 ns          498
BM_OperatorNewInefficent/4194304/100      1378909 ns      1395089 ns          560
BM_OperatorNewInefficent/1/200              15860 ns        15730 ns        40727
BM_OperatorNewInefficent/64/200             15322 ns        15346 ns        44800
BM_OperatorNewInefficent/256/200            18126 ns        18415 ns        40727
BM_OperatorNewInefficent/1024/200           17762 ns        18032 ns        40727
BM_OperatorNewInefficent/4096/200           19692 ns        19671 ns        37333
BM_OperatorNewInefficent/16384/200          61256 ns        61384 ns        11200
BM_OperatorNewInefficent/65536/200         175425 ns       176467 ns         4073
BM_OperatorNewInefficent/262144/200       1443734 ns      1443273 ns          498
BM_OperatorNewInefficent/1048576/200      2562697 ns      2544981 ns          264
BM_OperatorNewInefficent/4194304/200      2625383 ns      2635542 ns          249
BM_OperatorNewInefficent/1/400              31486 ns        31390 ns        22400
BM_OperatorNewInefficent/64/400             29284 ns        29297 ns        22400
BM_OperatorNewInefficent/256/400            24711 ns        24554 ns        28000
BM_OperatorNewInefficent/1024/400           31910 ns        31459 ns        20364
BM_OperatorNewInefficent/4096/400           40124 ns        39447 ns        15448
BM_OperatorNewInefficent/16384/400         127891 ns       128348 ns         5600
BM_OperatorNewInefficent/65536/400        1020829 ns      1024933 ns          747
BM_OperatorNewInefficent/262144/400       3509747 ns      3523284 ns          204
BM_OperatorNewInefficent/1048576/400      5430723 ns      5440848 ns          112
BM_OperatorNewInefficent/4194304/400      5196479 ns      5161830 ns          112
*/

#endif