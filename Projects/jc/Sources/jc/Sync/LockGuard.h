/*
 * 작성자 : 윤정도
 */

#pragma once

#include "jc/Config.h"

NS_JC_BEGIN

template <typename TLock>
class LockGuard final
{
	using TLockGuard = LockGuard<TLock>;
public:
	LockGuard(TLock& _lock) : m_pLock(&_lock) {
		m_pLock->Lock();
	}

	~LockGuard() {
		m_pLock->Unlock();
	}

	TLock* Handle() const { return m_pLock; }

	void Lock() { m_pLock->Lock(); }
	void Unlock() { m_pLock->Unlock(); }

	LockGuard(const TLockGuard& _lockGuard) = delete;
	TLock& operator=(const TLockGuard& _lockGuard) = delete;
private:
	TLock* m_pLock;
};


// jc 라이브러리내부에서 사용하는 용도의 락가드
// 앱이 종료된 이후에는 락기능 무효화를 시킴
// 이유는 Config.h 주석 참고
template <typename TLock>
class JCoreLibLockGuard final
{
	using TLockGuard = LockGuard<TLock>;
public:
	JCoreLibLockGuard(TLock& _lock) : m_pLock(&_lock) {
		if (!AppExited)
			m_pLock->Lock();
	}

	~JCoreLibLockGuard() {
		if (!AppExited)
			m_pLock->Unlock();
	}

	TLock* Handle() const { return m_pLock; }

	void Lock() {
		if (!AppExited)
			m_pLock->Lock();
	}
	void Unlock() {
		if (!AppExited)
			m_pLock->Unlock();
	}

	JCoreLibLockGuard(const TLockGuard& _lockGuard) = delete;
	TLock& operator=(const TLockGuard& _lockGuard) = delete;
private:
	TLock* m_pLock;
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
	RwLockGuard(TRwLock& _lock) : m_pLock(&_lock) {
		if constexpr (Mode == RwLockMode::Read)
			m_pLock->ReadLock();
		else
			m_pLock->WriteLock();
	}

	~RwLockGuard() {
		if constexpr (Mode == RwLockMode::Read)
			m_pLock->ReadUnlock();
		else
			m_pLock->WriteUnlock();
	}


	RwLockGuard(TLockGuard&& _lockGuard) = delete;
	void operator=(const TLockGuard& _lockGuard) = delete;
private:
	TRwLock* m_pLock;
};

NS_END

