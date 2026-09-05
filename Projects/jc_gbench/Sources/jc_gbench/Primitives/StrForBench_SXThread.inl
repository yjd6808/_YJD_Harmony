/*
 * StrForBench_SXThread.inl: S6/S7 공용 cross-thread 후보 본문 (8B 핸들).
 * 헤더 가드 없음. StrForBench.h에서 네임스페이스 s6/s7로 두 번 include한다.
 * STRFORBENCH_XTHREAD_AUTO_DRAIN이 0이면 S6(명시적 pump),
 * 0이 아니면 S7(Acquire 때 자동 drain)이다.
 * STRFORBENCH_XTHREAD_STORAGE()는 사용할 보관소를 지정한다.
 * 클래스 본문은 StrForBench_SImpl.inl을 공유한다.
 */

#include <cstring>

namespace jc_gbench { namespace STRFORBENCH_NS {

struct StrBenchImpl
{
	char* pHeap_{};
	int len_{};
	int cap_;
	_u32 owner_{};
	char sso_[44];
};

static_assert(sizeof(StrBenchImpl) == 64, "S6/S7 StringImpl slot must stay 64 bytes.");

inline thread_local benchpool::XThreadPool<64>* t_pPool = nullptr;

inline benchpool::XThreadPool<64>& ThisPool()
{
	benchpool::XThreadPool<64>* p = t_pPool;
	if (!p)
	{
		p = STRFORBENCH_XTHREAD_STORAGE().GetOrCreate(benchpool::BenchThreadId());
		t_pPool = p;
	}
	return *p;
}

inline StrBenchImpl* PoolNewImpl()
{
	void* raw = ThisPool().Acquire(STRFORBENCH_XTHREAD_AUTO_DRAIN != 0);
	StrBenchImpl* p = new (raw) StrBenchImpl{};
	p->owner_ = benchpool::BenchThreadId();
	return p;
}

inline void PoolReleaseImpl(StrBenchImpl* _p)
{
	if (!_p)
		return;
	const _u32 owner = _p->owner_;
	_p->~StrBenchImpl();
	if (owner == benchpool::BenchThreadId())
	{
		ThisPool().ReleaseLocal(_p);
	}
	else
	{
		STRFORBENCH_XTHREAD_STORAGE().GetOrCreate(owner)->ReleaseShared(_p);
	}
}

// S6 명시 pump. owner 스레드가 주기적으로 호출한다.
inline void ProcessThreadPoolDeferred()
{
	ThisPool().DrainInbox();
}

#include "StrForBench_SImpl.inl"

}} // namespace jc_gbench::STRFORBENCH_NS
