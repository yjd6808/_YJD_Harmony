#include "Core.h"
#include "jc_gbench/Primitives/StringBench_Common.h"

using namespace jc_gbench;

#if StringLifetimeBenchTest == ON

// B1: 생성/소멸 (Arg = 데이터셋 인덱스)
template <typename TString>
static void BM_String_Construct(State& state)
{
	const StrBenchData d = GetStrBenchData((int)state.range(0));
	for (auto _ : state)
	{
		TString s(d.p);
		DoNotOptimize(s.Source());
		ClobberMemory();
	}
}

// B2: 복사 생성
template <typename TString>
static void BM_String_CopyConstruct(State& state)
{
	const StrBenchData d = GetStrBenchData((int)state.range(0));
	const TString src(d.p);
	for (auto _ : state)
	{
		TString dst(src);
		DoNotOptimize(dst.Source());
		ClobberMemory();
	}
}

// B3: 복사 대입 (두 번째부터 기존 capacity 재사용)
template <typename TString>
static void BM_String_CopyAssign(State& state)
{
	const StrBenchData d = GetStrBenchData((int)state.range(0));
	const TString src(d.p);
	TString dst;
	for (auto _ : state)
	{
		dst = src;
		DoNotOptimize(dst.Source());
		ClobberMemory();
	}
}

// B4: 이동
template <typename TString>
static void BM_String_Move(State& state)
{
	const StrBenchData d = GetStrBenchData((int)state.range(0));
	for (auto _ : state)
	{
		TString src(d.p);
		TString dst(static_cast<TString&&>(src));
		DoNotOptimize(dst.Source());
		ClobberMemory();
	}
}

#if BenchStrB1 == ON
STR_REG_R0(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_Construct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
#endif

#if BenchStrB2 == ON
STR_REG_R0(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_CopyConstruct, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
#endif

#if BenchStrB3 == ON
STR_REG_R0(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_CopyAssign, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
#endif

#if BenchStrB4 == ON
STR_REG_R0(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_Move, DenseRange(0, 6)->Unit(benchmark::TimeUnit::kNanosecond));
#endif

#endif
