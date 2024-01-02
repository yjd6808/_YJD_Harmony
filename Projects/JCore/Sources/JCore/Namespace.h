/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 2:44:41 PM
 * =====================
 *
 */

#pragma once

#define NS_STD_BEGIN	namespace std {
#define NS_STD_END		}

#define NS_JC_BEGIN		namespace JCore {
#define NS_JC_END		}

#define NS_DETAIL_BEGIN	namespace Detail {
#define NS_DETAIL_END	}

#define NS_BENCHMARK_BEGIN		namespace benchmark {
#define NS_BENCHMARK_END		}

#define USING_NS_JC			using namespace JCore
#define USING_NS_JC_DETAIL	using namespace JCore::Detail
#define USING_NS_DETAIL		using namespace Detail


#define USING_NS_STD		using namespace std
#define USING_NS_STD_CHRONO using namespace std::chrono
#define USING_NS_BENCHMARK  using namespace benchmark


