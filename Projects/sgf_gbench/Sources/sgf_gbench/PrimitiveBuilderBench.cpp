#include "sgf/Graphics/PrimitiveBuilder.h"

#if PrimitiveBuilderBench == ON

static void BM_BuildCircle(State& state)
{
	for (auto _ : state)
	{
		state.PauseTiming();
		sgf::FillResult out;
		state.ResumeTiming();
		sgf::PrimitiveBuilder::BuildCircle(jc::vec2(0.0f, 0.0f), 50.0f, jc::color::WHITE, 32, out);
		benchmark::DoNotOptimize(out.vertices_.Size());
	}
}
BENCHMARK(BM_BuildCircle);

static void BM_BuildRect(State& state)
{
	for (auto _ : state)
	{
		state.PauseTiming();
		sgf::FillResult out;
		state.ResumeTiming();
		sgf::PrimitiveBuilder::BuildRect(jc::rect(0.0f, 0.0f, 100.0f, 100.0f), jc::color::WHITE, out);
		benchmark::DoNotOptimize(out.vertices_.Size());
	}
}
BENCHMARK(BM_BuildRect);

#endif
