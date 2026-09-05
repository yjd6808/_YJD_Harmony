/*
 * StrForBenchPool.h: S3~S7용 TLS free-list / DeferredQueue / PoolStorage가 들어가는 자리.
 * 락 없음. 같은 스레드에서 Acquire/Release하는 것만 허용한다.
 * (cross-thread Release는 S5~S7에서 owner 검사 후 따로 처리한다.)
 */

#pragma once

#include <cstddef>
#include <new>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "jc/Threading/Thread.h"

namespace jc_gbench { namespace benchpool {

template <size_t kSlotSize>
class TlsFreeList
{
	struct Node { Node* pNext_; };
	static_assert(sizeof(Node) <= kSlotSize, "slot too small for free-list link");

	Node* pHead_ = nullptr;
public:
	TlsFreeList() = default;
	~TlsFreeList()
	{
		while (pHead_)
		{
			Node* p = pHead_;
			pHead_ = p->pNext_;
			::operator delete(p);
		}
	}
	TlsFreeList(const TlsFreeList&) = delete;
	TlsFreeList& operator=(const TlsFreeList&) = delete;

	void* AcquireRaw()
	{
		if (pHead_)
		{
			Node* p = pHead_;
			pHead_ = p->pNext_;
			return p;
		}
		return ::operator new(kSlotSize);
	}

	void ReleaseRaw(void* _p)
	{
		Node* p = static_cast<Node*>(_p);
		p->pNext_ = pHead_;
		pHead_ = p;
	}
};

template <typename TSlot>
class TlsSlotPool
{
	static_assert(alignof(TSlot) <= alignof(void*), "over-aligned slot");
	TlsFreeList<sizeof(TSlot)> free_;
public:
	TSlot* AcquireRaw() { return static_cast<TSlot*>(free_.AcquireRaw()); }
	void ReleaseRaw(TSlot* _p) { free_.ReleaseRaw(_p); }
};

// 호출 스레드의 jc 스레드 ID를 반환한다.
inline _u32 BenchThreadId() { return jc::Thread::GetThreadId(); }

// S6/S7 공용 cross-thread pool 기반.
// hot path(같은 스레드)는 lock-free, cross-thread 반납만 inbox(mutex)에 넣는다.
// S6은 명시적 pump 때만 inbox를 비우고, S7은 Acquire 때 자동으로 비운다.
template <size_t kSlotSize>
struct XThreadPool
{
	struct Node { Node* pNext_; };
	static_assert(sizeof(Node) <= kSlotSize, "slot too small for free-list link");

	_u32 ownerId_ = 0;
	Node* pFree_ = nullptr;
	std::mutex inboxMutex_;
	std::vector<void*> inbox_;

	~XThreadPool()
	{
		while (pFree_)
		{
			Node* p = pFree_;
			pFree_ = p->pNext_;
			::operator delete(p);
		}
		for (void* p : inbox_)
		{
			::operator delete(p);
		}
	}
	XThreadPool() = default;
	XThreadPool(const XThreadPool&) = delete;
	XThreadPool& operator=(const XThreadPool&) = delete;

	void* TryPop()
	{
		if (!pFree_)
			return nullptr;
		Node* p = pFree_;
		pFree_ = p->pNext_;
		return p;
	}

	void PushFree(void* _p)
	{
		Node* p = static_cast<Node*>(_p);
		p->pNext_ = pFree_;
		pFree_ = p;
	}

	void* Acquire(bool _autoDrain)
	{
		if (void* p = TryPop())
			return p;
		if (_autoDrain)
			DrainInbox();
		if (void* p = TryPop())
			return p;
		return ::operator new(kSlotSize);
	}

	void ReleaseLocal(void* _p) { PushFree(_p); }

	void ReleaseShared(void* _p)
	{
		std::lock_guard<std::mutex> lock(inboxMutex_);
		inbox_.push_back(_p);
	}

	void DrainInbox()
	{
		std::lock_guard<std::mutex> lock(inboxMutex_);
		for (void* p : inbox_)
		{
			PushFree(p);
		}
		inbox_.clear();
	}
};

// 프로세스 lifetime pool 보관소. 스레드가 종료돼도 pool이 살아있어
// cross-thread 반납이 항상 유효하다. 죽은 스레드의 pool 누적은 GC 정책 항목이다.
struct XThreadStorage
{
	std::mutex mutex_;
	std::unordered_map<_u32, XThreadPool<64>*> pools_;

	~XThreadStorage()
	{
		for (auto& kv : pools_)
		{
			delete kv.second;
		}
	}
	XThreadStorage() = default;
	XThreadStorage(const XThreadStorage&) = delete;
	XThreadStorage& operator=(const XThreadStorage&) = delete;

	XThreadPool<64>* GetOrCreate(_u32 _id)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = pools_.find(_id);
		if (it != pools_.end())
			return it->second;
		XThreadPool<64>* p = new XThreadPool<64>();
		p->ownerId_ = _id;
		pools_[_id] = p;
		return p;
	}

	void DrainAll()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& kv : pools_)
		{
			kv.second->DrainInbox();
		}
	}

	void PurgeAll()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& kv : pools_)
		{
			delete kv.second;
		}
		pools_.clear();
	}
};

inline XThreadStorage& GlobalXThreadStorageS6()
{
	static XThreadStorage s;
	return s;
}

inline XThreadStorage& GlobalXThreadStorageS7()
{
	static XThreadStorage s;
	return s;
}

}} // namespace jc_gbench::benchpool
