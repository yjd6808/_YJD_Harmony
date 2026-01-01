/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <jc/Sync/ILock.h>
#include <jc/Primitives/Atomic.h>

NS_JC_BEGIN

class SpinLock final : public ILock
{
public:
	using TGuard = LockGuard<SpinLock>;

	SpinLock();
	~SpinLock() override;

	void Lock() override;
	void Unlock() override;
	bool TryLock() override;
	bool IsLocked() override;
private:
	Atomic<bool> isLocked_;
};

using SpinLockGuard = LockGuard<SpinLock>;
extern template       LockGuard<SpinLock>;

NS_JC_END