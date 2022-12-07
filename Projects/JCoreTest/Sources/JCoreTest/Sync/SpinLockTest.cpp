#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/SpinLock.h>

#if TEST_SpinLockTest == ON

namespace SpinLockTest {

static int number = 0;
static SpinLock lk;

TEST(SpinLockTest, SpinLock) {
    number = 0;

    std::thread increaser{ [] {
        for (int i = 0; i < 200'000; i++) {
            lk.Lock();
            number++;
            number++;
            number++;
            lk.Unlock();
        }
    }};

    std::thread decreaser{ [] {
        for (int i = 0; i < 200'000; i++) {
            lk.Lock();
            number--;
            number--;
            number--;
            lk.Unlock();
        }
    }};

    increaser.join();
    decreaser.join();

    EXPECT_TRUE(number == 0);
}

} // namespace SpinLockTest

#endif // #if TEST_SpinLockTest == ON