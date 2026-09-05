/*
 * StrForBench: jc::String의 벤치용 복사본이다.
 * jc 본체는 손대지 않고 이 복사본 안에서 후보 구현(S0~S7)을 갈아 성능을 비교한다.
 * PCH(Core.h) 이후에 include되어야 한다.
 */

#pragma once

#include <string>
#include <sstream>
#include <ostream>
#include <cstdio>
#include <cstdarg>
#include <cctype>

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Assert.h"
#include "jc/Ascii.h"
#include "jc/Memory.h"
#include "jc/Exception.h"
#include "jc/Tuple.h"
#include "jc/Allocator/DefaultAllocator.h"
#include "jc/Container/Vector.h"
#include "jc/Primitives/StringView.h"
#include "jc/Primitives/StringUtil.h"
#include "jc_gbench/Primitives/StrForBenchPool.h"

// 후보 식별자 (Phase별 .inl 분기용)
#define STRFORBENCH_S0 0	// 현행 그대로 (baseline)
#define STRFORBENCH_S1 1	// inline SSO 24B
#define STRFORBENCH_S2 2	// StringImpl* 8B
#define STRFORBENCH_S3 3	// + TLS free-list
#define STRFORBENCH_S4 4	// + Impl SSO
#define STRFORBENCH_S5 5	// + owner 비교 / global fallback
#define STRFORBENCH_S6 6	// + deferred queue
#define STRFORBENCH_S7 7	// + Pool Storage

// 한 바이너리에 여러 후보를 공존시키기 위해 네임스페이스별로 .inl을 반복 include한다.
// BenchStrS* 매크로로 컴파일 대상 후보를 고른다.
#if BenchStrS0 == ON
#define STRFORBENCH_STRATEGY STRFORBENCH_S0
#define STRFORBENCH_NS s0
#include "StrForBench.inl"
#undef STRFORBENCH_STRATEGY
#undef STRFORBENCH_NS
#endif

#if BenchStrS1 == ON
#define STRFORBENCH_NS s1
#include "StrForBench_S1.inl"
#undef STRFORBENCH_NS
#endif

#if BenchStrS2 == ON
#define STRFORBENCH_NS s2
#include "StrForBench_S2.inl"
#undef STRFORBENCH_NS
#endif

#if BenchStrS3 == ON
#define STRFORBENCH_NS s3
#include "StrForBench_S3.inl"
#undef STRFORBENCH_NS
#endif

#if BenchStrS4 == ON
#define STRFORBENCH_NS s4
#include "StrForBench_S4.inl"
#undef STRFORBENCH_NS
#endif

#if BenchStrS5 == ON
#define STRFORBENCH_NS s5
#include "StrForBench_S5.inl"
#undef STRFORBENCH_NS
#endif

#if BenchStrS6 == ON
#define STRFORBENCH_NS s6
#define STRFORBENCH_XTHREAD_AUTO_DRAIN 0
#define STRFORBENCH_XTHREAD_STORAGE() benchpool::GlobalXThreadStorageS6()
#include "StrForBench_SXThread.inl"
#undef STRFORBENCH_XTHREAD_STORAGE
#undef STRFORBENCH_XTHREAD_AUTO_DRAIN
#undef STRFORBENCH_NS
#endif

#if BenchStrS7 == ON
#define STRFORBENCH_NS s7
#define STRFORBENCH_XTHREAD_AUTO_DRAIN 1
#define STRFORBENCH_XTHREAD_STORAGE() benchpool::GlobalXThreadStorageS7()
#include "StrForBench_SXThread.inl"
#undef STRFORBENCH_XTHREAD_STORAGE
#undef STRFORBENCH_XTHREAD_AUTO_DRAIN
#undef STRFORBENCH_NS
#endif
