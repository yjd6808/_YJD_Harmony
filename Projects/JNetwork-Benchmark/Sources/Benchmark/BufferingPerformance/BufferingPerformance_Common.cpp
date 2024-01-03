/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include "BufferingPerformance_Common.h"

NS_BUFFERING_PERFORMANCE_BEGIN
int TestClientCount;
int TestSendCount;
AutoResetEvent TestFinished(false);
NS_BUFFERING_PERFORMANCE_END