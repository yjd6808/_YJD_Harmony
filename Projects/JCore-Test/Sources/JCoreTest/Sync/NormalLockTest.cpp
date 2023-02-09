#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/NormalLock.h>

#if TEST_NormalLockTest == ON

namespace NormalLockTest {

static int number = 0;
static NormalLock lk;

TEST(NormalLockTest, NormalLock) {
    number = 0;

    std::thread increaser{ []() {
        for (int i = 0; i < 200'000; i++) {
            lk.Lock();
            number++;
            number++;
            number++;
            lk.Unlock();
        }
    } };

    std::thread decreaser{ []() {
        for (int i = 0; i < 200'000; i++) {
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