/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include "jc/Sync/NormalLock.h"

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
	NormalLock m_lock;
	_u32 m_lockedThreadId;
	int m_recursion;
};

using RecursiveLockGuard = LockGuard<RecursiveLock>;
extern template            LockGuard<RecursiveLock>;

NS_END
