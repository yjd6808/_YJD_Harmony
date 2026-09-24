#pragma once

#include "sgf/PCH/Core.h"
#include "sgf/SgfHeader.h"

#pragma warning(push, 0)
#include <benchmark/benchmark.h>
#pragma warning(pop)

USING_NS_JC;
USING_NS_SGF;
USING_NS_STD;
USING_NS_BENCHMARK;

#define ON 1
#define OFF 0

#define TransformBench ON
#define PrimitiveBuilderBench ON
