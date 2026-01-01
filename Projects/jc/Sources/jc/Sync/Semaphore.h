/*
 * 작성자: 윤정도
 * =====================
 */


#pragma once

#include <jc/Sync/ConditionVariable.h>

NS_JC_BEGIN

class Semaphore final : public ILock
{
public:
	Semaphore();
	Semaphore(int _maxCount, int _initialUsableCount);

	void Lock() override;
	void Unlock() override;
	bool TryLock() override;
	bool IsLocked() override;
	int MaxCount() const { return maxCount_; }
	int UsableCount();

    void Acquire() { Lock(); }
    void Release(int _count = 1);
    bool TryAcquire() { return TryLock(); }
    bool IsAcquired() { return IsLocked(); }
private:
	ConditionVariable condVar_;
	NormalLock lock_;
	int maxCount_;
	int usableCount_;
};

using SemaphoreGuard =  LockGuard<Semaphore>;
extern template         LockGuard<Semaphore>;

NS_JC_END