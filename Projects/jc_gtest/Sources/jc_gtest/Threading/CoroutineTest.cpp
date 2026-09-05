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

// [코루틴-06] 작은 custom 요청은 티어로 올림되고 크기도 티어 크기가 된다.
// - 이전에는 티어만 바뀌고 크기가 5000B 그대로라 정렬이 깨지고 풀이 오염됐다.
static void CoTestFn_YieldForever06(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Custom_RoundUp)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 5000);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->stack_.stackTier_, cstLow);
	EXPECT_EQ(pCtx->stack_.size_, (_u32)CO_STACK_SIZE_LOW);
	EXPECT_EQ(((uintptr_t)pCtx->stack_.pStackBase_ & (CO_PAGE_SIZE - 1)), 0u);
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-06] 크기가 0인 custom은 만들 수 없다.
TEST(Coroutine, Custom_ZeroRejected)
{
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 0);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeInvalidStackSize);
}

// [코루틴-06] custom 5000B가 풀을 오염시키지 않는다.
// - 이전에는 5000B 스택이 cstLow 풀에 들어가 다음 사용자가 좁은 스택을 받았다.
// - 6KB를 쓰는 이유: 5000B 스택에는 절대 안 들어가고, 진짜 16KB Low 스택의
//   초기 커밋(8KB)에는 들어가므로 VEH 확장을 타지 않고 판별된다.
//   (Low에서 10KB 같은 성장은 VEH 예산 부족으로 죽을 수 있어 15에서 다룬다.)
static void CoTestFn_Touch6K(CoContext*)
{
	volatile char buf[6 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

TEST(Coroutine, Custom_NoPoolMix)
{
	g_cCoMgr.Clear();
	// 5000B custom → Low로 올림 → 종료 → 풀에 반납
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 5000);
	while (pCtx)
		pCtx = CoResume(pCtx);

	// 같은 풀에서 꺼낸 Low 스택으로 6KB를 써도 멀쩡해야 한다. (5000B였으면 오버플로우)
	pCtx = CoRun(CoTestFn_Touch6K, cstLow);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNone);
	g_cCoMgr.Clear();
}

// [코루틴-06] 진짜 큰 custom은 페이지 올림되고 풀에 들어가지 않는다.
TEST(Coroutine, Custom_LargeNoPool)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 300000);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->stack_.stackTier_, cstCustom);
	EXPECT_EQ(pCtx->stack_.size_, ((_u32)300000 + CO_PAGE_SIZE - 1) & ~(_u32)(CO_PAGE_SIZE - 1));
	char* pCustomBase = pCtx->stack_.pStackBase_;
	while (pCtx)
		pCtx = CoResume(pCtx);

	// custom은 해제되고 풀에 남지 않으므로 예약 영역이 반환되어야 한다.
	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pCustomBase - 1, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_FREE);

	// 다음 Low 요청이 custom 스택 주소를 받으면 안 된다.
	pCtx = CoRun(CoTestFn_YieldForever06, cstLow);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_NE(pCtx->stack_.pStackBase_, pCustomBase);
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

#endif // TEST_CoroutineTest == ON
