/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once


#include <JCore/Declspec.h>
#include <JCore/Sync/LockGuard.h>

NS_JC_BEGIN

enum RwLockType
{
    eNormalRwLock
};

struct JCORE_NOVTABLE IRwLock
{
    IRwLock() = default;
    virtual ~IRwLock() = default;

    virtual void WriteLock() = 0;
    virtual bool TryWriteLock() = 0;
    virtual void WriteUnlock() = 0;
    virtual bool IsWriteLocked() = 0;

    virtual void ReadLock() = 0;
    virtual bool TryReadLock() = 0;
    virtual void ReadUnlock() = 0;
    virtual bool IsReadLocked() = 0;
};

NS_JC_END;

