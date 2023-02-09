/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

template <typename TLock>
class LockGuard final
{
	using TLockGuard = LockGuard<TLock>;
public:
	LockGuard(TLock& mtx) : m_Mtx(&mtx) {
		m_Mtx->Lock();
	}

	~LockGuard() {
		m_Mtx->Unlock();
	}

	LockGuard(const TLockGuard& mtx) = delete;
	TLock& operator=(const TLockGuard& mtx) = delete;
private:
	TLock* m_Mtx;
};


enum class RwLockMode
{
	Read,
	Write
};

template <typename TRwLock, RwLockMode Mode>
class RwLockGuard final
{
	using TLockGuard = RwLockGuard<TRwLock, Mode>;
public:
	RwLockGuard(TRwLock& mtx) : m_Mtx(&mtx) {
		if constexpr (Mode == RwLockMode::Read)
			m_Mtx->ReadLock();
		else
			m_Mtx->WriteLock();
	}

	~RwLockGuard() {
		if constexpr (Mode == RwLockMode::Read)
			m_Mtx->ReadUnlock();
		else
			m_Mtx->WriteUnlock();
	}


	RwLockGuard(TLockGuard&& mtx) = delete;
	void operator=(const TLockGuard& mtx) = delete;
private:
	TRwLock* m_Mtx;
};

NS_JC_END

