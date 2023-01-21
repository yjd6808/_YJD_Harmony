/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>
#include <JCore/Sync/LockGuard.h>


namespace JCore {

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



} // namespace JCore;