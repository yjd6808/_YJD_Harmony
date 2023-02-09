/*
	작성자 : 윤정도
	뮤텍스 테스트
*/


#include <JCoreTest/CoreTest.h>

#if TEST_LockGuardTest == ON

#include <JCore/Sync/EventLock.h>
#include <JCore/Sync/NormalLock.h>
#include <JCore/Sync/NormalRwLock.h>
#include <JCore/Sync/RecursiveLock.h>
#include <JCore/Sync/Semaphore.h>
#include <JCore/Sync/SpinLock.h>
#include <JCore/Exception.h>

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