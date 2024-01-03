/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <Benchmark/Core.h>
#include "BufferingWithNagle_Common.h"

NS_BUFFERING_WITH_NAGLE_BEGIN
int TestClientCount;
int TestSendCount;
bool AsyncSending = false;
AutoResetEvent TestFinished(false);
NS_BUFFERING_WITH_NAGLE_END