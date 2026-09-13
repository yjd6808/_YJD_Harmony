/*
 * 작성자: 윤정도
 * jc 코루틴 gtest (docs 01~15 단계별 추가)
 */

#include "jc/Threading/Coroutine.h"

#include <thread>
#include <string>

#include <intrin.h>
#include <stdexcept>
#include <float.h>

#if TEST_CoroutineTest == ON

TEST(Coroutine, Error_NullFunction)
{
	CoId id = CoRun(nullptr);
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(CoGetLastError(), coeNullFunction);
	// 읽으면 지워진다. 같은 실패를 두 번 보고하지 않는다.
	EXPECT_EQ(CoGetLastError(), coeNone);
}

static void CoTestFn_Empty(CoContext*) {}

TEST(Coroutine, Error_SuccessClearsError)
{
	CoId id = CoRun(CoTestFn_Empty);
	EXPECT_EQ(id, CO_INVALID_ID);	// 즉시 종료 → 해제 후 무효 id
	EXPECT_EQ(CoGetLastError(), coeNone);
}

static void CoTestFn_Grow10K(CoContext*)
{
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

TEST(Coroutine, Veh_AutoRegistered)
{
	CoId id = CoRun(CoTestFn_Grow10K);
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(CoGetLastError(), coeNone);
	// 풀에 반납된 스택을 해제한다. (~CoMgr가 없어 프로세스 종료 시 릭으로 잡히므로)
	g_cCoMgr.Clear();
}

static volatile bool g_coTestRanOnWrongThread = false;
static void CoTestFn_YieldThenFlag(CoContext*)
{
	CoYield();
	g_coTestRanOnWrongThread = true;
}

TEST(Coroutine, Resume_WrongThread)
{
	g_coTestRanOnWrongThread = false;
	CoId id = CoRun(CoTestFn_YieldThenFlag, {.spec_ = CoStackSpec::Low()});
	ASSERT_NE(id, CO_INVALID_ID);

	bool resumed = false;
	CoError err = coeNone;
	std::thread th([&]
	{
		resumed = CoResume(id);
		err = CoGetLastError();
	});
	th.join();

	EXPECT_FALSE(resumed);
	// 타 스레드 맵에는 id가 안 보이므로 stale로 보고된다. (점프 방지는 동일)
	EXPECT_EQ(err, coeStaleHandle);
	EXPECT_FALSE(g_coTestRanOnWrongThread);

	// 원래 스레드에서 정상 종료시킨다.
	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}

static CoError g_coTestSelfErr = coeNone;
static bool g_coTestSelfResumed = false;
static void CoTestFn_SelfResume(CoContext*)
{
	bool r = CoResume(CoCurrentCtx()->id_);
	g_coTestSelfResumed = r;
	g_coTestSelfErr = CoGetLastError();
}

TEST(Coroutine, Resume_Self)
{
	g_coTestSelfResumed = false;
	CoId id = CoRun(CoTestFn_SelfResume, {.spec_ = CoStackSpec::Low()});
	while (CoResume(id)) {}
	EXPECT_FALSE(g_coTestSelfResumed);
	EXPECT_EQ(g_coTestSelfErr, coeInvalidState);
	g_cCoMgr.Clear();
}

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
	CoContext* pA = nullptr;
	CoId h = CoRun(CoTestFn_YieldOnce, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pA});
	ASSERT_NE(h, CO_INVALID_ID);

	while (CoResume(h)) {}	// 끝까지 돌려 풀에 반납

	// 같은 티어로 다시 시작하면 풀에서 같은 주소가 재사용된다.
	CoContext* pB = nullptr;
	CoId hB = CoRun(CoTestFn_B, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pB});
	ASSERT_NE(hB, CO_INVALID_ID);
	EXPECT_EQ(pB, pA);	// 재사용 확인

	// 예전 id는 죽은 id다. B가 다시 실행되면 안 된다.
	g_coTestB_Resumed = false;
	EXPECT_FALSE(CoResume(h));
	EXPECT_EQ(CoGetLastError(), coeStaleHandle);
	EXPECT_FALSE(g_coTestB_Resumed);

	while (CoResume(hB)) {}
	g_cCoMgr.Clear();
}

// 작은 custom 요청은 티어로 올림되고 크기도 티어 크기가 된다.
static void CoTestFn_YieldForever06(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Custom_RoundUp)
{
	g_cCoMgr.Clear();
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_YieldForever06, {.spec_ = CoStackSpec::Custom(5000), .ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_EQ(pView->stack_.stackTier_, cstLow);
	EXPECT_EQ(pView->stack_.size_, (_u32)CO_STACK_SIZE_LOW);
	EXPECT_EQ(((uintptr_t)pView->stack_.pStackBase_ & (CO_PAGE_SIZE - 1)), 0u);
	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}

// 크기가 0인 custom은 만들 수 없다.
TEST(Coroutine, Custom_ZeroRejected)
{
	CoId id = CoRun(CoTestFn_YieldForever06, {.spec_ = CoStackSpec::Custom(0)});
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(CoGetLastError(), coeInvalidStackSize);
}

// custom 5000B가 풀을 오염시키지 않는다.
// - 6KB를 쓰는 이유: 5000B 스택에는 절대 안 들어가고, 진짜 16KB Low 스택의
//   초기 커밋(8KB)에는 들어가므로 VEH 확장을 타지 않고 판별된다.
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
	CoId id = CoRun(CoTestFn_YieldForever06, {.spec_ = CoStackSpec::Custom(5000)});
	while (CoResume(id)) {}

	// 같은 풀에서 꺼낸 Low 스택으로 6KB를 써도 멀쩡해야 한다. (5000B였으면 오버플로우)
	id = CoRun(CoTestFn_Touch6K, {.spec_ = CoStackSpec::Low()});
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(CoGetLastError(), coeNone);
	g_cCoMgr.Clear();
}

// 진짜 큰 custom은 페이지 올림되고 풀에 들어가지 않는다.
TEST(Coroutine, Custom_LargeNoPool)
{
	g_cCoMgr.Clear();
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_YieldForever06, {.spec_ = CoStackSpec::Custom(300000), .ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_EQ(pView->stack_.stackTier_, cstCustom);
	EXPECT_EQ(pView->stack_.size_, ((_u32)300000 + CO_PAGE_SIZE - 1) & ~(_u32)(CO_PAGE_SIZE - 1));
	char* pCustomBase = pView->stack_.pStackBase_;
	while (CoResume(id)) {}

	// custom은 해제되고 풀에 남지 않으므로 예약 영역이 반환되어야 한다.
	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pCustomBase - 1, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_FREE);

	// 다음 Low 요청이 custom 스택 주소를 받으면 안 된다.
	CoContext* pLowView = nullptr;
	id = CoRun(CoTestFn_YieldForever06, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pLowView});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_NE(pLowView->stack_.pStackBase_, pCustomBase);
	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}

// fn 안에서 던진 예외는 보관 후 꺼내 받을 수 있다.
static void CoTestFn_Throw(CoContext*)
{
	throw std::runtime_error("co-boom");
}

TEST(Coroutine, Exception_StoredAndTaken)
{
	CoId id = CoRun(CoTestFn_Throw);
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(CoGetLastError(), coeException);
	bool caught = false;
	try
	{
		CoTakePendingException();
	}
	catch (const std::runtime_error&)
	{
		caught = true;
	}
	EXPECT_TRUE(caught);
	CoClearPendingException();	// 보관 소유권 정리 (take 뒤 catch가 끝난 자리)
	g_cCoMgr.Clear();
}

static std::string g_coCaughtWhat;
static void CoTestOnException(CoId, std::exception_ptr _ex)
{
	try
	{
		if (_ex)
			std::rethrow_exception(_ex);
	}
	catch (const std::runtime_error& ex)
	{
		g_coCaughtWhat = ex.what();
	}
}

TEST(Coroutine, Exception_OnExceptionCallback)
{
	g_coCaughtWhat.clear();
	CoId id = CoRun(CoTestFn_Throw, {.onException_ = CoTestOnException});
	EXPECT_EQ(id, CO_INVALID_ID);
	EXPECT_EQ(g_coCaughtWhat, "co-boom");
	// 소비형 전달이라 보관에 남지 않는다.
	EXPECT_FALSE(CoTakePendingException());
	g_cCoMgr.Clear();
}

// 코루틴 안에서 보는 StackBase/Limit은 코루틴 스택을 가리킨다.
// - DeallocationStack은 비상 밴드 상단(pEmergencyTop_)이 설치된다.
//   (예약 하단은 커널이 직접 확장해 VEH가 안 불리므로 경계를 위로 둔다)
static void* g_coTebVals[3] = { nullptr, nullptr, nullptr };
static void* g_coTebBase = nullptr;
static void* g_coTebEmergencyTop = nullptr;
static void CoTestFn_ReadTeb(CoContext* pCtx)
{
	g_coTebVals[0] = (void*)__readgsqword(0x08);
	g_coTebVals[1] = (void*)__readgsqword(0x10);
	g_coTebVals[2] = (void*)__readgsqword(0x1478);
	g_coTebBase = pCtx->stack_.pStackBase_;
	g_coTebEmergencyTop = pCtx->stack_.pEmergencyTop_;
}

TEST(Coroutine, Teb_StackRangesInside)
{
	CoId id = CoRun(CoTestFn_ReadTeb);
	while (CoResume(id)) {}
	EXPECT_EQ(g_coTebVals[0], g_coTebBase);
	EXPECT_EQ(g_coTebVals[2], g_coTebEmergencyTop);
	g_cCoMgr.Clear();
}

// 성장 가드존은 PAGE_GUARD다. (NOACCESS면 예외 배달이 죽으므로)
// - DeallocationStack 교체와 조합 시 커널이 직접 확장해 VEH가 안 불릴 수 있어,
//   Teb_StackLimitAfterGrowth 테스트에서 VEH 확장이 도는지 함께 감시한다.
TEST(Coroutine, GuardZone_IsPageGuard)
{
	g_cCoMgr.Clear();
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_YieldForever06, {.ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);
	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pView->stack_.pGuardLimit_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_NE(mbi.Protect & PAGE_GUARD, 0u);
	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}

// 확장 후에도 throw/catch가 되고, yield를 거치면 한계가 동기화된다.
// - 한계 동기화는 yield 시점에 살아있는 TEB 값을 기준으로 맞춘다.
//   (커널이 가드 폴트를 직접 확장할 수 있어 pStackLimit_만 믿을 수 없기 때문)
static bool g_coTestLimitMatch = false;
static bool g_coTestCaught = false;
static void CoTestFn_ThrowAfterGrow(CoContext*)
{
	volatile char big[CO_PAGE_SIZE * 4];
	for (int i = (int)sizeof(big) - 1; i >= 0; i -= CO_PAGE_SIZE)
		big[i] = (char)i;
	CoYield();	// 여기서 살아있는 한계가 stack_에 동기화된다.
	void* limitNow = (void*)__readgsqword(0x10);
	CoContext* self = CoCurrentCtx();
	g_coTestLimitMatch = (limitNow == self->stack_.pStackLimit_);
	try { throw 42; }
	catch (int v) { g_coTestCaught = (v == 42); }
	(void)big;
}

TEST(Coroutine, Teb_StackLimitAfterGrowth)
{
	g_coTestLimitMatch = false;
	g_coTestCaught = false;
	CoId id = CoRun(CoTestFn_ThrowAfterGrow);
	while (CoResume(id)) {}
	EXPECT_TRUE(g_coTestLimitMatch);
	EXPECT_TRUE(g_coTestCaught);
	g_cCoMgr.Clear();
}

// 코루틴이 바꾼 반올림 모드가 yield/resume을 넘어 유지되고,
// 스레드 쪽에는 새지 않는다. (x87 CW도 같은 경로로 교체되므로 MXCSR로 대표 검증)
static unsigned g_coMxcsrInside = 0;
static void CoTestFn_Rounding(CoContext*)
{
	unsigned old = 0;
	::_controlfp_s(&old, _RC_DOWN, _MCW_RC);
	CoYield();
	unsigned now = 0;
	::_controlfp_s(&now, 0, 0);
	g_coMxcsrInside = now;
}

TEST(Coroutine, Abi_MxcsrPreserved)
{
	CoId id = CoRun(CoTestFn_Rounding);
	ASSERT_NE(id, CO_INVALID_ID);
	unsigned mid = 0;
	::_controlfp_s(&mid, 0, 0);
	EXPECT_EQ(mid & _MCW_RC, (unsigned)_RC_NEAR);	// 스레드 쪽은 그대로
	while (CoResume(id)) {}
	EXPECT_EQ(g_coMxcsrInside & _MCW_RC, (unsigned)_RC_DOWN);	// 코루틴 쪽은 유지
	unsigned after = 0;
	::_controlfp_s(&after, 0, 0);
	EXPECT_EQ(after & _MCW_RC, (unsigned)_RC_NEAR);	// 종료 후 스레드 복원
	g_cCoMgr.Clear();
}

// 스택 오버플로우가 __except에 닿고, 표시·복구가 된다.
// - 예약 아래 비상 패드가 SEH 디스패치 공간을 보장한다.
// - 확립된 프레임에서 직접 찍으면 잡힌다. (__chkstk/RTC-fill처럼 RSP가 따라
//   내려가는 프로브는 배달 공간·언와인드 문제로 죽을 수 있어 별도 검증 대상)
static int CoTestFilterSO(unsigned _code)
{
	return _code == EXCEPTION_STACK_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

// yield 시점에 살아있는 한계가 stack_에 동기화된다.
// - 커널이 가드 폴트를 직접 확장할 수 있어 pStackLimit_만 믿을 수 없으므로,
//   yield 때 TEB 값을 기준으로 맞추고 resume은 이 값을 읽는다.
// - 우리 ExpandStack이 돌았으면 guard까지 내려가 있고, 커널이 먹었으면
//   guard는 그대로다. (limit은 yield 동기화로 내려감)
static char* g_coSyncLimitAtYield = nullptr;
static char* g_coSyncLimitInitial = nullptr;
static char* g_coSyncGuardAtYield = nullptr;
static char* g_coSyncGuardInitial = nullptr;
static void CoTestFn_SyncGrowYield(CoContext* pCtx)
{
	g_coSyncLimitInitial = pCtx->stack_.pStackLimit_;
	g_coSyncGuardInitial = pCtx->stack_.pGuardLimit_;
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
	CoYield();
	g_coSyncLimitAtYield = CoCurrentCtx()->stack_.pStackLimit_;
	g_coSyncGuardAtYield = CoCurrentCtx()->stack_.pGuardLimit_;
	(void)buf;
}

TEST(Coroutine, Growth_YieldSyncsLimit)
{
	g_cCoMgr.Clear();
	CoId id = CoRun(CoTestFn_SyncGrowYield);
	while (CoResume(id)) {}
	// 한계는 살아있는 값으로 동기화된다. 가드는 우리 ExpandStack이 안 돌아서 그대로다.
	EXPECT_NE(g_coSyncLimitAtYield, g_coSyncLimitInitial);
	EXPECT_EQ(g_coSyncGuardAtYield, g_coSyncGuardInitial);
	g_cCoMgr.Clear();
}

static bool g_coOverflowCaught = false;
static bool g_coOverflowFlag = false;
static bool g_coOverflowReset = false;

// 확립된 프레임에서 오버플로우 가드를 직접 찍으면 잡힌다.
// - __chkstk/RTC-fill처럼 RSP가 따라 내려가는 프로브는 배달 공간이 없어 죽지만,
//   직접 인덱스로 찍으면 RSP가 위에 있어 디스패치가 된다.
__declspec(noinline) static void CoTestTouchDeep(volatile char* _p)
{
	*_p = 42;
}

static void CoTestFn_DirectTouch(CoContext* pCtx)
{
	__try
	{
		// 오버플로우 가드 페이지를 직접 찍는다. (RSP는 위에 그대로)
		volatile char* pGuard = (volatile char*)pCtx->stack_.pStackEnd_;
		CoTestTouchDeep(pGuard);
	}
	__except (CoTestFilterSO(GetExceptionCode()))
	{
		g_coOverflowCaught = true;
		CoNoteStackOverflow();
		g_coOverflowFlag = CoCurrentCtx()->stack_.overflowed_;
		g_coOverflowReset = CoResetStackOverflow();
	}
}

TEST(Coroutine, Overflow_DirectTouchCaught)
{
	g_coOverflowCaught = false;
	g_coOverflowFlag = false;
	g_coOverflowReset = false;
	g_cCoMgr.Clear();
	CoId id = CoRun(CoTestFn_DirectTouch);
	while (CoResume(id)) {}
	EXPECT_TRUE(g_coOverflowCaught);
	EXPECT_TRUE(g_coOverflowFlag);
	EXPECT_TRUE(g_coOverflowReset);
	g_cCoMgr.Clear();
}

// 스레드가 코루틴을 쓰고 종료해도 크래시가 없다.
// - TLS 소멸 중/이후 가드 폴트가 오면 CoVEH가 매니저를 보지 않는다.
// - jc::Thread는 생성된 스레드에서 ThreadLocal 맵 릭을 남기므로
//   std::thread로 검증한다.
TEST(Coroutine, Veh_ThreadExitDuringClear)
{
	std::thread th([]
	{
		CoId id = CoRun(CoTestFn_YieldForever06);
		while (CoResume(id)) {}
		g_cCoMgr.Clear();
	});
	th.join();
	EXPECT_TRUE(true);
}

// VEH 통계 API가 동작한다.
// - 커널이 성장을 직접 처리하면 우리 VEH가 안 불려 0일 수 있다.
//   0이 잘못됐다는 뜻이 아니라 안 불렸다는 뜻이다.
TEST(Coroutine, Veh_StatsSmoke)
{
	CoMgr::CoVehStats st = g_cCoMgr.GetVehStats();
	EXPECT_TRUE(st.maxDispatchUsed_ < 64 * 1024);
	EXPECT_TRUE(true);
}

// 슬랩에 패킹된다. Low 100개가 소수 예약에 들어간다.
static void CoTestFn_YieldForever13(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Slab_PacksReservations)
{
	g_cCoMgr.Clear();
	const int N = 100;
	CoId ids[100];
	CoContext* views[100];
	for (int i = 0; i < N; ++i)
	{
		ids[i] = CoRun(CoTestFn_YieldForever13, {.spec_ = CoStackSpec::Low(), .ppOut_ = &views[i]});
		ASSERT_NE(ids[i], CO_INVALID_ID);
	}

	// 1MB 단위로 묶인다. 서로 다른 슬랩 개수를 센다.
	uintptr_t slabs[100];
	int slabCount = 0;
	for (int i = 0; i < N; ++i)
	{
		uintptr_t slabBase = (uintptr_t)views[i]->stack_.pReserveBase_ & ~((uintptr_t)0x100000 - 1);
		bool found = false;
		for (int j = 0; j < slabCount; ++j)
		{
			if (slabs[j] == slabBase)
			{
				found = true;
				break;
			}
		}
		if (!found)
			slabs[slabCount++] = slabBase;
	}
	// Low 슬롯 20KB → 1MB에 51개. 100개면 슬랩 2개면 된다. (여유 있게 4개 이하)
	EXPECT_LE(slabCount, 4);

	for (int i = 0; i < N; ++i)
	{
		while (CoResume(ids[i])) {}
	}
	g_cCoMgr.Clear();
}

// 이웃 슬롯이 붙어 있어도 각자 동작한다. (경계는 바닥 가드가 담당)
TEST(Coroutine, Slab_NeighborIsolation)
{
	g_cCoMgr.Clear();
	CoContext* pAView = nullptr;
	CoContext* pBView = nullptr;
	CoId a = CoRun(CoTestFn_YieldForever13, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pAView});
	CoId b = CoRun(CoTestFn_YieldForever13, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pBView});
	ASSERT_NE(a, CO_INVALID_ID);
	ASSERT_NE(b, CO_INVALID_ID);
	// 같은 슬랩에 이웃하게 들어갔는지 확인한다. (슬롯 단위 연속)
	ptrdiff_t diff = pAView->stack_.pStackBase_ > pBView->stack_.pStackBase_
		? pAView->stack_.pStackBase_ - pBView->stack_.pStackBase_
		: pBView->stack_.pStackBase_ - pAView->stack_.pStackBase_;
	EXPECT_EQ(diff, (ptrdiff_t)(CO_STACK_SIZE_LOW + CO_PAGE_SIZE));	// 16KB + 패드 1장
	while (CoResume(a)) {}
	while (CoResume(b)) {}
	g_cCoMgr.Clear();
}
// init이 전체를 덮어도(Eager) 반납이 죽지 않는다.
static void CoTestFn_YieldForever12Eager(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Recycle_ExtremeInitCount)
{
	g_cCoMgr.Clear();
	g_cCoMgr.SetPageInitCount(CO_STACK_PAGE_COUNT_HIGH);	// High 전체 선커밋
	CoId id = CoRun(CoTestFn_YieldForever12Eager, {.spec_ = CoStackSpec::High()});
	while (CoResume(id)) {}	// 여기서 RecycleStack이 돈다. 죽으면 안 됨.
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.Clear();
}
// - 이전에는 반납 때 전체 decommit + 재commit이라 생성 1회에 수 µs가 들었다.
static void CoTestFn_YieldForever12(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Pool_NoRecommitOnReuse)
{
	g_cCoMgr.Clear();
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_YieldForever12, {.ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);
	char* pBase = pView->stack_.pStackBase_;
	while (CoResume(id)) {}	// 풀로 반납. 커밋 유지되어야 함.

	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pBase - CO_PAGE_SIZE, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);	// decommit됐으면 FREE/RESERVE다.

	// 같은 풀에서 꺼내면 같은 주소가 재사용되고 바로 동작한다.
	CoContext* pView2 = nullptr;
	id = CoRun(CoTestFn_YieldForever12, {.ppOut_ = &pView2});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_EQ(pView2->stack_.pStackBase_, pBase);
	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}
// - 예약 아래 패드 N페이지 RW 커밋, 오버플로우 가드 1페이지 GUARD,
//   DeallocationStack = 밴드 상단.
TEST(Coroutine, Overflow_LayoutCheck)
{
	g_cCoMgr.Clear();
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_YieldForever06, {.ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);

	MEMORY_BASIC_INFORMATION mbi{};
	// 패드: 예약 시작 주소는 RW 커밋이어야 한다.
	EXPECT_EQ(::VirtualQuery(pView->stack_.pReserveBase_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_EQ(mbi.Protect & 0xFF, (DWORD)PAGE_READWRITE);
	// 오버플로우 가드: GUARD여야 한다. (배달 push가 자동 해제)
	EXPECT_EQ(::VirtualQuery(pView->stack_.pStackEnd_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_NE(mbi.Protect & PAGE_GUARD, 0u);
	// 패드 크기: Mid(16p)는 4장이다.
	EXPECT_EQ(pView->stack_.pStackEnd_ - pView->stack_.pReserveBase_, (ptrdiff_t)(4 * CO_PAGE_SIZE));
	// 밴드 상단 = 예약 하단 + 1페이지다.
	EXPECT_EQ(pView->stack_.pEmergencyTop_, pView->stack_.pStackEnd_ + CO_PAGE_SIZE);

	while (CoResume(id)) {}
	g_cCoMgr.Clear();
}

// userData로 전역 없이 데이터를 넘긴다.
static void CoTestFn_UserData(CoContext* pCtx)
{
	int* pAcc = (int*)pCtx->userData_;
	*pAcc += 1;
	CoYield();
	*pAcc += 10;
}

TEST(Coroutine, Api_UserData)
{
	int acc = 0;
	CoId id = CoRun(CoTestFn_UserData, {.userData_ = &acc});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_EQ(acc, 1);
	while (CoResume(id)) {}
	EXPECT_EQ(acc, 11);
	g_cCoMgr.Clear();
}

// 값 채널로 양방향 값을 주고받는다.
// - CoYield(내보내기)는 다음 resume 때 들어오는 값을 돌려주고,
//   CoResume(넣기)는 재개 뒤 코루틴이 마지막에 낸 값을 돌려준다.
// - 첫 yield 값은 resume 전에 switchData_에서 직접 읽는다.
static unsigned g_coEchoGot = 0;
static void CoTestFn_Echo(CoContext*)
{
	_u64 v = CoYield((_u64)100);
	g_coEchoGot = (unsigned)v;
	CoYield((_u64)200);
}

TEST(Coroutine, Api_Transfer)
{
	g_coEchoGot = 0;
	CoContext* pView = nullptr;
	CoId id = CoRun(CoTestFn_Echo, {.ppOut_ = &pView});
	ASSERT_NE(id, CO_INVALID_ID);
	EXPECT_EQ(pView->switchData_, (_u64)100);	// 첫 yield 값
	_u64 out = 0;
	EXPECT_TRUE(CoResume(id, (_u64)111, &out));
	EXPECT_EQ(out, (_u64)200);	// 두 번째 yield 값
	EXPECT_EQ(g_coEchoGot, (unsigned)111);	// 코루틴이 111을 받았다
	EXPECT_FALSE(CoResume(id, (_u64)222, &out));	// fn 종료
	EXPECT_EQ(out, (_u64)0);
	g_cCoMgr.Clear();
}

// 람다 캡처로 코루틴을 돌린다.
TEST(Coroutine, Api_Lambda)
{
	int acc = 0;
	CoId id = CoRunFn([&](CoContext*)
	{
		for (int i = 0; i < 3; ++i)
		{
			acc += i;
			CoYield();
		}
	});
	while (CoResume(id)) {}
	EXPECT_EQ(acc, 3);
	g_cCoMgr.Clear();
}

// CoScope로 협력적 취소를 한다.
static bool g_coCancelSeen = false;
static void CoTestFn_Cancel(CoContext*)
{
	while (!CoCancelRequested())
		CoYield();
	g_coCancelSeen = true;
}

TEST(Coroutine, Api_ScopedCancel)
{
	g_coCancelSeen = false;
	{
		CoScope sc(CoRun(CoTestFn_Cancel));
		EXPECT_TRUE(sc.Resume());
		sc.Cancel();
		EXPECT_FALSE(CoResume(sc.Id()));	// 정리 후 id는 죽은 id다
		EXPECT_EQ(CoGetLastError(), coeStaleHandle);
	}
	EXPECT_TRUE(g_coCancelSeen);
	g_cCoMgr.Clear();
}

#endif // TEST_CoroutineTest == ON
