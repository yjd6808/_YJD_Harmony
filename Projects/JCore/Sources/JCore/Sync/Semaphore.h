/*
 * 작성자: 윤정도
 * =====================
 */


#pragma once

#include <JCore/Sync/NormalLock.h>

#include <condition_variable>

NS_JC_BEGIN

class Semaphore final : public ILock
{
public:
	Semaphore();
	Semaphore(int maxCount, int initialUsableCount);

	void Lock() override;
	void Unlock() override;
	bool TryLock() override;
	bool IsLocked() override;
	int MaxCount() const { return m_iMaxCount; }
	int UsableCount();

    void Acquire() { Lock(); }
    void Release(int count = 1);
    bool TryAcquire() { return TryLock(); }
    bool IsAcquired() { return IsLocked(); }
private:
	std::condition_variable m_Condvar;
	std::mutex m_Mtx;
	int m_iMaxCount;
	int m_iUsableCount;
};

using SemaphoreGuard =  LockGuard<Semaphore>;
extern template         LockGuard<Semaphore>;

NS_JC_END