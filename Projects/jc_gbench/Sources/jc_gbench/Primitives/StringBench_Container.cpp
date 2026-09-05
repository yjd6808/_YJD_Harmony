#include "Core.h"
#include "jc_gbench/Primitives/StringBench_Common.h"

#include <vector>
#include <algorithm>

using namespace jc_gbench;

#if StringContainerBenchTest == ON

// B9: N개 push + sort (sizeof 차이와 간접 참조 비용)
template <typename TString>
static void BM_String_VectorSort(State& state)
{
	const StrBenchData d = GetStrBenchData(3);
	const int n = (int)state.range(0);
	for (auto _ : state)
	{
		state.PauseTiming();
		std::vector<TString> v;
		v.reserve(n);
		for (int i = 0; i < n; ++i)
			v.emplace_back(d.p);
		state.ResumeTiming();

		std::sort(v.begin(), v.end());

		DoNotOptimize(v.data());
		ClobberMemory();
	}
}

// B10: 정렬된 N개에서 binary_search (해시/비교 경로 대신 operator< 비교 비용)
template <typename TString>
static void BM_String_BinarySearch(State& state)
{
	const int n = (int)state.range(0);
	std::vector<TString> v;
	v.reserve(n);
	char buf[32];
	for (int i = 0; i < n; ++i)
	{
		snprintf(buf, sizeof(buf), "key_%06d", i);
		v.emplace_back(buf);
	}
	std::sort(v.begin(), v.end());

	int k = 0;
	for (auto _ : state)
	{
		snprintf(buf, sizeof(buf), "key_%06d", k % n);
		const TString needle(buf);
		const bool found = std::binary_search(v.begin(), v.end(), needle);
		DoNotOptimize(found);
		++k;
		ClobberMemory();
	}
}

#if BenchStrB9 == ON
STR_REG_R0(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S0(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S1(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S2(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S3(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S4(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S5(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S6(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_S7(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
STR_REG_R1(BM_String_VectorSort, Arg(10000)->Unit(benchmark::TimeUnit::kMillisecond));
#endif

#if BenchStrB10 == ON
STR_REG_R0(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S0(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S1(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S2(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S3(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S4(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S5(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S6(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S7(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_R1(BM_String_BinarySearch, Arg(10000)->Unit(benchmark::TimeUnit::kMicrosecond));
#endif

#endif
