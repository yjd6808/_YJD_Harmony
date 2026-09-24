#include "sgf/Scene/Transform.h"

#if TransformBench == ON

static void BM_TransformGetWorldMatrix(State& state)
{
	sgf::Transform t(nullptr);
	t.SetLocalPosition(jc::vec3(1.0f, 2.0f, 3.0f));
	t.SetLocalScale(jc::vec3(2.0f, 2.0f, 2.0f));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(t.GetWorldMatrix());
	}
}
BENCHMARK(BM_TransformGetWorldMatrix);

static void BM_TransformRebuildLocal(State& state)
{
	sgf::Transform t(nullptr);
	for (auto _ : state)
	{
		state.PauseTiming();
		t.SetLocalPosition(jc::vec3((float)state.iterations(), 0.0f, 0.0f));
		state.ResumeTiming();
		benchmark::DoNotOptimize(t.GetWorldMatrix());
	}
}
BENCHMARK(BM_TransformRebuildLocal);

#endif
