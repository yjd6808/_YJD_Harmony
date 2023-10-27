/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Config.h>

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

	TLock* Handle() const { return m_Mtx; }

	void Lock() { m_Mtx->Lock(); }
	void Unlock() { m_Mtx->Unlock(); }

	LockGuard(const TLockGuard& mtx) = delete;
	TLock& operator=(const TLockGuard& mtx) = delete;
private:
	TLock* m_Mtx;
};


// JCore 라이브러리내부에서 사용하는 용도의 락가드
// 앱이 종료된 이후에는 락기능 무효화를 시킴
// 이유는 Config.h 주석 참고
template <typename TLock>
class JCoreLibLockGuard final
{
	using TLockGuard = LockGuard<TLock>;
public:
	JCoreLibLockGuard(TLock& mtx) : m_Mtx(&mtx) {
		if (!AppExited)
			m_Mtx->Lock();
	}

	~JCoreLibLockGuard() {
		if (!AppExited)
			m_Mtx->Unlock();
	}

	TLock* Handle() const { return m_Mtx; }

	void Lock() {
		if (!AppExited)
			m_Mtx->Lock();
	}
	void Unlock() {
		if (!AppExited)
			m_Mtx->Unlock();
	}

	JCoreLibLockGuard(const TLockGuard& mtx) = delete;
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

