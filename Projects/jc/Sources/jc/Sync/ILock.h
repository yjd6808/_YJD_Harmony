/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include "jc/Declspec.h"
#include "jc/Define.h"
#include "jc/Sync/LockGuard.h"


NS_JC_BEGIN

enum LockType
{
    eNormalLock,
    eRecursiveLock,
    eEventLock,
    eSemaphore,
    eSpinLock,
    eUnusedLock
};

struct JC_NOVTABLE ILock
{
	ILock() = default;
	virtual ~ILock() = default;

	virtual void Lock() = 0;
	virtual bool TryLock() = 0;
	virtual void Unlock() = 0;
	virtual bool IsLocked() = 0;
};

NS_END;

#define JC_LOCK_GUARD(lock)		jc::LockGuard<jc::ILock> JC_CONCAT_COUNTER(__lock_guard__)(lock)
#define JC_LIB_LOCK_GUARD(lock)	jc::JCoreLibLockGuard<jc::ILock> JC_CONCAT_COUNTER(__lock_guard__)(lock)