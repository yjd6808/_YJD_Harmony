/*
 * 작성자: 윤정도
 * 생성일: 12/5/2022 8:45:26 PM
 * =====================
 * 아무 기능을 하지 않는 락
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Sync/LockGuard.h>

namespace JCore {

  class UnusedLock final : public ILock
  {
  public:
    void Lock() override {}
    bool TryLock() override { return true; }
    void Unlock() override {}
    bool IsLocked() override { return true; }
  };

  using UnusedLockGuard = LockGuard<UnusedLock>;
  extern template         LockGuard<UnusedLock>;
} // namespace JCore;

