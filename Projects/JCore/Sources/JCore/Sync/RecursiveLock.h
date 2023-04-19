/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/NormalLock.h>

NS_JC_BEGIN

class RecursiveLock final : public ILock
{
public:
	using TGuard = LockGuard<RecursiveLock>;

	RecursiveLock();
	~RecursiveLock() override = default;

	void Lock() override;
	bool TryLock() override;
	void Unlock() override;
	bool IsLocked() override;
private:
	NormalLock m_Lock;
	Int32U m_uiLockedThreadId;
	int m_iRecursion;
};

using RecursiveLockGuard = LockGuard<RecursiveLock>;
extern template            LockGuard<RecursiveLock>;

NS_JC_END
