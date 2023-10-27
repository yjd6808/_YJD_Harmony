﻿/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>
#include <JCore/Define.h>
#include <JCore/Sync/LockGuard.h>


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

struct JCORE_NOVTABLE ILock
{
	ILock() = default;
	virtual ~ILock() = default;

	virtual void Lock() = 0;
	virtual bool TryLock() = 0;
	virtual void Unlock() = 0;
	virtual bool IsLocked() = 0;
};

NS_JC_END;

#define JCORE_LOCK_GUARD(lock)		JCore::LockGuard<JCore::ILock> JCORE_CONCAT_COUNTER(__lock_guard__)(lock)
#define JCORE_LIB_LOCK_GUARD(lock)	JCore::JCoreLibLockGuard<JCore::ILock> JCORE_CONCAT_COUNTER(__lock_guard__)(lock)