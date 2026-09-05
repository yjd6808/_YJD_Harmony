/*
 * StrForBench_S5.inl: S5 owner 비교 + global fallback 후보 (8B 핸들).
 * 헤더 가드 없음. StrForBench.h에서 네임스페이스 s5로 include한다.
 * 같은 스레드에서 소멸하면 TLS pool로, 다른 스레드에서 소멸하면
 * 힙 버퍼는 일반 해제, 슬롯은 개별 할당분이므로 global delete 한다(S5' 단순화안).
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

static_assert(sizeof(StrBenchImpl) == 64, "S5 StringImpl slot must stay 64 bytes.");

inline thread_local benchpool::TlsSlotPool<StrBenchImpl> t_implPool{};

inline StrBenchImpl* PoolNewImpl()
{
	StrBenchImpl* p = t_implPool.AcquireRaw();
	p = new (p) StrBenchImpl{};
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
		t_implPool.ReleaseRaw(_p);
	}
	else
	{
		::operator delete(_p);
	}
}

#include "StrForBench_SImpl.inl"

}} // namespace jc_gbench::STRFORBENCH_NS
