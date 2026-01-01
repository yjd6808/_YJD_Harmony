/*
	작성자 : 윤정도
	뮤텍스 테스트
*/


#include <jc_gtest/Core.h>

#if TEST_LockGuardTest == ON

#include <jc/Sync/EventLock.h>
#include <jc/Sync/NormalLock.h>
#include <jc/Sync/NormalRwLock.h>
#include <jc/Sync/RecursiveLock.h>
#include <jc/Sync/Semaphore.h>
#include <jc/Sync/SpinLock.h>
#include <jc/Exception.h>

namespace LockGuardTest {

    TEST(LockGuardTest, LockGuard) {
        
        {
            EventLock lk;
            {
                LockGuard lg(lk);
                EXPECT_TRUE(lk.IsLocked());
                EXPECT_FALSE(lk.TryLock());
            }
            EXPECT_FALSE(lk.IsLocked());
        }

        {
            NormalLock lk;
            {
                LockGuard lg(lk);
                EXPECT_TRUE(lk.IsLocked());
                EXPECT_FALSE(lk.TryLock());
                EXPECT_THROW(lk.Lock(), RuntimeException);
            }
            EXPECT_FALSE(lk.IsLocked());
        }

        {
            RecursiveLock lk;
            {
                LockGuard lg(lk);
                EXPECT_TRUE(lk.TryLock());
                lk.Unlock();
                {
                    LockGuard lg2(lk);
                    EXPECT_TRUE(lk.TryLock());
                    lk.Unlock();
                }
            }
        }

        {
            Semaphore lk(1, 1);
            {
                LockGuard lg(lk);
                EXPECT_TRUE(lk.IsAcquired());
                EXPECT_FALSE(lk.TryAcquire());
            }
            EXPECT_FALSE(lk.IsLocked());
        }



        {
            SpinLock lk;
            {
                LockGuard lg(lk);
                EXPECT_TRUE(lk.IsLocked());
                EXPECT_FALSE(lk.TryLock());
            }
            EXPECT_FALSE(lk.IsLocked());
        }

        {

            NormalRwLock rwlk;
            {
                RwLockGuard<IRwLock, RwLockMode::Write> lg(rwlk);
                EXPECT_TRUE(rwlk.IsWriteLocked());
            }

            EXPECT_FALSE(rwlk.IsWriteLocked());

            {
                RwLockGuard<IRwLock, RwLockMode::Read> lg1(rwlk);
                EXPECT_TRUE(rwlk.IsReadLocked());
                RwLockGuard<IRwLock, RwLockMode::Read> lg2(rwlk);
                EXPECT_TRUE(rwlk.IsReadLocked());
                RwLockGuard<IRwLock, RwLockMode::Read> lg3(rwlk);
                EXPECT_TRUE(rwlk.IsReadLocked());
            }

            EXPECT_FALSE(rwlk.IsReadLocked());
        }
    }



} // namespace LockGuardTest

#endif