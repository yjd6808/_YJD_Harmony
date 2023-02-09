#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/EventLock.h>

#if TEST_EventLockTest == ON

namespace EventLockTest {

static int number = 0;
static EventLock lk;

TEST(EventLockTest, EventLock) {
    number = 0;

    std::thread increaser{ []() {
        for (int i = 0; i < 20'000; i++) {
            lk.Lock();
            number++;
            number++;
            number++;
            lk.Unlock();
        }
    } };

    std::thread decreaser{ []() {
        for (int i = 0; i < 20'000; i++) {
            lk.Lock();
            number--;
            number--;
            number--;
            lk.Unlock();
        }
    } };

    increaser.join();
    decreaser.join();

    EXPECT_TRUE(number == 0);
}

} // namespace NormalLockTest

#endif // #if TEST_NormalLockTest == ON