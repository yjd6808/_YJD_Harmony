/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * 스레드-로컬 인덱스 기반 오브젝트풀
 *
 * - 스레드마다 프리리스트(Pool)를 하나씩 갖고, 객체마다 그 객체를 힙에서 받아온 Pool 번호(poolIdx_)를 찍는다.
 * - 같은 스레드 안 Pop/Push는 락·원자 연산 없음.
 * - 다른 스레드가 Push하면 poolIdx_로 주인 Pool을 찾아 inbox에 CAS push한다. 주인은 프리리스트가 비었을 때 inbox를 통째로 회수한다.
 * - 주인 스레드가 종료되면 Pool은 고아(alive_ = 0)가 되고 번호는 다음 스레드가 재사용한다. 고아 Pool의 객체를 Push하면 즉시 힙으로 간다.
 *
 * "Indexed"는 소유 Pool 번호 인덱스를 말한다. CIndexedMemoryPool(사이즈 클래스 인덱싱)과는 무관.
 * 기존 ObjectPool<T>(전역 연결리스트 + SpinLock)과는 다른 정책이므로 대체가 아니라 보완이다.
 * 공개 API는 Pop()/Push()다 (ObjectPool<T>의 PopObject/PushObject와 같은 동사).
 *
 * [T의 요구 조건]
 *  - TlsPoolNode를 첫 베이스로 상속한다 (offset 0에 poolIdx_)
 *  - sizeof(T) >= 16, alignof(T) <= alignof(max_align_t)
 *  - Push된 상태에서는 풀이 offset 8~15를 링크로 덮어쓴다
 *
 * [사용]
 *  struct Foo : TlsPoolNode { ... };
 *  Foo* p = ObjectPoolTLSIndexed<Foo>::Pop();   // 생성자 호출 안 함 (메모리만)
 *  ObjectPoolTLSIndexed<Foo>::Push(p);          // 소멸자 호출 안 함
 *
 *  struct Bar : TlsPoolNode, TlsPoolNewDeleteMixin<Bar> { ... };
 *  Bar* p = dbg_new Bar(...);  delete p;        // 생성자/소멸자 포함
 *  new Bar[10] 사용 불가
 */

#pragma once

#include <new>
#include <typeinfo>
#include <type_traits>
#include <cstddef>

#include "jc/Define.h"
#include "jc/Macro.h"
#include "jc/Type.h"
#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Debug/New.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Sync/SpinLock.h"
#include "jc/Sync/LockGuard.h"
#include "jc/Logger/LoggerDefine.h"
#include "jc/TypeTraits.h"

NS_JC_BEGIN

struct TlsPoolNode
{
	_u32 poolIdx_;		// 이 객체를 힙에서 처음 받아온 풀의 번호. 풀만 쓴다.
};

template <typename T>
class ObjectPoolTLSIndexed;

// new/delete 문법이 필요한 T가 함께 상속한다. String 슬롯에는 쓰지 않는다.
// 의도적으로 클래스 바깥 템플릿이다. 중첩으로 두면 파생 클래스 정의 중에
// 풀 본체가 불완전 타입으로 인스턴스화되어 클래스 영역 static_assert에 걸린다 (CRTP).
//   struct Bar : TlsPoolNode, TlsPoolNewDeleteMixin<Bar> { ... };
template <typename T>
struct TlsPoolNewDeleteMixin
{
	using TPool = ObjectPoolTLSIndexed<T>;

	static void* operator new(size_t _size);
	static void* operator new(size_t _size, int _blockUse, char const* _pFileName, int _lineNumber);
	static void operator delete(void* _pObject);
	static void operator delete(void* _pObject, int _blockUse, char const* _pFileName, int _lineNumber);
	static void* operator new[](size_t _size) = delete;
	static void operator delete[](void* _pObject) = delete;
};

template <typename T>
class ObjectPoolTLSIndexed final
{
	static_assert(IsBaseOf_v<TlsPoolNode, T>, "T는 TlsPoolNode를 첫 베이스로 상속해야 합니다.");
	static_assert(sizeof(T) >= 16, "T는 최소 16바이트여야 합니다. (프리 상태 링크가 offset 8~15를 사용)");
	static_assert(alignof(T) <= alignof(std::max_align_t), "over-aligned T는 지원하지 않습니다.");

public:
	using TPool = ObjectPoolTLSIndexed<T>;
	using TLock = SpinLock;

	static constexpr _u32 kNone				= ~0u;
	static constexpr _u32 kFastPoolCount	= 256;
	static constexpr _u32 kSegmentBits		= 6;
	static constexpr _u32 kSegmentSize		= 1u << kSegmentBits;
	static constexpr _u32 kMaxSegmentCount	= 1024;
	static constexpr _u32 kMaxPoolCount		= kFastPoolCount + kSegmentSize * kMaxSegmentCount;

	struct Stats
	{
		_u32 poolCount;			// 발급된 풀 번호 수
		_u32 alivePoolCount;	// alive_ == 1인 풀 수
		_u32 orphanPoolCount;	// 고아 목록 길이
		_u64 totalCount;		// 살아있는 풀들의 totalCount_ 합 (스냅샷, 근사)
		_u64 freeCount;			// 살아있는 풀들의 freeCount_ 합 (inbox 미포함)
	};

	// new/delete 문법은 TlsPoolNewDeleteMixin<T> 상속으로 opt-in 한다 (클래스 바깥 정의).

	ObjectPoolTLSIndexed() = delete;		// static 전용 클래스

	static T* Pop();					// 메모리만 돌려준다. 생성자 호출 안 함. poolIdx_는 찍혀 있다.
	static void Push(T* _pObject);		// 소멸자 호출 안 함. 어느 스레드에서 불러도 안전.
	static void SweepOrphans();			// 고아 풀들의 inbox를 힙으로. 콜드.
	static void TrimThisThread();		// 현 스레드 풀의 pFree_를 힙으로 + SweepOrphans. 콜드.
	static Stats GetStats();			// 집계. 콜드. 스냅샷이므로 근사값.
	static _u32 ThisPoolIdx();			// 현 스레드 풀 번호. 없으면 kNone. 테스트용.

private:
	struct FreeNode
	{
		_u32		poolIdx_;		// offset 0 : TlsPoolNode::poolIdx_와 같은 자리. 프리 상태에서도 보존.
		_u32		reserved_;		// offset 4 : 건드리지 않음 (T의 데이터 자리)
		FreeNode*	pNext_;			// offset 8 : 프리 상태에서만 유효
	};
	static_assert(sizeof(FreeNode) == 16);

	struct alignas(JC_CACHE_ALIGN_SIZE) Pool
	{
		FreeNode*	pFree_;				// 프리리스트 헤드 (LIFO)
		_u32		idx_;				// 풀 번호
		_u32		totalCount_;		// 이 풀이 힙에서 받아온 객체 수 (owner 전용)
		_u32		freeCount_;			// pFree_ 길이 (owner 전용)
		_u32		reserved_;
		Pool*		pNextOrphan_;		// 고아 목록 링크 (OrphanLock 아래에서만)

		alignas(JC_CACHE_ALIGN_SIZE)
		Atomic<FreeNode*>	inbox_;		// foreign 반납함. push: 누구나 CAS, pop: owner가 Exchange(nullptr)
		Atomic<_u32>		alive_;		// 1 = owner 살아있음, 0 = 고아
	};
	static_assert(sizeof(Pool) == 2 * JC_CACHE_ALIGN_SIZE);
	static_assert(std::is_trivially_destructible_v<Atomic<Pool*>>, "Atomic<Pool*>는 trivially destructible이어야 합니다. (정적 소멸 순서)");
	static_assert(std::is_trivially_destructible_v<Atomic<_u32>>, "Atomic<_u32>는 trivially destructible이어야 합니다. (정적 소멸 순서)");

	struct Segment
	{
		Atomic<Pool*> slots_[kSegmentSize];
	};

	struct Guard
	{
		_u32 idx_ = kNone;
		~Guard();
	};

	static Pool* PoolAt(_u32 _idx);
	static Pool* ThisPool();
	static Pool* AttachPool();
	static Pool* CreatePool();
	static Pool* PopOrphan();
	static void PushOrphan(Pool* _pPool);
	static _u32 FreeChain(FreeNode* _pHead);
	static TLock& GetOrphanLock();

	inline static Atomic<Pool*>		FastTable[kFastPoolCount];
	inline static Atomic<Segment*>	SegmentTable[kMaxSegmentCount];
	inline static Atomic<_u32>		PoolCount;
	inline static Pool*				OrphanHead = nullptr;
	inline static _u32				OrphanCount = 0;

	inline static thread_local Pool*	tls_pPool = nullptr;
	inline static thread_local Guard	tls_Guard;
};

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline T* ObjectPoolTLSIndexed<T>::Pop()
{
	Pool* pPool = ThisPool();
	FreeNode* pNode = pPool->pFree_;

	if (pNode == nullptr)
	{
		// 프리리스트가 비었다. 다른 스레드가 돌려준 것이 있으면 통째로 가져온다.
		pNode = pPool->inbox_.Exchange(nullptr);

		if (pNode == nullptr)
		{
			// 그것도 없다. 힙에서 새로 받고 이 풀 번호를 찍는다. (I1: 도장은 이 곳 1회)
			T* pNew = Memory::Allocate<T*>(sizeof(T));
			pNew->poolIdx_ = pPool->idx_;
			++pPool->totalCount_;
			return pNew;
		}

		// drain한 체인의 길이는 이 자리에서 세지 않는다. (freeCount_는 스냅샷용 근사값)
		// 반환하는 헤드 이후 노드들을 pFree_로 잘라붙인다.
		pPool->pFree_ = pNode->pNext_;
		jc_assert(pNode->poolIdx_ == pPool->idx_);		// I3
		return reinterpret_cast<T*>(pNode);
	}

	pPool->pFree_ = pNode->pNext_;
	--pPool->freeCount_;
	jc_assert(pNode->poolIdx_ == pPool->idx_);			// I2
	return reinterpret_cast<T*>(pNode);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void ObjectPoolTLSIndexed<T>::Push(T* _pObject)
{
	if (_pObject == nullptr)
	{
		_LogWarn_("반납할려는 %s 객체가 nullptr 입니다.", typeid(T).name());
		return;
	}

	FreeNode* pNode = reinterpret_cast<FreeNode*>(_pObject);
	Pool* pMine = tls_pPool;

	// ① 내 풀 것 — 핫 패스. 락·원자 연산 없음.
	if (pMine != nullptr && pNode->poolIdx_ == pMine->idx_)
	{
		pNode->pNext_ = pMine->pFree_;
		pMine->pFree_ = pNode;
		++pMine->freeCount_;
		return;
	}

	// ② 다른 스레드 것. 주인을 찾는다.
	jc_assert(pNode->poolIdx_ < PoolCount.Load());
	Pool* pOwner = PoolAt(pNode->poolIdx_);

	if (pOwner->alive_.Load() == 0)
	{
		// 주인 스레드가 이미 종료됨. inbox에 넣으면 아무도 안 꺼내가므로 바로 힙으로.
		Memory::Deallocate(_pObject);
		return;
	}

	// ③ 주인 inbox에 CAS push (Treiber). 실패하면 _expected가 최신 헤드로 갱신되어 있다.
	FreeNode* pHead = pOwner->inbox_.Load();
	do
	{
		pNode->pNext_ = pHead;
	}
	while (!pOwner->inbox_.CompareExchange(pHead, pNode));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::PoolAt(_u32 _idx)
{
	if (_idx < kFastPoolCount)
	{
		return FastTable[_idx].Load();
	}

	const _u32 overflowIdx = _idx - kFastPoolCount;
	Segment* pSegment = SegmentTable[overflowIdx >> kSegmentBits].Load();
	jc_assert(pSegment != nullptr);
	return pSegment->slots_[overflowIdx & (kSegmentSize - 1)].Load();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::ThisPool()
{
	Pool* pPool = tls_pPool;

	if (pPool != nullptr)
	{
		return pPool;
	}

	return AttachPool();		// 스레드당 1회
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::AttachPool()
{
	jc_assert(tls_pPool == nullptr);

	SweepOrphans();								// 기회가 된 김에 고아 inbox 정리 (D6)

	Pool* pPool = PopOrphan();					// 반납된 번호가 있으면 재사용
	if (pPool == nullptr)
	{
		pPool = CreatePool();					// 없으면 새 번호
	}

	jc_assert(pPool->pFree_ == nullptr);		// I6
	pPool->alive_.Store(1);						// 이 시점부터 foreign Push가 inbox로 온다
	tls_pPool = pPool;
	tls_Guard.idx_ = pPool->idx_;				// tls_Guard 첫 참조: 생성자 실행 + 소멸자 등록
	return pPool;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::CreatePool()
{
	const _u32 idx = PoolCount.Increment() - 1;			// Increment는 증가 후 값을 반환
	jc_assert_msg(idx < kMaxPoolCount, "ObjectPoolTLSIndexed: 풀 번호가 상한(%u)을 넘었습니다.", kMaxPoolCount);

	// Pool은 alignas(64). 일반 new는 C++17 aligned new를 탄다. dbg_new(placement 인자)와는 혼용 안 되므로
	// 정렬 할당 + placement new로 명시한다. 해제하지 않는다 (D8).
	void* pMem = ::operator new(sizeof(Pool), std::align_val_t{ alignof(Pool) });
	Pool* pPool = new (pMem) Pool{};
	pPool->idx_ = idx;

	if (idx < kFastPoolCount)
	{
		FastTable[idx].Store(pPool);
		return pPool;
	}

	const _u32 overflowIdx = idx - kFastPoolCount;
	const _u32 segIdx = overflowIdx >> kSegmentBits;

	Segment* pSegment = SegmentTable[segIdx].Load();
	if (pSegment == nullptr)
	{
		// 같은 세그먼트에 속한 번호를 받은 다른 스레드와 경쟁할 수 있다. CAS로 한 명만 이긴다.
		Segment* pFresh = new Segment{};
		Segment* pExpected = nullptr;
		if (SegmentTable[segIdx].CompareExchange(pExpected, pFresh))
		{
			pSegment = pFresh;
		}
		else
		{
			delete pFresh;
			pSegment = pExpected;
		}
	}

	pSegment->slots_[overflowIdx & (kSegmentSize - 1)].Store(pPool);
	return pPool;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::TLock& ObjectPoolTLSIndexed<T>::GetOrphanLock()
{
	static TLock lock;
	return lock;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::PopOrphan()
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	Pool* pPool = OrphanHead;
	if (pPool == nullptr)
	{
		return nullptr;
	}

	OrphanHead = pPool->pNextOrphan_;
	pPool->pNextOrphan_ = nullptr;
	--OrphanCount;
	return pPool;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void ObjectPoolTLSIndexed<T>::PushOrphan(Pool* _pPool)
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	_pPool->pNextOrphan_ = OrphanHead;
	OrphanHead = _pPool;
	++OrphanCount;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline _u32 ObjectPoolTLSIndexed<T>::FreeChain(FreeNode* _pHead)
{
	_u32 count = 0;
	while (_pHead != nullptr)
	{
		FreeNode* pNext = _pHead->pNext_;
		Memory::Deallocate(_pHead);
		_pHead = pNext;
		++count;
	}
	return count;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void ObjectPoolTLSIndexed<T>::SweepOrphans()
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	for (Pool* pPool = OrphanHead; pPool != nullptr; pPool = pPool->pNextOrphan_)
	{
		jc_assert(pPool->alive_.Load() == 0);
		FreeChain(pPool->inbox_.Exchange(nullptr));		// 고아가 된 뒤 늦어 들어온 반납물
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void ObjectPoolTLSIndexed<T>::TrimThisThread()
{
	Pool* pPool = tls_pPool;
	if (pPool != nullptr)
	{
		FreeChain(pPool->pFree_);
		pPool->pFree_ = nullptr;
		pPool->totalCount_ -= pPool->freeCount_;
		pPool->freeCount_ = 0;
		FreeChain(pPool->inbox_.Exchange(nullptr));
	}

	SweepOrphans();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline ObjectPoolTLSIndexed<T>::Guard::~Guard()
{
	if (idx_ == kNone)
	{
		return;
	}

	Pool* pPool = PoolAt(idx_);

	// 1) 먼저 고아 선언. 이후 foreign Push는 ② 경로(즉시 힙)로 간다.
	pPool->alive_.Store(0);

	// 2) 내 프리리스트 힙 반납. (이 스레드만 만지는 것이라 락 불필요)
	FreeChain(pPool->pFree_);
	pPool->pFree_ = nullptr;
	pPool->freeCount_ = 0;
	pPool->totalCount_ = 0;

	// 3) inbox 두 번 drain.
	//    첫 번째: alive_=0 이전에 들어온 것.
	//    두 번째: "alive_==1을 읽고 → 우리가 alive_=0 하고 1차 drain → 그 다음 CAS push" 한 연산을 거의 다 잡는다.
	//    두 번째 이후에도 극히 늦게 push된 것은 SweepOrphans()가 회수한다.
	FreeChain(pPool->inbox_.Exchange(nullptr));
	FreeChain(pPool->inbox_.Exchange(nullptr));

	// 4) 번호 반납
	tls_pPool = nullptr;
	PushOrphan(pPool);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline typename ObjectPoolTLSIndexed<T>::Stats ObjectPoolTLSIndexed<T>::GetStats()
{
	Stats stats{};
	stats.poolCount = PoolCount.Load();

	for (_u32 i = 0; i < stats.poolCount; ++i)
	{
		Pool* pPool = PoolAt(i);
		if (pPool == nullptr)				// R6: 번호 발급 ~ 테이블 store 사이. 유일한 null 가능 호출자.
		{
			continue;
		}
		if (pPool->alive_.Load() == 0)
		{
			continue;
		}
		++stats.alivePoolCount;
		stats.totalCount += pPool->totalCount_;		// owner가 쓰는 중일 수 있다. 스냅샷이므로 근사.
		stats.freeCount  += pPool->freeCount_;
	}

	{
		JC_LIB_LOCK_GUARD(GetOrphanLock());
		stats.orphanPoolCount = OrphanCount;
	}
	return stats;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline _u32 ObjectPoolTLSIndexed<T>::ThisPoolIdx()
{
	return tls_pPool != nullptr ? tls_pPool->idx_ : kNone;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void* TlsPoolNewDeleteMixin<T>::operator new(size_t _size)
{
	jc_assert_msg(_size == sizeof(T), "파생 타입은 이 풀로 할당할 수 없습니다.");
	return TPool::Pop();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void* TlsPoolNewDeleteMixin<T>::operator new(size_t _size, int /*_blockUse*/, char const* /*_pFileName*/, int /*_lineNumber*/)
{
	return operator new(_size);			// dbg_new 호환. 풀이 이미 추적하므로 디버그 인자는 버린다.
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void TlsPoolNewDeleteMixin<T>::operator delete(void* _pObject)
{
	TPool::Push(static_cast<T*>(_pObject));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void TlsPoolNewDeleteMixin<T>::operator delete(void* _pObject, int /*_blockUse*/, char const* /*_pFileName*/, int /*_lineNumber*/)
{
	TPool::Push(static_cast<T*>(_pObject));
}

NS_END
