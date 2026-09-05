/*
 * 작성자: 윤정도
 * jc 코루틴 gtest (docs 01~15 단계별 추가)
 */

#include "jc/Threading/Coroutine.h"

#if TEST_CoroutineTest == ON

// [코루틴-07] fn이 null이면 종료가 아니라 실패이므로 원인이 보여야 한다.
TEST(Coroutine, Error_NullFunction)
{
	CoContext* pCtx = CoRun(nullptr, cstMid);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNullFunction);
	// 읽으면 지워진다. 같은 실패를 두 번 보고하지 않는다.
	EXPECT_EQ(CoGetLastError(), coeNone);
}

// [코루틴-07] 성공 경로에서는 에러가 남지 않는다.
static void CoTestFn_Empty(CoContext*) {}

TEST(Coroutine, Error_SuccessClearsError)
{
	CoContext* pCtx = CoRun(CoTestFn_Empty, cstMid);
	EXPECT_EQ(pCtx, nullptr);	// 즉시 종료 → 해제 후 nullptr
	EXPECT_EQ(CoGetLastError(), coeNone);
}

// [코루틴-07] VEH를 직접 등록하지 않아도 스택 확장이 동작한다.
// - 이전에는 AddVectoredExceptionHandler를 빼먹으면 여기서 종료됐다.
static void CoTestFn_Grow10K(CoContext*)
{
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

TEST(Coroutine, Veh_AutoRegistered)
{
	CoContext* pCtx = CoRun(CoTestFn_Grow10K, cstMid);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNone);
	// 풀에 반납된 스택을 해제한다. (~CoMgr가 없어 프로세스 종료 시 릭으로 잡히므로)
	g_cCoMgr.Clear();
}

#endif // TEST_CoroutineTest == ON
