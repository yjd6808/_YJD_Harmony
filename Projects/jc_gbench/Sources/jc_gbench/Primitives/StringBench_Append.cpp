#include "Core.h"
#include "jc_gbench/Primitives/StringBench_Common.h"

using namespace jc_gbench;

#if StringAppendBenchTest == ON

// B5: 1B Append 1024회 (성장 정책)
template <typename TString>
static void BM_String_Append1B(State& state)
{
	for (auto _ : state)
	{
		TString s;
		for (int i = 0; i < 1024; ++i)
			s.Append('a');
		DoNotOptimize(s.Source());
		ClobberMemory();
	}
}

// B6: operator+ 연쇄 5회 (임시 객체)
template <typename TString>
static void BM_String_ConcatChain(State& state)
{
	const TString a("aa"), b("bb"), c("cc"), d("dd"), e("ee");
	for (auto _ : state)
	{
		TString r = a + b + c + d + e;
		DoNotOptimize(r.Source());
		ClobberMemory();
	}
}

// B7: Format
template <typename TString>
static void BM_String_Format(State& state)
{
	for (auto _ : state)
	{
		TString s;
		s.Format("%d %s %f", 12345, "bench", 1.5);
		DoNotOptimize(s.Source());
		ClobberMemory();
	}
}

// B8: Find / Compare (Long 데이터셋)
template <typename TString>
static void BM_String_FindCompare(State& state)
{
	const StrBenchData d = GetStrBenchData(4);
	const TString src(d.p);
	const TString needle("xx");
	for (auto _ : state)
	{
		const int f = src.Find("xx");
		const int c = src.Compare(needle);
		DoNotOptimize(f + c);
		ClobberMemory();
	}
}

#if BenchStrB5 == ON
STR_REG_R0(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S0(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S1(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S2(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S3(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S4(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S5(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S6(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_S7(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
STR_REG_R1(BM_String_Append1B, Unit(benchmark::TimeUnit::kMicrosecond));
#endif

#if BenchStrB6 == ON
STR_REG_R0(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_ConcatChain, Unit(benchmark::TimeUnit::kNanosecond));
#endif

#if BenchStrB7 == ON
STR_REG_R0(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_Format, Unit(benchmark::TimeUnit::kNanosecond));
#endif

#if BenchStrB8 == ON
STR_REG_R0(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_FindCompare, Unit(benchmark::TimeUnit::kNanosecond));
#endif

#endif
