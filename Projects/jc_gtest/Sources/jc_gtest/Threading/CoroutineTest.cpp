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

// [코루틴-05] 다른 스레드에서 resume하면 실행되지 않고 에러가 남는다.
// - 이전에는 남의 스레드 매니저에 엉뚱한 컨텍스트가 등록돼 풀이 오염됐다.
static volatile bool g_coTestRanOnWrongThread = false;
static void CoTestFn_YieldThenFlag(CoContext*)
{
	CoYield();
	g_coTestRanOnWrongThread = true;
}

TEST(Coroutine, Resume_WrongThread)
{
	g_coTestRanOnWrongThread = false;
	CoContext* pCtx = CoRun(CoTestFn_YieldThenFlag, cstLow);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->state_, csYield);

	bool resumed = false;
	CoError err = coeNone;
	std::thread th([&]
	{
		CoContext* r = CoResume(pCtx);
		resumed = (r != nullptr);
		err = CoGetLastError();
	});
	th.join();

	EXPECT_FALSE(resumed);
	EXPECT_EQ(err, coeWrongThread);
	EXPECT_FALSE(g_coTestRanOnWrongThread);

	// 원래 스레드에서 정상 종료시킨다.
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-05] 자기 자신을 resume하면 스택이 오염되므로 거부된다.
static CoError g_coTestSelfErr = coeNone;
static bool g_coTestSelfResumed = false;
static void CoTestFn_SelfResume(CoContext*)
{
	CoContext* r = CoResume(CoCurrentCtx());
	g_coTestSelfResumed = (r != nullptr);
	g_coTestSelfErr = CoGetLastError();
}

TEST(Coroutine, Resume_Self)
{
	g_coTestSelfResumed = false;
	CoContext* pCtx = CoRun(CoTestFn_SelfResume, cstLow);
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_FALSE(g_coTestSelfResumed);
	EXPECT_EQ(g_coTestSelfErr, coeInvalidState);
	g_cCoMgr.Clear();
}

// [코루틴-05] 종료 후 같은 주소로 재사용된 컨텍스트를 예전 핸들로 resume하면 거부된다.
// - 이전에는 엉뚱한 코루틴이 실행되는 ABA 문제가 있었다.
static void CoTestFn_YieldOnce(CoContext*)
{
	CoYield();
}

static volatile bool g_coTestB_Resumed = false;
static void CoTestFn_B(CoContext*)
{
	CoYield();
	g_coTestB_Resumed = true;
}

TEST(Coroutine, Resume_StaleHandle)
{
	g_cCoMgr.Clear();	// 풀을 비워 같은 주소 재사용을 확정한다.
	CoHandle h = CoRunH(CoTestFn_YieldOnce, cstLow);
	ASSERT_TRUE(h.IsAlive());

	while (CoResumeH(h) != nullptr) {}	// 끝까지 돌려 풀에 반납
	EXPECT_FALSE(h.IsAlive());

	// 같은 티어로 다시 시작하면 풀에서 같은 주소가 재사용된다.
	CoHandle hB = CoRunH(CoTestFn_B, cstLow);
	ASSERT_TRUE(hB.IsAlive());
	EXPECT_EQ(hB.pCtx, h.pCtx);	// 재사용 확인

	// 예전 핸들은 세대가 달라 죽은 핸들이다. B가 다시 실행되면 안 된다.
	g_coTestB_Resumed = false;
	CoContext* r = CoResumeH(h);
	EXPECT_EQ(r, nullptr);
	EXPECT_EQ(CoGetLastError(), coeStaleHandle);
	EXPECT_FALSE(g_coTestB_Resumed);

	while (CoResumeH(hB) != nullptr) {}
	g_cCoMgr.Clear();
}

#endif // TEST_CoroutineTest == ON
