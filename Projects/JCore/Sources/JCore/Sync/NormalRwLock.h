/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/IRwLock.h>
#include <JCore/Sync/ConditionVariable.h>

NS_JC_BEGIN

class NormalRwLock final : public IRwLock
{
public:
	using TGuard = LockGuard<NormalRwLock>;

	NormalRwLock();

	void WriteLock() override;
	bool TryWriteLock() override;
	void WriteUnlock() override;
	bool IsWriteLocked() override;

	void ReadLock() override;
	bool TryReadLock() override;
	void ReadUnlock() override;
	bool IsReadLocked() override;
private:
	ConditionVariable m_CondVar;
	NormalLock m_Lock;

	int m_iReadCount;
	bool m_bWriteFlag;
};

using NormalWriteLockGuard = RwLockGuard<NormalRwLock, RwLockMode::Write>;
using NormalReadLockGuard = RwLockGuard<NormalRwLock, RwLockMode::Read>;

extern template NormalWriteLockGuard;
extern template NormalReadLockGuard;

NS_JC_END

