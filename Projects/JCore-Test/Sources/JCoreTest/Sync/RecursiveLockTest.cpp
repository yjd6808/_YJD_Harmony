#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/RecursiveLock.h>

#if TEST_RecursiveLockTest == ON

namespace RecursiveLockTest {

static int number = 0;
static RecursiveLock lk;

TEST(RecursiveLockTest, RecursiveLock) {
    number = 0;

    std::thread increaser{ []() {
        for (int i = 0; i < 200'000; i++) {
            lk.Lock();
            lk.Lock();
            number++;
            number++;
            number++;
            lk.Unlock();
            lk.Unlock();
        }
    } };

    std::thread decreaser{ []() {
        for (int i = 0; i < 200'000; i++) {
            lk.Lock();
            lk.Lock();
            number--;
            number--;
            number--;
            lk.Unlock();
            lk.Unlock();
        }
    } };

    increaser.join();
    decreaser.join();

    EXPECT_TRUE(number == 0);
}

} // namespace RecursiveLockTest

#endif // #if TEST_RecursiveLockTest == ON